/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "LoadVehicleAssetsAction.h"

#include "GameFeaturesSubsystemSettings.h"
#include "VehicleFeatureDefinition.h"

#if WITH_EDITORONLY_DATA
void ULoadVehicleAssetsAction::AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData)
{
	Super::AddAdditionalAssetBundleData(AssetBundleData);

	if (Definition.IsNull()) {
		return;
	}

	// Definition path itself is declared by meta=(AssetBundles) on the property
	// Actions is an Instanced array
	// InitializeAssetBundlesFromMetadata walks into this object and finds it
	//
	// Metadata doesn't follow soft references inside other assets
	// Vehicles are stored as TSoftClassPtr
	// Bundle loading doesn't recurse across assets
	//The loop ensures vehicles are loaded before falling back to LoadSynchronous
	const UVehicleFeatureDefinition* Loaded = Definition.LoadSynchronous();
	if (Loaded == nullptr)
	{
		return;
	}

	for (const TSoftClassPtr<ASV_VehicleBaseExternal>& LoadedVehicle : Loaded->Vehicles)
	{
		if (!LoadedVehicle.IsNull())
		{
			AssetBundleData.AddBundleAssetTruncated(
				UGameFeaturesSubsystemSettings::LoadStateClient, LoadedVehicle.ToSoftObjectPath());
		}
	}
}
#endif
