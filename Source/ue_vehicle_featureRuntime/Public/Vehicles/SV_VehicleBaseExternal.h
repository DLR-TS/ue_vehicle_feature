// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SV_VehicleBaseExternal.generated.h"

UCLASS(BlueprintType)
class UE_VEHICLE_FEATURERUNTIME_API ASV_VehicleBaseExternal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASV_VehicleBaseExternal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void UpdatePosition(FTransform newPosition);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Category = Vehicle, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Mesh;
};
