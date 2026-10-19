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
#include "Components/ActorComponent.h"
#include "GEMSTAR_Vehicle_Loader.generated.h"

class AVehicleManager;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE_VEHICLE_FEATURERUNTIME_API UGEMSTAR_Vehicle_Loader : public UActorComponent
{
	GENERATED_BODY()

public:
	UGEMSTAR_Vehicle_Loader();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	TObjectPtr<AVehicleManager> VehicleManager;
};
