/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "GEMSTAR_Vehicle_Loader.h"

#include "Subsystems/GemstarSimulationSubsystem.h"
#include "VehicleManager.h"

UGEMSTAR_Vehicle_Loader::UGEMSTAR_Vehicle_Loader()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGEMSTAR_Vehicle_Loader::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	if (World == nullptr) {
		return;
	}

	UGemstarSimulationSubsystem* GemstarSimulationSubsystem = UGemstarSimulationSubsystem::Get(GetWorld());
	if (GemstarSimulationSubsystem == nullptr)
	{
		UE_LOG(Vehicle, Error, TEXT("ue_vehicle_feature: simulation subsystem unavailable"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();

	VehicleManager = World->SpawnActor<AVehicleManager>(AVehicleManager::StaticClass(), FTransform::Identity, SpawnParams);

	if (!IsValid(VehicleManager)) {
		UE_LOG(Vehicle, Error, TEXT("ue_vehicle_feature: could not spawn AVehicleManager"));
		return;
	}

	TMap<FString, FInnerConfig> config = GemstarSimulationSubsystem->RegisterManagerAndGetConfiguration(VehicleManager , "ue_vehicle_feature");
	
	VehicleManager->Configure(config, GemstarSimulationSubsystem);

	UE_LOG(Vehicle, Log, TEXT("Registered and configured simulation vehicles"));
}

void UGEMSTAR_Vehicle_Loader::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(VehicleManager)) {
		VehicleManager->Destroy();
		VehicleManager = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}
