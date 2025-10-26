#pragma once

#include "CoreMinimal.h"
#include "Character/EnemyCharacter.h"
#include "EnemyTurret.generated.h"

/**
 * An enemy type that moves to a specified location, stops, and then fires projectiles in multiple directions.
 */
UCLASS()
class COLORSHOOTING_API AEnemyTurret : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	AEnemyTurret();

protected:
	// Overridden from AActor.
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** The target location where the enemy will stop moving. This can be set per-instance in the editor. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Movement", meta = (MakeEditWidget = "true"))
	FVector TargetLocation;

	/** The speed at which the enemy moves towards its target location. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed = 200.0f;

	/** The number of projectiles to fire in a single burst. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 BulletsPerBurst = 8;

	/** The time, in seconds, between each burst of fire. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float TimeBetweenBursts = 3.0f;

private:
	/** A flag to indicate whether the enemy has reached its destination. */
	bool bHasReachedTarget = false;

	/** Timer handle for managing the burst firing rate. */
	FTimerHandle BurstFireTimerHandle;

	/** Fires a burst of projectiles spread out in a circle. */
	void FireBurst();
};
