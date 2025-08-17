
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoundDataAsset.generated.h"

class USoundBase;

/**
 * Data asset to hold sound information.
 */
UCLASS(BlueprintType)
class COLORSHOOTING_API USoundDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** The sound to play. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundBase> Sound;
};
