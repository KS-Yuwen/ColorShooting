#include "Character/EnemyCharacter.h"
#include "Bullet/Bullet.h"
#include "ColorShootingGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/GameConstantManager.h"
#include "Subsystem/EnemyManagerSubsystem.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Register with the EnemyManagerSubsystem
	if (UEnemyManagerSubsystem* EnemyManager = GetWorld()->GetGameInstance()->GetSubsystem<UEnemyManagerSubsystem>())
	{
		EnemyManager->RegisterEnemy(this);
	}
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister from the EnemyManagerSubsystem
	if (UEnemyManagerSubsystem* EnemyManager = GetWorld()->GetGameInstance()->GetSubsystem<UEnemyManagerSubsystem>())
	{
		EnemyManager->UnregisterEnemy(this);
	}

	Super::EndPlay(EndPlayReason);
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.0f)
	{
		ABullet* Bullet = Cast<ABullet>(DamageCauser);
		if (Bullet && Bullet->bWasReflected)
		{
			bKilledByReflectedBullet = true;
		}
	}

	return ActualDamage;
}

void AEnemyCharacter::OnDeath()
{
	AColorShootingGameMode* GameMode = Cast<AColorShootingGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode != nullptr)
	{
		UGameConstantManager* ConstantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
		if (ConstantManager != nullptr)
		{
			const FName ScoreId = bKilledByReflectedBullet ? FName("Score.EnemyKillReflected") : FName("Score.EnemyKill");
			const int32 Score = ConstantManager->GetIntValue(ScoreId);
			GameMode->AddScore(Score);
		}
	}

	Super::OnDeath();
}
