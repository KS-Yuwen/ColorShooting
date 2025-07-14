#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameConstant.generated.h"

USTRUCT(BlueprintType)
struct FGameConstant : public FTableRowBase
{
	GENERATED_BODY()

public:
	FGameConstant() : Value(0) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameConstant")
	int32 Value;
};
