#include "Character/EnemyTurret.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

AEnemyTurret::AEnemyTurret()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyTurret::BeginPlay()
{
	Super::BeginPlay();

	// This enemy uses a custom timer for burst firing, so stop the parent's default fire timer.
	GetWorldTimerManager().ClearTimer(M_FireTimerHandle);

	// Set the movement speed.
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = MovementSpeed; // For walking
		MoveComp->MaxFlySpeed = MovementSpeed;  // For flying
	}
}

void AEnemyTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Only handle movement logic if the target has not yet been reached.
	if (!bHasReachedTarget)
	{
		const FVector CurrentLocation = GetActorLocation();
		// Use a 2D distance check to ignore height differences.
		if (FVector::Dist2D(CurrentLocation, TargetLocation) < 10.0f)
		{
			// --- Target Reached ---
			bHasReachedTarget = true;

			// Stop all movement immediately.
			if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
			{
				MoveComp->StopMovementImmediately();
			}

			// Start the timer to fire bursts.
			GetWorldTimerManager().SetTimer(BurstFireTimerHandle, this, &AEnemyTurret::FireBurst, TimeBetweenBursts, true, 0.0f);
		}
		else
		{
			// --- Move Towards Target ---
			const FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
			AddMovementInput(Direction);
		}
	}
}

void AEnemyTurret::FireBurst()
{
	if (BulletsPerBurst <= 0)
	{
		return;
	}

	const float AngleStep = 360.0f / BulletsPerBurst;
	const float StartAngle = GetActorRotation().Yaw;

	for (int32 i = 0; i < BulletsPerBurst; ++i)
	{
		const float FiringAngle = StartAngle + (i * AngleStep);
		const FRotator FireRotation(0.0f, FiringAngle, 0.0f);
		FireInDirection(FireRotation);
	}
}
