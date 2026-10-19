/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "VehicleManager.h"

#include "Engine/AssetManager.h"
#include "VehicleFeatureDefinition.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"


// Sets default values
AVehicleManager::AVehicleManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVehicleManager::BeginPlay()
{
	Super::BeginPlay();
	UpdateVehicleMap();
}

// Called every frame
void AVehicleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ClearVehiclesWithoutUpdates();
}

// Called when the game Ends
void AVehicleManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	WriteVehicleMappingLogToFile();

	if (IsValid(GemstarSimulationSubsystem)) {
		GemstarSimulationSubsystem->UnregisterManager(this);
		GemstarSimulationSubsystem = nullptr;
	}

	for (TPair<FName, FVehicleInstances>& Controller : SpawnedVehicles){

		for (FVehicleInstance& Instance : Controller.Value.Instances){

			if (IsValid(Instance.VehicleInstance)){
				Instance.VehicleInstance->DetachAllSceneComponents(
					Instance.VehicleInstance->GetRootComponent(),
					FDetachmentTransformRules::KeepWorldTransform);
				Instance.VehicleInstance->Destroy();
			}
			Instance.VehicleInstance = nullptr;
		}
	}
	SpawnedVehicles.Reset();
	ConfigurationNameToVehicleNameInScenario.Reset();

	Super::EndPlay(EndPlayReason);
}

void AVehicleManager::Configure(TMap<FString, FInnerConfig> Configurations, UGemstarSimulationSubsystem* SimulationSubsystem) {
	GemstarSimulationSubsystem = SimulationSubsystem;

	if (Configurations.Num() > 1) {
		UE_LOG(Vehicle, Warning, TEXT("Point cloud visualizer. Multiple configs."));
	}
	for (const TPair<FString, FInnerConfig>& Pair : Configurations) {
		for (auto& entry : Pair.Value.Entries) {
			if (entry.Key == "Debug") {
				if (entry.Value == "True" || entry.Value == "true") {
					bDebug = true;
				}
			}
			else if (entry.Key == "Input") {
				FDataTarget dt;
				dt.Actor = this;
				ConnectInputs(&entry.Value, GemstarSimulationSubsystem, dt, GemstarInterfaceType::TRAFFICUPDATE);
				continue;
			}
			else if (entry.Key == "VehicleDataConfiguration") {
				VehicleDefinitionsPath = entry.Value;
				FillVehicleDefinitions(VehicleDefinitionsPath);
				continue;
			}
			else if (entry.Key == "VehicleMappingLog") {
				bWriteVehicleMappingLog = entry.Value.ToBool();
				continue;
			}
			else if (entry.Key == "VehicleMappingFileName") {
				VehicleMappingFileName = entry.Value;
				continue;
			}
			else if (entry.Key == "ClearAbandonedVehicles") {
				bClearAbandonedVehicles = entry.Value.ToBool();
				continue;
			}
			else {
				ConfigurationNameToVehicleNameInScenario.Add(entry);
			}
		}
	}
}

FVehicleTableRow AVehicleManager::FindClosestVehicleDefinition(FVehicleDataUpdateData& Data) {
	FVehicleTableRow vehicleProperties;

	if (VehicleDefinitionArray.Num() == 0) {

		if (VehicleDefinitionsPath.IsEmpty()) {
			FConfigFile CustomConfigFile;
			CustomConfigFile.Read(GemstarSimulationSubsystem->GetConfigurationPath());

			if(!CustomConfigFile.GetString(TEXT("ue_vehicle_feature"), TEXT("VehicleDataConfiguration"), VehicleDefinitionsPath)) {
				UE_LOG(Vehicle, Warning, TEXT("Failed to load VehicleDataConfiguration from INI file: %s"), *GemstarSimulationSubsystem->GetConfigurationPath());
				return vehicleProperties;
			}
		}
		FillVehicleDefinitions(VehicleDefinitionsPath);

		if (VehicleDefinitionArray.Num() == 0) {
			UE_LOG(Vehicle, Warning, TEXT("No Vehicle Definitions Data Table is configured! Will crash!"));
			return vehicleProperties;
		}
		UE_LOG(Vehicle, Warning, TEXT("No Vehicle Definitions Data Table is configured! Will crash!"));
		return vehicleProperties;
	}

	if (!Data.Name.IsNone()) {
		for (auto& vehicledefition : VehicleDefinitionArray) {
			if (vehicledefition.Name == Data.Name || vehicledefition.VehicleAsset == Data.Name) {
				UE_LOG(Vehicle, Log, TEXT("Matched vehicle name %s"), *Data.Name.ToString());
				return vehicledefition;
			}
		}
		UE_LOG(Vehicle, Log, TEXT("Can not match vehicle name %s"), *Data.Name.ToString());
	}

	if (!Data.VehicleClass.IsNone()) {
		for (auto& vehicledefition : VehicleDefinitionArray) {
			if (vehicledefition.Name == Data.VehicleClass || vehicledefition.VehicleAsset == Data.VehicleClass) {
				UE_LOG(Vehicle, Log, TEXT("Matched vehicle type %s to name %s or asset %s"),
					*Data.VehicleClass.ToString(), *vehicledefition.Name.ToString(), *vehicledefition.VehicleAsset.ToString());
				return vehicledefition;
			}
		}
		UE_LOG(Vehicle, Log, TEXT("Can not match vehicle type %s"), *Data.VehicleClass.ToString());
	}

	//find by size
	int minIndex = 0;
	float minSquaredDiff = FLT_MAX;
	for (int i = 0; i < VehicleDefinitionArray.Num(); i++) {
		FVector vdiff = VehicleDefinitionArray[i].OSI_Bounding_Box - Data.BoundingBox;
		float squareddiff = vdiff.X * vdiff.X + vdiff.Y * vdiff.Y + vdiff.Z * vdiff.Z;
		if (squareddiff < minSquaredDiff) {
			minIndex = i;
			minSquaredDiff = squareddiff;
		}
	}
	vehicleProperties = VehicleDefinitionArray[minIndex];
	UE_LOG(Vehicle, Log, TEXT("Matched vehicle by size. Minimum squared difference: %f"), minSquaredDiff);
	return vehicleProperties;
}

void AVehicleManager::FillVehicleDefinitions(FString FileName) {
	FString ContextString;
	TArray<FName> RowNames;

	VehicleDefinitions = LoadVehicleDefinitions(*FileName);

	if (VehicleDefinitions == NULL) {
		UE_LOG(Vehicle, Warning, TEXT("No VehicleDefinitions available!"));
		return;
	}

	RowNames = VehicleDefinitions->GetRowNames();

	for (auto& Name : RowNames)
	{
		FVehicleTableRow* Row = VehicleDefinitions->FindRow<FVehicleTableRow>(Name, ContextString);
		if (Row)
		{
			VehicleDefinitionArray.Add(*Row);
		}
	}
}

UDataTable* AVehicleManager::LoadVehicleDefinitions(FString FileName) {
	const FString FilePath = FPaths::IsRelative(FileName) ? (GetVehicleRootDirectory() / FileName) : FileName;

	FString JsonString;

	if (!FFileHelper::LoadFileToString(JsonString, *FilePath)) {
		UE_LOG(Vehicle, Warning, TEXT("Could not load VehicleDefinitions JSON file: %s"), *FilePath);
		return nullptr;
	}

	UDataTable* NewVehicleDefinitions = NewObject<UDataTable>();
	NewVehicleDefinitions->RowStruct = FVehicleTableRow::StaticStruct();

	const TArray<FString> Issues = NewVehicleDefinitions->CreateTableFromJSONString(JsonString);

	for (const FString& Issue : Issues) {
		UE_LOG(Vehicle, Warning, TEXT("VehicleDefinitions JSON issue: %s"), *Issue);
	}

	if (NewVehicleDefinitions->GetRowMap().Num() == 0) {
		UE_LOG(Vehicle, Warning, TEXT("VehicleDefinitions JSON contained no rows: %s"), *FilePath);
	}

	return NewVehicleDefinitions;
}

void AVehicleManager::RegisterVehicleController_Implementation(FName& Controller) {
	if (SpawnedVehicles.Contains(Controller)) {
		UE_LOG(Vehicle, Log, TEXT("Register new vehicle controller with same name: %s"), *Controller.ToString());
		return;
	}

	FVehicleInstances instances;
	SpawnedVehicles.Add(Controller, instances);
}

void AVehicleManager::VehicleDataUpdate_Implementation(FVehicleDataUpdateData& Data) {
	if (Data.Name.IsNone()) {
		UE_LOG(Vehicle, Log, TEXT("Update vehicle, but no identifier is given. Skip."));
		return;
	}

	FVehicleInstances* vehiclesFromController = SpawnedVehicles.Find(Data.Provider);
	if (vehiclesFromController == nullptr) {
		UE_LOG(Vehicle, Log, TEXT("No controller registered with this name: %s Creating a new controller."), *Data.Provider.ToString());
		RegisterVehicleController_Implementation(Data.Provider);
		vehiclesFromController = SpawnedVehicles.Find(Data.Provider);
	}
	ASV_VehicleBaseExternal* base;
	FVehicleInstance* vehicleInstance = vehiclesFromController->Instances.FindByPredicate([&Data](const FVehicleInstance& instance)
		{
			return instance.Name.IsEqual(Data.Name);
		});
	if (vehicleInstance == nullptr) {
		//vehicle does not exist
		base = SpawnVehicle(Data);
		vehiclesFromController->Instances.Add({ Data.Name, base });
	}
	else {
		base = vehicleInstance->VehicleInstance;
		vehicleInstance->LastUpdatedInFrame = GFrameCounter;
	}

	//update position, a failed spawn stays cached as null so it is not retried every update
	if (IsValid(base)) {
		base->SetActorTransform(Data.Position, false, nullptr, ETeleportType::TeleportPhysics);
	}
	//TODO Z position
}

ASV_VehicleBaseExternal* AVehicleManager::SpawnVehicle(FVehicleDataUpdateData& data) {
	ASV_VehicleBaseExternal* actor = nullptr;
	auto vehicleDefinition = FindClosestVehicleDefinition(data);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TSubclassOf<ASV_VehicleBaseExternal>* vehicleAssetPtr = AvailableVehicles.Find(vehicleDefinition.VehicleAsset);
	if (vehicleAssetPtr && *vehicleAssetPtr)
	{
		UClass* vehicleClass = **vehicleAssetPtr;
		FRotator rotation = data.Position.GetRotation().Rotator();

		actor = GetWorld()->SpawnActor<ASV_VehicleBaseExternal>(
			vehicleClass,
			data.Position,
			SpawnInfo
		);
		// collect information about the spawned traffic actor
		if (actor)
		{
			FVehicleMappingLogEntry Entry;
			Entry.Provider = data.Provider;
			Entry.ScenarioPlayerName = data.Name;
			Entry.ScenarioPlayerType = data.VehicleClass;
			Entry.VehicleAssetName = vehicleDefinition.VehicleAsset;
			Entry.OsiObjectId = actor->GetUniqueID();

			UE_LOG(Vehicle, Log, TEXT("Spawned vehicle. Name: %s, Vehicle Class: %s, Asset: %s, OSI ID: %i"),
				*data.Name.ToString(), *data.VehicleClass.ToString(),
				*vehicleDefinition.VehicleAsset.ToString(), Entry.OsiObjectId);

			VehicleMappingLog.Add(Entry);
		} else {
			UE_LOG(Vehicle, Warning, TEXT("Could not spawn vehicle!"));
		}
	} else {
		UE_LOG(Vehicle, Warning, TEXT("Could not find available vehicle!"));
	}

	return actor;
}

#if WITH_EDITOR
void AVehicleManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AVehicleManager, bClickToUpdateVehicleMap))
	{
		if (bClickToUpdateVehicleMap)
		{
			UpdateVehicleMap();
			bClickToUpdateVehicleMap = false;
		}
	}
}
#endif

void AVehicleManager::ClearVehiclesWithoutUpdates() {

	if (!bClearAbandonedVehicles && ClearedInFrame == GFrameCounter) {
		return;
	}

	ClearedInFrame = GFrameCounter;
	for (auto& VehicleController : SpawnedVehicles) {
		for (int i = VehicleController.Value.Instances.Num() - 1; i >= 0; i--) {
			if (VehicleController.Value.Instances[i].LastUpdatedInFrame != GFrameCounter) {
				if (IsValid(VehicleController.Value.Instances[i].VehicleInstance)) {
					UE_LOG(Vehicle, Log, TEXT("Remove vehicle from simulation: %s"),
						*VehicleController.Value.Instances[i].Name.ToString());
					VehicleController.Value.Instances[i].VehicleInstance->DetachAllSceneComponents(
						VehicleController.Value.Instances[i].VehicleInstance->GetRootComponent(),
						FDetachmentTransformRules::KeepWorldTransform);
					VehicleController.Value.Instances[i].VehicleInstance->Destroy();
					VehicleController.Value.Instances.RemoveAt(i);
				}
			}
		}
	}
}

void AVehicleManager::UpdateVehicleMap()
{
	AvailableVehicles.Empty();

	// Preferred: the feature definition, which is bundled and therefore cooked
	const FPrimaryAssetId DefinitionId(
		UVehicleFeatureDefinition::PrimaryAssetType, TEXT("DA_VehicleFeatureDefinition"));
	UAssetManager& AssetManager = UAssetManager::Get();

	const UVehicleFeatureDefinition* Definition =
		Cast<UVehicleFeatureDefinition>(AssetManager.GetPrimaryAssetObject(DefinitionId));
	
	if (!IsValid(Definition)) {
		Definition = Cast<UVehicleFeatureDefinition>(
			AssetManager.GetPrimaryAssetPath(DefinitionId).TryLoad());
	}

	if (IsValid(Definition) && Definition->Vehicles.Num() > 0) {

		for (const TSoftClassPtr<ASV_VehicleBaseExternal>& Soft : Definition->Vehicles) {

			if (UClass* Loaded = Soft.LoadSynchronous()) {
				// Key by the asset name the table rows use: a class reference's name carries a
				// _C suffix the rows do not
				FString KeyName = Soft.GetAssetName();
				KeyName.RemoveFromEnd(TEXT("_C"));
				AvailableVehicles.Add(FName(*KeyName), Loaded);
			}
		}
		UE_LOG(Vehicle, Log, TEXT("Found total: %d (from the vehicle feature definition)"),
			AvailableVehicles.Num());
		return;
	}

	// Legacy path: scan the folder. Convenient, but invisible to the cooker
	UE_LOG(Vehicle, Warning,
		TEXT("GEMSTAR: no vehicle feature definition resolved; falling back to the folder scan. "
			"These assets are not referenced by anything the cooker can see."));

	TArray<FAssetData> AssetList;
	AssetList = GetAllAssetsInFolder(TEXT("/ue_vehicle_feature/Blueprints/Vehicles"));

	for (const FAssetData& Asset : AssetList)
	{
		FString ClassPath = Asset.GetObjectPathString() + TEXT("_C");
		UClass* AssetClass = LoadObject<UClass>(nullptr, *ClassPath);
		UE_LOG(Vehicle, Log, TEXT("Check asset: %s (%s)"), *Asset.AssetName.ToString(), *ClassPath);

		if (AssetClass && AssetClass->IsChildOf(ASV_VehicleBaseExternal::StaticClass()))
		{
			UE_LOG(Vehicle, Log, TEXT("Add available vehicle: %s"), *Asset.AssetName.ToString());
			AvailableVehicles.Add(Asset.AssetName, AssetClass);
		}
	}
	UE_LOG(Vehicle, Log, TEXT("Found total: %d"), AvailableVehicles.Num());
}

TArray<FAssetData> AVehicleManager::GetAllAssetsInFolder(FString path)
{
	TArray<FAssetData> AssetList;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().GetAssetsByPath(*path, AssetList, false);
	return AssetList;
}

bool AVehicleManager::MountOn_Implementation(const FString& nameByConfiguration, AActor* Actor) {
	const FString* nameInScenario = ConfigurationNameToVehicleNameInScenario.Find(nameByConfiguration);
	if (!nameInScenario) {
		return false;
	}
	for (auto& vehicleControllers : SpawnedVehicles) {
		for (auto& instances : vehicleControllers.Value.Instances) {
			if (nameInScenario->Compare(instances.Name.ToString(), ESearchCase::IgnoreCase) == 0) {
				Actor->AttachToActor(instances.VehicleInstance, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
				return true;
			}
		}
	}

	return false;
}

FString AVehicleManager::ToCSVField(const FString& Value) const {
	FString Escaped = Value;
	Escaped.ReplaceInline(TEXT("\""), TEXT("\"\""));
	return FString::Printf(TEXT("\"%s\""), *Escaped);
}

void AVehicleManager::WriteVehicleMappingLogToFile() {
	if (!bWriteVehicleMappingLog) {
		UE_LOG(Vehicle, Warning, TEXT("No vehicle mapping log written!"));
		return;
	}

	const FString OutputDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Output"));
	IFileManager::Get().MakeDirectory(*OutputDir, true);
	FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
	const FString FileName = FString::Printf(TEXT("%s_%s"), *Timestamp, *VehicleMappingFileName);

	const FString OutputFile = FPaths::Combine(OutputDir, FileName);

	FString Csv;
	Csv += TEXT("Provider, ScenarioPlayerName, ScenarioPlayerType, VehicleAssetName, OsiObjectId\n");
	for (const FVehicleMappingLogEntry& Entry : VehicleMappingLog) {
		Csv += FString::Printf(TEXT("%s, %s, %s, %s, %lld\n"),
			*ToCSVField(Entry.Provider.ToString()),
			*ToCSVField(Entry.ScenarioPlayerName.ToString()),
			*ToCSVField(Entry.ScenarioPlayerType.ToString()),
			*ToCSVField(Entry.VehicleAssetName.ToString()),
			Entry.OsiObjectId
		);
	}
	const bool bSaved = FFileHelper::SaveStringToFile(Csv, *OutputFile);
	if (bSaved) {
		UE_LOG(Vehicle, Log, TEXT("Vehicle mapping log written to: %s"), *OutputFile);
	}
	else {
		UE_LOG(Vehicle, Error, TEXT("Somthing went wrong. No vehicle mapping log written to: %s"), *OutputFile);
	}
}

FString AVehicleManager::GetVehicleRootDirectory()
{
	const FString ProjectDir = FPaths::ProjectDir();
	FString VehicleDir = FPaths::Combine(
		ProjectDir,
		TEXT("Simulation"),
		TEXT("Vehicle")
	);
	VehicleDir = FPaths::ConvertRelativePathToFull(VehicleDir);
	FPaths::NormalizeDirectoryName(VehicleDir);
	return VehicleDir;
}
