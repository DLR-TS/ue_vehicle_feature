/*
 * Copyright (c) 2026 DLR e.V.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "Vehicles/SV_VehicleBase.h"

// Sets default values
ASV_VehicleBase::ASV_VehicleBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create a static mesh component
	SimpleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SimpleMeshComponent"));
}

// Called when the game starts or when spawned
void ASV_VehicleBase::BeginPlay()
{
	Super::BeginPlay();
	// Set the component's mesh
	SimpleMesh->SetStaticMesh(StaticVehicleMesh);
	RootComponent = SimpleMesh;
}

// Called every frame
void ASV_VehicleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
