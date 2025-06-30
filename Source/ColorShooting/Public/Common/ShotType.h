#pragma once

#include "CoreMinimal.h"
#include "ShotType.generated.h"

UENUM(BlueprintType)
enum class EShotType : uint8
{
	Red		UMETA(DisplayName = "Red"),
	Green	UMETA(DisplayName = "Green"),
	Blue	UMETA(DisplayName = "Blue")
};
