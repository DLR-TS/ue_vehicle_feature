// Fill out your copyright notice in the Description page of Project Settings.


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
