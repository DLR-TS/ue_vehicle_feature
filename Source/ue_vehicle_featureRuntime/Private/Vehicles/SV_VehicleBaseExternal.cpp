/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "Vehicles/SV_VehicleBaseExternal.h"

// Sets default values
ASV_VehicleBaseExternal::ASV_VehicleBaseExternal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VehicleMeshComponent"));
	Mesh->SetSimulatePhysics(false);
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ASV_VehicleBaseExternal::BeginPlay()
{
	Super::BeginPlay();
	oldPosition = GetActorLocation();
	currentVelocity = FVector::ZeroVector;

}

// Called every frame
void ASV_VehicleBaseExternal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector oldMemory = oldPosition;
	//internal speed calculation
	if (firstTick) {
		firstTick = false;
		currentVelocity = FVector::ZeroVector;
	}
	else {
		currentVelocity = calculateVelocity(DeltaTime);
		oldPosition = GetActorLocation();
	}
	//Troubleshooting info log
	FVector dLoc = oldPosition - oldMemory;
	//UE_LOG(Vehicle, Log, TEXT("Name: %s, dLocV: %s, d: %.5f, vV: %s, v: %.2f, dLog: %.2f"), *GetActorNameOrLabel(), *dLoc.ToCompactString(), DeltaTime, *currentVelocity.ToCompactString(), currentVelocity.Length(), dLoc.Length());
}

void ASV_VehicleBaseExternal::UpdatePosition(FTransform newPosition)
{
	//position update policies here TODO

	SetActorLocation(newPosition.GetLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation(newPosition.GetRotation(), ETeleportType::TeleportPhysics);

	//Z correction TODO
}

FVector ASV_VehicleBaseExternal::GetVelocity() const
{
	return currentVelocity;
}

FString ASV_VehicleBaseExternal::GetMaterialFilePath()
{
	return materialFilePath;
}

FVector ASV_VehicleBaseExternal::calculateVelocity(float DeltaTime)
{
	return (GetActorLocation() - oldPosition) / DeltaTime;
}
