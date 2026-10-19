/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "ue_vehicle_featureRuntimeModule.h"
#include "AssetRegistry/IAssetRegistry.h"

#define LOCTEXT_NAMESPACE "Fue_vehicle_featureRuntimeModule"

void Fue_vehicle_featureRuntimeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory;
	// the exact timing is specified in the .uplugin file per-module

    // In standalone mode in editor build (when right-click .uproject and select Launch game)
        // the AssetRegistry does not automatically scan for some odd reason.
        // Try to isolate this specific case and apply bandaid fix
#if WITH_EDITOR
    if (!GIsEditor && !IsRunningCommandlet())
    {
        TArray<FString> ContentPaths;
        FPackageName::QueryRootContentPaths(ContentPaths);

        //NOTE: Longest scan is "/Engine/", followed by "/Game/" and "/Niagara/"
        // if others are not needed, scan only "/Game/" to gain time

        const auto t = FPlatformTime::Seconds();

        IAssetRegistry::Get()->ScanPathsSynchronous(ContentPaths);

        UE_LOG(LogLoad, Log, TEXT("AssetRegistry ScanPathsSynchronous took %f seconds"), FPlatformTime::Seconds() - t);
    }
    else {
        UE_LOG(LogLoad, Log, TEXT("Not in editor or cmdlet"));
    }
#else
    UE_LOG(LogLoad, Log, TEXT("Not in editor"));
#endif
}

void Fue_vehicle_featureRuntimeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.
	// For modules that support dynamic reloading, we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(Fue_vehicle_featureRuntimeModule, ue_vehicle_featureRuntime)
