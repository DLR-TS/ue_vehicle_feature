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
#include "Engine/DataAsset.h"
#include "VehicleFeatureDefinition.generated.h"

class ASV_VehicleBaseExternal;

UCLASS(BlueprintType)
class UE_VEHICLE_FEATURERUNTIME_API UVehicleFeatureDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const FPrimaryAssetType PrimaryAssetType;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	// Spawnable vehicle bodies, keyed at runtime by asset name to match VehicleAsset in the table
	UPROPERTY(EditDefaultsOnly, Category = "Vehicles")
	TArray<TSoftClassPtr<ASV_VehicleBaseExternal>> Vehicles;
};
