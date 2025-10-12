#include "Character/EnemyChaser.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyChaser::AEnemyChaser()
{
	PrimaryActorTick.bCanEverTick = true;

	// Automatically possess this character with an AI controller when spawned
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyChaser::BeginPlay()
{
	Super::BeginPlay();

	// Get a reference to the player pawn
	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	// Set the movement speed from the ChaseSpeed property
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = ChaseSpeed;
	}

	// Stop the default fire timer from the parent class
	GetWorldTimerManager().ClearTimer(M_FireTimerHandle);
}

void AEnemyChaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Re-acquire the player pawn if the reference is lost
	if (!PlayerPawn.IsValid())
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	FRotator TargetRotation = GetActorRotation(); // Default to current rotation

	if (PlayerPawn.IsValid())
	{
		// Player is alive: Chase and fire.
		
		// --- Rotation ---
		// Calculate target rotation towards player
		const FVector Direction = PlayerPawn->GetActorLocation() - GetActorLocation();
		TargetRotation = Direction.Rotation();

		// --- Combat ---
		const float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
		const bool bIsInRange = DistanceToPlayer <= FireRange;

		if (bIsInRange && !bCanFire)
		{
			bCanFire = true;
			GetWorldTimerManager().SetTimer(M_FireTimerHandle, this, &AEnemyCharacter::Fire, M_FireRate, true);
		}
		else if (!bIsInRange && bCanFire)
		{
			bCanFire = false;
			GetWorldTimerManager().ClearTimer(M_FireTimerHandle);
		}
	}
	else
	{
		// Player is dead or invalid: Stop firing.
		// TargetRotation is already set to the current rotation, so it will fly straight.
		if (bCanFire)
		{
			bCanFire = false;
			GetWorldTimerManager().ClearTimer(M_FireTimerHandle);
		}
	}

	// --- Apply Rotation and Movement ---
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed);
	SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f));
	AddMovementInput(GetActorForwardVector());
}
