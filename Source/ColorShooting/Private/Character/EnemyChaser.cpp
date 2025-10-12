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

	// If this enemy has been permanently deactivated, just fly forward and do nothing else.
	if (bIsDeactivated)
	{
		const FVector NewLocation = GetActorLocation() + (GetActorForwardVector() * ChaseSpeed * DeltaTime);
		SetActorLocation(NewLocation, true); // bSweep = true to attempt to stop at walls
		return;
	}

	// Try to get player pawn if we don't have a valid reference.
	if (!PlayerPawn.IsValid())
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
		// If still not valid after trying (e.g. player doesn't exist at level start), deactivate.
		if (!PlayerPawn.IsValid())
		{
			bIsDeactivated = true;
			return;
		}
	}

	ACharacterBase* PlayerCharacter = Cast<ACharacterBase>(PlayerPawn.Get());

	// If player is valid and alive (Health > 0), chase them.
	if (PlayerPawn.IsValid() && PlayerCharacter && PlayerCharacter->GetHealth() > 0)
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
			GetWorldTimerManager().SetTimer(M_FireTimerHandle, this, &AEnemyCharacter::Fire, M_FireRate, true);
		}
		else if (!bIsInRange && bCanFire)
		{
			bCanFire = false;
			GetWorldTimerManager().ClearTimer(M_FireTimerHandle);
		}
	}
	else // Player is dead or invalid.
	{
		// Detach the AI controller permanently.
		AController* CurrentController = GetController();
		if (CurrentController)
		{
			CurrentController->UnPossess();
		}
		bIsDeactivated = true;

		// Stop firing.
		if (bCanFire)
		{
			bCanFire = false;
			GetWorldTimerManager().ClearTimer(M_FireTimerHandle);
		}

		// Move forward one last time to start flying off.
		AddMovementInput(GetActorForwardVector());
	}
}
