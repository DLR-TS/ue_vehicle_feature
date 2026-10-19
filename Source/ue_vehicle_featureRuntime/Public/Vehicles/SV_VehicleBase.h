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

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "SV_VehicleBase.generated.h"


UCLASS(BlueprintType)
class UE_VEHICLE_FEATURERUNTIME_API ASV_VehicleBase : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASV_VehicleBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* StaticVehicleMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SimpleMesh;
};
