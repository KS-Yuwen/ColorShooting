#include "Character/EnemyCharacter.h"
#include "Bullet/Bullet.h"
#include "ColorShootingGameMode.h"
#include "Kismet/GameplayStatics.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
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
		const int32 Score = bKilledByReflectedBullet ? 500 : 100;
		GameMode->AddScore(Score);
	}

	Super::OnDeath();
}
