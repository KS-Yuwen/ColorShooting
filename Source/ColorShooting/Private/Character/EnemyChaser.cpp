#include "Character/EnemyChaser.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Character/CharacterBase.h"

AEnemyChaser::AEnemyChaser()
{
	PrimaryActorTick.bCanEverTick = true;

	// Disable all forms of automatic rotation to take full manual control in Tick.
	bUseControllerRotationYaw = false;
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = false;
	}

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

	// If deactivated, just keep moving forward and do nothing else.
	if (bIsDeactivated)
	{
		AddMovementInput(GetActorForwardVector());
		return;
	}

	// Attempt to get the player pawn if the reference is weak or invalid.
	if (!PlayerPawn.IsValid())
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	ACharacterBase* PlayerCharacter = PlayerPawn.IsValid() ? Cast<ACharacterBase>(PlayerPawn.Get()) : nullptr;

	// If the player is valid and alive, chase them.
	if (PlayerCharacter && PlayerCharacter->GetHealth() > 0)
	{
		// --- Rotation ---
		const FVector Direction = PlayerPawn->GetActorLocation() - GetActorLocation();
		const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), Direction.Rotation(), DeltaTime, RotationSpeed);
		SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f));

		// --- Movement ---
		AddMovementInput(GetActorForwardVector());

		// --- Combat ---
		const float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
		const bool bIsInRange = DistanceToPlayer <= FireRange;
		if (bIsInRange && !bCanFire)
		{
			bCanFire = true;
			GetWorldTimerManager().SetTimer(M_FireTimerHandle, this, &AEnemyCharacter::Fire, M_FireRate, true, 0.0f);
		}
		else if (!bIsInRange && bCanFire)
		{
			bCanFire = false;
			GetWorldTimerManager().ClearTimer(M_FireTimerHandle);
		}
	}
	else // Player is dead or invalid, so deactivate.
	{
		bIsDeactivated = true;

		// Stop firing if we were.
		if (bCanFire)
		{
			bCanFire = false;
			GetWorldTimerManager().ClearTimer(M_FireTimerHandle);
		}

		// Stop any AI-driven movement.
		AController* CurrentController = GetController();
		if (CurrentController)
		{
			CurrentController->StopMovement();
		}
	}
}
