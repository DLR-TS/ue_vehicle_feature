// Copyright Epic Games, Inc. All Rights Reserved.

#include "ue_vehicle_featureRuntimeModule.h"

#define LOCTEXT_NAMESPACE "Fue_vehicle_featureRuntimeModule"

void Fue_vehicle_featureRuntimeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory;
	// the exact timing is specified in the .uplugin file per-module
}

void Fue_vehicle_featureRuntimeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.
	// For modules that support dynamic reloading, we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(Fue_vehicle_featureRuntimeModule, ue_vehicle_featureRuntime)
