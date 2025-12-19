// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "UObject/NoExportTypes.h"
#include "VehicleTableRow.generated.h"

/** Vehicle Values */
USTRUCT(BlueprintType)
struct FVehicleTableRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	/** Name of the vehicle */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Name"))
	FName Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "VehicleAsset"))
	FName VehicleAsset;

	/** Bounding box center to middle of rear axis */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "OSI_BB_Center_To_Rear"))
	FVector OSI_BB_Center_To_Rear = FVector::ZeroVector;

	/** Bounding box center to middle to front axis */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "OSI_BB_Center_To_Front"))
	FVector OSI_BB_Center_To_Front = FVector::ZeroVector;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "OSI_Ground_Clearance"))
	double OSI_Ground_Clearance = 0.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "OSI_Number_Wheels"))
	int32 OSI_Number_Wheels = 4;

	/** Bounding box without mirrors */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "OSI_Bounding_Box"))
	FVector OSI_Bounding_Box = FVector::ZeroVector;
};
