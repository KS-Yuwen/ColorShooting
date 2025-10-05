#pragma once

#include "CoreMinimal.h"
#include "Character/EnemyCharacter.h"
#include "EnemySineMover.generated.h"

/**
 * 
 */
UCLASS()
class COLORSHOOTING_API AEnemySineMover : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	AEnemySineMover();

protected:
	// The amplitude of the sine wave movement.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Amplitude = 100.0f;

	// The frequency of the sine wave movement.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Frequency = 1.0f;

	// The horizontal movement speed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeedY = -100.0f;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	// The initial position of the character.
	FVector InitialPosition;

	// The time elapsed since the character started moving.
	float RunningTime;
};
