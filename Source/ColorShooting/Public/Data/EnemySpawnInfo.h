#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemySpawnInfo.generated.h"

class ACharacter;

USTRUCT(BlueprintType)
struct FEnemySpawnInfo : public FTableRowBase
{
    GENERATED_BODY()

public:
    /** The class of the enemy to spawn */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemySpawnInfo")
    TSubclassOf<ACharacter> EnemyClass;

    /** The time in seconds from the start of the stage to spawn this enemy */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemySpawnInfo")
    float SpawnTime;

    /** The location relative to the world origin to spawn the enemy */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemySpawnInfo")
    FVector SpawnLocation;
};
