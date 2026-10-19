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

#include "Interfaces/Sensor/SensorReflectionData.h"
#include "CoreMinimal.h"
#include "VehicleLog.h"
#include "GameFramework/Actor.h"
#include "SV_VehicleBaseExternal.generated.h"

UCLASS(BlueprintType)
class UE_VEHICLE_FEATURERUNTIME_API ASV_VehicleBaseExternal : public AActor, public ISensorReflectionData
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

	UPROPERTY(EditAnywhere)
	FVector currentVelocity = FVector::ZeroVector;

	virtual FVector GetVelocity() const override;
	virtual FString GetMaterialFilePath() override;
	virtual FVector calculateVelocity(float DeltaTime) override;

private:
	FVector oldPosition;
	bool firstTick = true;
	FString materialFilePath = "";
};
