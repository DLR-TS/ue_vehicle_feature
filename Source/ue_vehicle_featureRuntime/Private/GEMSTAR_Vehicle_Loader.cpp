// Fill out your copyright notice in the Description page of Project Settings.


#include "GEMSTAR_Vehicle_Loader.h"

// Sets default values for this component's properties
UGEMSTAR_Vehicle_Loader::UGEMSTAR_Vehicle_Loader()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UGEMSTAR_Vehicle_Loader::BeginPlay()
{
	Super::BeginPlay();
	AActor* OwnerActor = GetOwner();
	AGEMSTARRoot* GemstarRoot = Cast<AGEMSTARRoot>(OwnerActor);
	if (GemstarRoot)
	{
		if (GemstarRoot->UseSimVehicles)
		{
			FTransform SpawnTransform;

			FActorSpawnParameters SpawnParams;
			AVehicleManager* SpawnedVehicleActor = GetWorld()->SpawnActor<AVehicleManager>(AVehicleManager::StaticClass(), SpawnTransform, SpawnParams);

			TMap<FString, FInnerConfig> config = GemstarRoot->RegisterManagerAndGetConfiguration(SpawnedVehicleActor);
			SpawnedVehicleActor->Configure(config, GemstarRoot);
			UE_LOG(LogTemp, Log, TEXT("Registered and configured Sim Vehicles"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error: Cast of GEMSTARRoot was not successful"));
	}
}


// Called every frame
void UGEMSTAR_Vehicle_Loader::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

