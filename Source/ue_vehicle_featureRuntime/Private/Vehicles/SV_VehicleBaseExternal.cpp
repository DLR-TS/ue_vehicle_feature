// Fill out your copyright notice in the Description page of Project Settings.


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
	
}

// Called every frame
void ASV_VehicleBaseExternal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASV_VehicleBaseExternal::UpdatePosition(FTransform newPosition)
{
	//position update poilcies here TODO

	SetActorLocation(newPosition.GetLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation(newPosition.GetRotation(), ETeleportType::TeleportPhysics);

	//Z correction TODO
}