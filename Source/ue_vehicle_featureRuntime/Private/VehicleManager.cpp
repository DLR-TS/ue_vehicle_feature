// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleManager.h"

// Sets default values
AVehicleManager::AVehicleManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

}

void AVehicleManager::Configure(TMap<FString, FInnerConfig> Configurations, AGEMSTARRoot* root) {
	GEMSTARRoot = root;
	if (Configurations.Num() > 1) {
		UE_LOG(LogTemp, Warning, TEXT("Point cloud visualizer. Multiple configs."));
	}
	for (const TPair<FString, FInnerConfig>& Pair : Configurations) {
		for (auto& entry : Pair.Value.Entries) {
			if (entry.Key == "Debug") {
				if (entry.Value == "True" || entry.Value == "true") {
					bDebug = true;
				}
			}
			else if (entry.Key == "Input") {
				ConnectInputs(&entry.Value, root, this, GEMSTARInterfaceType::TRAFFICUPDATE);
				continue;
			}
			else if (entry.Key == "TypeMapping") {
				FillVehicleDefinitions(entry.Value);
				continue;
			}
			else {
				ConfigurationNameToVehicleNameInScenario.Add(entry);
			}
		}
	}
}

FVehicleTableRow AVehicleManager::FindClosestVehicleDefinitions(FName& Type, FVector& Size) {
	FVehicleTableRow vehicleProperties;
	if (VehicleDefinitionArray.Num() == 0) {
		FillVehicleDefinitions();
		if (VehicleDefinitionArray.Num() == 0) {
			UE_LOG(LogTemp, Warning, TEXT("No Vehicle Definitions Data Table is configured! Will crash!"));
			return vehicleProperties;
		}
	}

	if (!Type.IsNone()) {
		for (auto& vehicledefition : VehicleDefinitionArray) {
			if (vehicledefition.Name == Type || vehicledefition.VehicleAsset == Type) {
				return vehicledefition;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Can not match vehicle type %s"), *Type.ToString());
	}

	//find by size
	int minIndex = 0;
	int minSquaredDiff = INT32_MAX;
	for (int i = 0; i < VehicleDefinitionArray.Num(); i++) {
		FVector vdiff = VehicleDefinitionArray[i].OSI_Bounding_Box - Size;
		float squareddiff = vdiff.X * vdiff.X + vdiff.Y * vdiff.Y + vdiff.Z * vdiff.Z;
		if (squareddiff < minSquaredDiff) {
			minIndex = i;
			minSquaredDiff = squareddiff;
		}
	}
	vehicleProperties = VehicleDefinitionArray[minIndex];

	return vehicleProperties;
}

void AVehicleManager::FillVehicleDefinitions(FString FileName) {
	FString ContextString;
	TArray<FName> RowNames;

	VehicleDefinitions = LoadObject<UDataTable>(nullptr, *FileName);

	if (VehicleDefinitions == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("No VehicleDefinitions available!"));
		return;
	}

	RowNames = VehicleDefinitions->GetRowNames();

	for (auto& name : RowNames)
	{
		FVehicleTableRow* row = VehicleDefinitions->FindRow<FVehicleTableRow>(name, ContextString);
		if (row)
		{
			VehicleDefinitionArray.Add(*row);
		}
	}
}

void AVehicleManager::RegisterVehicleController_Implementation(FName& controller) {
	if (SpawnedVehicles.Contains(controller)) {
		UE_LOG(LogTemp, Log, TEXT("Register new vehicle controller with same name: %s"), *controller.ToString());
		return;
	}

	FVehicleInstances instances;
	SpawnedVehicles.Add(controller, instances);
}

void AVehicleManager::VehicleDataUpdate_Implementation(FVehicleDataUpdateData& data) {
	if (data.Name.IsNone()) {
		UE_LOG(LogTemp, Log, TEXT("Update vehicle, but no identifier is given. Skip."));
		return;
	}

	FVehicleInstances* vehiclesFromController = SpawnedVehicles.Find(data.Provider);
	if (vehiclesFromController == nullptr) {
		UE_LOG(LogTemp, Log, TEXT("No controller registered with this name. % s Creating a controller now."), *data.Provider.ToString());

		RegisterVehicleController_Implementation(data.Provider);
		vehiclesFromController = SpawnedVehicles.Find(data.Provider);
	}

	ASV_VehicleBaseExternal* base;
	FVehicleInstance* vehicleInstance = vehiclesFromController->Instances.FindByPredicate([&data](const FVehicleInstance& instance)
		{
			return instance.Name.IsEqual(data.Name);
		});
	if (vehicleInstance == nullptr) {
		//vehicle does not exist
		base = SpawnVehicle(data);
		vehiclesFromController->Instances.Add({ data.Name, base });
	}
	else {
		base = vehicleInstance->VehicleInstance;
	}

	//update position
	base->SetActorTransform(data.Position, false, nullptr, ETeleportType::TeleportPhysics);
	//TODO Z position
}

ASV_VehicleBaseExternal* AVehicleManager::SpawnVehicle(FVehicleDataUpdateData& data) {
	ASV_VehicleBaseExternal* actor = nullptr;
	auto vehicleDefinition = FindClosestVehicleDefinitions(data.VehicleClass, data.BoundingBox);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TSubclassOf<ASV_VehicleBaseExternal> vehicleAsset = *AvailableVehicles.Find(vehicleDefinition.VehicleAsset);
	if (vehicleAsset && *vehicleAsset)
	{
		UClass* vehicleClass = *vehicleAsset;
		FRotator rotation = data.Position.GetRotation().Rotator();

		actor = GetWorld()->SpawnActor<ASV_VehicleBaseExternal>(
			vehicleClass,
			data.Position,
			SpawnInfo
		);
	}

	return actor;
}

#if WITH_EDITOR
void AVehicleManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AVehicleManager, bKlickToUpdateVehicleMap))
	{
		if (bKlickToUpdateVehicleMap)
		{
			UpdateVehicleMap();
			bKlickToUpdateVehicleMap = false;
		}
	}
}
#endif

void AVehicleManager::UpdateVehicleMap()
{
	TArray<FAssetData> AssetList;
	AssetList = GetAllAssetsInFolder(TEXT("/ue_vehicle_feature/Blueprints/Vehicles"));
	AvailableVehicles.Empty();

	for (const FAssetData& Asset : AssetList)
	{
		FString ClassPath = Asset.ObjectPath.ToString() + TEXT("_C");
		UClass* AssetClass = LoadObject<UClass>(nullptr, *ClassPath);
		if (AssetClass && AssetClass->IsChildOf(ASV_VehicleBaseExternal::StaticClass()))
		{
			//FName AssetName = Asset.AssetName;
			UE_LOG(LogTemp, Log, TEXT("Add available vehicle: %s"), *Asset.AssetName.ToString());
			AvailableVehicles.Add(Asset.AssetName, AssetClass);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Found total: %d"), AvailableVehicles.Num());
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
