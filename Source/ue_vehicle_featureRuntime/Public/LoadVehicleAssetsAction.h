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
#include "GameFeatureAction.h"
#include "VehicleFeatureDefinition.h"
#include "LoadVehicleAssetsAction.generated.h"

UCLASS(EditInlineNew, meta = (DisplayName = "Load Vehicle Assets"))
class UE_VEHICLE_FEATURERUNTIME_API ULoadVehicleAssetsAction : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	// Harvested into the feature's bundle by UpdateAssetBundleData
	// see the .cpp for what it cannot reach
	UPROPERTY(EditAnywhere, Category = "Vehicles", meta = (AssetBundles = "Client"))
	TSoftObjectPtr<UVehicleFeatureDefinition> Definition;

#if WITH_EDITORONLY_DATA
	virtual void AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData) override;
#endif
};
