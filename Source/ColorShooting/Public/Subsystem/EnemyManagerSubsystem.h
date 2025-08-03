// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/EnemySpawnInfo.h" // Include for spawn info struct
#include "EnemyManagerSubsystem.generated.h"

class AEnemyCharacter;
class UDataTable;

/**
 * 
 */
UCLASS(BlueprintType)
class COLORSHOOTING_API UEnemyManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * @brief Registers an enemy character with the subsystem.
	 * @param Enemy The enemy character to register.
	 */
	void RegisterEnemy(AEnemyCharacter* Enemy);

	/**
	 * @brief Unregisters an enemy character from the subsystem.
	 * @param Enemy The enemy character to unregister.
	 */
	void UnregisterEnemy(AEnemyCharacter* Enemy);

	/**
	 * @brief Finds the closest active enemy to a given location.
	 * @param Location The reference location to find the closest enemy from.
	 * @return The closest enemy character, or nullptr if no enemies are registered.
	 */
	AEnemyCharacter* GetClosestEnemy(const FVector& Location);

	/**
	 * @brief Starts a stage with the given data table.
	 * @param StageDataTable The data table containing the enemy spawn information for the stage.
	 */
	UFUNCTION(BlueprintCallable, Category = "EnemyManager")
	void StartStage(UDataTable* StageDataTable);

private:
	/**
	 * @brief The main loop for spawning enemies based on the stage data.
	 */
	void SpawnLoop();

	UPROPERTY(Transient)
	TArray<TObjectPtr<AEnemyCharacter>> M_ActiveEnemies;

	UPROPERTY()
    TObjectPtr<UDataTable> CurrentStageData;

    FTimerHandle SpawnLoopTimerHandle;
    
    float StageTime;
    
    int32 NextSpawnIndex;

    TArray<FEnemySpawnInfo> SpawnInfos;
};
