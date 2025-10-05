#include "Character/EnemyCharacter.h"
#include "Bullet/Bullet.h"
#include "ColorShootingGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/GameConstantManager.h"
#include "Subsystem/EnemyManagerSubsystem.h"
#include "TimerManager.h"
#include "Item/ItemBase.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set up flying movement as a default for all enemy characters
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(EMovementMode::MOVE_Flying);
		MoveComp->GravityScale = 0.0f;
	}
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Register with the EnemyManagerSubsystem
	UEnemyManagerSubsystem* enemyManager = GetWorld()->GetGameInstance()->GetSubsystem<UEnemyManagerSubsystem>();
	if (enemyManager == nullptr)
	{
		return;
	}
	enemyManager->RegisterEnemy(this);

	// Start firing timer with a random initial delay
	const float initialDelay = FMath::RandRange(0.5f, 2.0f);
	GetWorldTimerManager().SetTimer(M_FireTimerHandle, this, &AEnemyCharacter::Fire, M_FireRate, true, initialDelay);
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	// Clear timer
	GetWorldTimerManager().ClearTimer(M_FireTimerHandle);

	// Unregister from the EnemyManagerSubsystem
	UEnemyManagerSubsystem* enemyManager = GetWorld()->GetGameInstance()->GetSubsystem<UEnemyManagerSubsystem>();
	if (enemyManager == nullptr)
	{
		Super::EndPlay(endPlayReason);
		return;
	}
	enemyManager->UnregisterEnemy(this);

	Super::EndPlay(endPlayReason);
}

void AEnemyCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

void AEnemyCharacter::Fire()
{
	if (M_ProjectileClass == nullptr)
	{
		return;
	}

	UWorld* const world = GetWorld();
	if (world == nullptr)
	{
		return;
	}

	APawn* playerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (playerPawn == nullptr)
	{
		return;
	}

	const FVector fireLocation = GetActorLocation();
	const FRotator fireRotation = (playerPawn->GetActorLocation() - fireLocation).Rotation();

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.Instigator = GetInstigator();
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABullet* bullet = world->SpawnActor<ABullet>(M_ProjectileClass, fireLocation, fireRotation, spawnParams);
	if (bullet)
	{
		bullet->M_bIsPlayerBullet = false;
		bullet->M_ShotType = M_ColorType;
	}
}

float AEnemyCharacter::TakeDamage(float damageAmount, FDamageEvent const& damageEvent, AController* eventInstigator, AActor* damageCauser)
{
	const float actualDamage = Super::TakeDamage(damageAmount, damageEvent, eventInstigator, damageCauser);

	if (actualDamage <= 0.0f)
	{
		return actualDamage;
	}

	ABullet* bullet = Cast<ABullet>(damageCauser);
	if (bullet && bullet->M_bWasReflected)
	{
		M_bKilledByReflectedBullet = true;
	}

	return actualDamage;
}

void AEnemyCharacter::OnDeath_Implementation()
{
	// Ensure death logic only runs once to prevent re-entry and freezes.
	if (M_bIsDead)
	{
		return;
	}
	M_bIsDead = true;

	AColorShootingGameMode* gameMode = Cast<AColorShootingGameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode != nullptr)
	{
		UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
		if (constantManager != nullptr)
		{
			const FName scoreId = M_bKilledByReflectedBullet ? FName("Score.EnemyKillReflected") : FName("Score.EnemyKill");
			const int32 score = constantManager->GetIntValue(scoreId);
			gameMode->AddScore(score);
		}
	}

	// Drop items
	UWorld* const world = GetWorld();
	if (world)
	{
		for (const FDropItemInfo& dropInfo : M_DropItems)
		{
			if (dropInfo.ItemClass && FMath::FRand() < dropInfo.DropChance)
			{
				FActorSpawnParameters spawnParams;
				spawnParams.Owner = this;
				spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				world->SpawnActor<AItemBase>(dropInfo.ItemClass, GetActorLocation(), GetActorRotation(), spawnParams);
			}
		}
	}

	// Finally, destroy the actor.
	Destroy();
}
