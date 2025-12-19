// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "VehicleManager.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GEMSTAR_Vehicle_Loader.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE_VEHICLE_FEATURERUNTIME_API UGEMSTAR_Vehicle_Loader : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGEMSTAR_Vehicle_Loader();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
