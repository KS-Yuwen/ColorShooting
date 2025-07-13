#pragma once

#include "CoreMinimal.h"
#include "ShotType.generated.h"

UENUM(BlueprintType)
enum class EShotType : uint8
{
	Red		UMETA(DisplayName = "Red"),
	Green	UMETA(DisplayName = "Green"),
	Blue	UMETA(DisplayName = "Blue"),
	Max		UMETA(Hidden), // This is used to determine the maximum value of the enum
};
