#pragma once

#include "CoreMinimal.h"
#include "Character/EnemyCharacter.h"
#include "EnemyChaser.generated.h"

/**
 * A type of enemy that chases the player.
 */
UCLASS()
class COLORSHOOTING_API AEnemyChaser : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	AEnemyChaser();

protected:
	// The speed at which the enemy chases the player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ChaseSpeed = 150.0f;

	// The speed at which the enemy rotates towards the player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationSpeed = 5.0f;

	// The range at which the enemy starts firing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRange = 1000.0f;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	// A weak pointer to the player's pawn.
	TWeakObjectPtr<APawn> PlayerPawn;

	// Flag to check if the enemy is within firing range.
	bool bCanFire = false;

	// Flag to indicate that this enemy has been permanently deactivated.
	bool bIsDeactivated = false;
};
