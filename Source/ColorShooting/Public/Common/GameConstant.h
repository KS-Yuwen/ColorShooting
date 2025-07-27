#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameConstant.generated.h"

UENUM(BlueprintType)
enum class EConstantType : uint8
{
	Int,
	Float
};

USTRUCT(BlueprintType)
struct FGameConstant : public FTableRowBase
{
	GENERATED_BODY()

public:
	FGameConstant() : IntValue(0), FloatValue(0.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameConstant")
	EConstantType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameConstant")
	int32 IntValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameConstant")
	float FloatValue;
};
