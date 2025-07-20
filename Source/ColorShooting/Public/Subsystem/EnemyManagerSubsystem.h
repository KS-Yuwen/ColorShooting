// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EnemyManagerSubsystem.generated.h"

class AEnemyCharacter;

/**
 * 
 */
UCLASS()
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
	AEnemyCharacter* GetClosestEnemy(const FVector& Location) const;

private:
	UPROPERTY()
	TArray<TObjectPtr<AEnemyCharacter>> M_ActiveEnemies;
};
