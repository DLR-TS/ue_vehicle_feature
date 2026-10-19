/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#pragma once

#include "Subsystems/GemstarSimulationSubsystem.h"

#include "Engine/DataTable.h"
#include "Vehicles/SV_VehicleBase.h"
#include "Vehicles/SV_VehicleBaseExternal.h"
#include "Interfaces/Movement/VehicleDataUpdate.h"
#include "Interfaces/Movement/VehicleDataDispatcher.h"
#include "VehicleTableRow.h"
#include "VehicleLog.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/DateTime.h"
#include "HAL/FileManager.h"
#include "VehicleManager.generated.h"

USTRUCT(BlueprintType)
struct FVehicleInstance
{
	GENERATED_BODY()

	// Name of the vehicle object
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	// vehicle object
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ASV_VehicleBaseExternal* VehicleInstance;

	uint64 LastUpdatedInFrame = GFrameCounter;

	FVehicleInstance() : Name(NAME_None), VehicleInstance(nullptr) {}
	FVehicleInstance(FName Name, ASV_VehicleBaseExternal* VehicleInstance) : Name(Name), VehicleInstance(VehicleInstance) {}
};

USTRUCT(BlueprintType)
struct FVehicleInstances
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray <FVehicleInstance> Instances;

	FVehicleInstances() : Instances() {}
};

USTRUCT(BlueprintType)
struct FVehicleMappingLogEntry
{
	GENERATED_BODY()

	//scenario player provider
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Provider;

	// Name of the scenario player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ScenarioPlayerName;

	// Type of the scenario player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ScenarioPlayerType;

	// Name of the spawned vehicle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName VehicleAssetName;

	// Object ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 OsiObjectId = 0;
};

UCLASS(BlueprintType)
class UE_VEHICLE_FEATURERUNTIME_API AVehicleManager : public AActor,
	public IVehicleDataUpdate,
	public IVehicleDataDispatcher,
	public IGemstarManager
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* VehicleDefinitions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, FVehicleInstances> SpawnedVehicles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, TSubclassOf<ASV_VehicleBaseExternal>> AvailableVehicles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bClickToUpdateVehicleMap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GEMSTAR")
	FString VehicleDefinitionsPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bClearAbandonedVehicles = true;

private:
	TArray<FVehicleTableRow> VehicleDefinitionArray;

	UPROPERTY()
	TObjectPtr<UGemstarSimulationSubsystem> GemstarSimulationSubsystem;
	
	bool bDebug = false;
	TMap<FString, FString> ConfigurationNameToVehicleNameInScenario;

	void FillVehicleDefinitions(FString FileName);

	UDataTable* LoadVehicleDefinitions(FString FileName);

	ASV_VehicleBaseExternal* SpawnVehicle(FVehicleDataUpdateData& Data);
	TArray<FAssetData> GetAllAssetsInFolder(FString Path);

	uint64 ClearedInFrame = 0;
	void  ClearVehiclesWithoutUpdates();

	bool bWriteVehicleMappingLog = false;
	FString VehicleMappingFileName = TEXT("GEMSTAR_vehicle_mapping.csv");

	TArray <FVehicleMappingLogEntry> VehicleMappingLog;

	void WriteVehicleMappingLogToFile();
	FString ToCSVField(const FString& Value) const;
	FString GetVehicleRootDirectory();

public:
	AVehicleManager();
	virtual void Tick(float DeltaTime) override;
	void Configure(TMap<FString, FInnerConfig> Configurations, UGemstarSimulationSubsystem* SimulationSubsystem);

	UFUNCTION(BlueprintCallable)
	FVehicleTableRow FindClosestVehicleDefinition(FVehicleDataUpdateData& Data);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	void UpdateVehicleMap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void RegisterVehicleController_Implementation(FName& Controller);
	virtual void VehicleDataUpdate_Implementation(FVehicleDataUpdateData& Data) override;

	virtual bool MountOn_Implementation(const FString& NameByConfiguration, AActor* Actor) override;
};
