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
	UEnemyManagerSubsystem* enemyManager = GetWorld()->GetGameInstance()->GetSubsystem<UEnemyManagerSubsystem>();
	if (enemyManager == nullptr)
	{
		return;
	}
	enemyManager->RegisterEnemy(this);
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type endPlayReason)
{
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

void AEnemyCharacter::OnDeath()
{
	AColorShootingGameMode* gameMode = Cast<AColorShootingGameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode == nullptr)
	{
		Super::OnDeath();
		return;
	}

	UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
	if (constantManager == nullptr)
	{
		Super::OnDeath();
		return;
	}

	const FName scoreId = M_bKilledByReflectedBullet ? FName("Score.EnemyKillReflected") : FName("Score.EnemyKill");
	const int32 score = constantManager->GetIntValue(scoreId);
	gameMode->AddScore(score);

	Super::OnDeath();
}
