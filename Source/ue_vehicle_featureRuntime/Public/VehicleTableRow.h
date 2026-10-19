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
