#include "Character/EnemyChaser.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

AEnemyChaser::AEnemyChaser()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyChaser::BeginPlay()
{
	Super::BeginPlay();

	// Get a reference to the player pawn
	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	// Stop the default fire timer from the parent class
	GetWorldTimerManager().ClearTimer(M_FireTimerHandle);
}

void AEnemyChaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerPawn.IsValid())
	{
		return;
	}

	// --- Rotation ---
	const FVector Direction = PlayerPawn->GetActorLocation() - GetActorLocation();
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), Direction.Rotation(), DeltaTime, RotationSpeed);
	SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f)); // We only want to rotate in the Yaw

	// --- Movement ---
	AddMovementInput(GetActorForwardVector());

	// --- Combat ---
	const float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());

	// Check if the enemy should start or stop firing
	const bool bIsInRange = DistanceToPlayer <= FireRange;
	if (bIsInRange && !bCanFire)
	{
		bCanFire = true;
		// Start the fire timer
		GetWorldTimerManager().SetTimer(M_FireTimerHandle, this, &AEnemyCharacter::Fire, M_FireRate, true);
	}
	else if (!bIsInRange && bCanFire)
	{
		bCanFire = false;
		// Stop the fire timer
		GetWorldTimerManager().ClearTimer(M_FireTimerHandle);
	}
}
