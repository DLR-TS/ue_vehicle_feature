// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GEMSTARRoot.h"

#include "Engine/DataTable.h"
#include "Vehicles/SV_VehicleBase.h"
#include "Vehicles/SV_VehicleBaseExternal.h"
#include "VehicleDataUpdate.h"
#include "VehicleDataDispatcher.h"
#include "VehicleTableRow.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AssetRegistry/AssetRegistryModule.h"
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

	FVehicleInstance() : Name(NAME_None), VehicleInstance(nullptr) {}
	FVehicleInstance(FName name, ASV_VehicleBaseExternal* vehicleInstance) : Name(name), VehicleInstance(vehicleInstance) {}
};

USTRUCT(BlueprintType)
struct FVehicleInstances
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray <FVehicleInstance> Instances;

	FVehicleInstances() : Instances() {}
};


UCLASS(BlueprintType)
class UE_VEHICLE_FEATURERUNTIME_API AVehicleManager : public AActor,
	public IVehicleDataUpdate,
	public IVehicleDataDispatcher,
	public IGEMSTARManager
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
	bool bKlickToUpdateVehicleMap = false;

private:
	TArray<FVehicleTableRow> VehicleDefinitionArray;
	AGEMSTARRoot* GEMSTARRoot;
	bool bDebug = false;
	TMap<FString, FString> ConfigurationNameToVehicleNameInScenario;

	void FillVehicleDefinitions(FString FileName = TEXT("/ue_vehicle_feature/Blueprints/VehicleDataTable.VehicleDataTable"));
	ASV_VehicleBaseExternal* SpawnVehicle(FVehicleDataUpdateData& data);
	TArray<FAssetData> GetAllAssetsInFolder(FString path);

public:
	AVehicleManager();
	virtual void Tick(float DeltaTime) override;
	void Configure(TMap<FString, FInnerConfig> Configurations, AGEMSTARRoot* root);

	UFUNCTION(BlueprintCallable)
	FVehicleTableRow FindClosestVehicleDefinitions(FName& Type, FVector& Size);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	void UpdateVehicleMap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void RegisterVehicleController_Implementation(FName& controller);
	virtual void VehicleDataUpdate_Implementation(FVehicleDataUpdateData& data) override;

	virtual bool MountOn_Implementation(const FString& nameByConfiguration, AActor* Actor) override;
};
