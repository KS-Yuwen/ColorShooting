#include "Character/EnemyChaser.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyChaser::AEnemyChaser()
{
	PrimaryActorTick.bCanEverTick = true;

	// Automatically possess this character with an AI controller when spawned
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Set up flying movement for the character
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(EMovementMode::MOVE_Flying);
		MoveComp->GravityScale = 0.0f;
	}
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

	// Re-acquire the player pawn if the reference is lost (e.g., after player respawns)
	if (!PlayerPawn.IsValid())
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
		if (!PlayerPawn.IsValid())
		{
			return; // Stop processing if player is not available
		}
	}

	// --- Rotation ---
	const FVector Direction = PlayerPawn->GetActorLocation() - GetActorLocation();
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), Direction.Rotation(), DeltaTime, RotationSpeed);
	SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f)); // We only want to rotate in the Yaw

	// --- Movement ---
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->Velocity = GetActorForwardVector() * ChaseSpeed;
	}

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
