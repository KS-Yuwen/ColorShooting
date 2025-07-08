
#include "Bullet/BulletPoolComponent.h"
#include "Bullet/Bullet.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

UBulletPoolComponent::UBulletPoolComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Set default bullet class (can be overridden in Blueprints)
	static ConstructorHelpers::FClassFinder<ABullet> BulletClassFinder(TEXT("/Game/BluePrints/Bullet/BP_Bullet"));
	if (BulletClassFinder.Succeeded())
	{
		M_BulletClass = BulletClassFinder.Class;
	}

	M_PoolSize = 50; // Default pool size
}

void UBulletPoolComponent::BeginPlay()
{
	Super::BeginPlay();
	CreatePool();
}

void UBulletPoolComponent::CreatePool()
{
	if (M_BulletClass == nullptr)
	{
		return;
	}

	for (uint32 i = 0; i < M_PoolSize; ++i)
	{
		ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(M_BulletClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (Bullet)
		{
			Bullet->SetActive(false);
			M_BulletPool.Add(Bullet);

			// Assign half to player and half to enemy
			if (i < M_PoolSize / 2)
			{
				Bullet->bIsPlayerBullet = true;
				M_PooledPlayerBullets.Add(Bullet);
			}
			else
			{
				Bullet->bIsPlayerBullet = false;
				M_PooledEnemyBullets.Add(Bullet);
			}
		}
	}
}

ABullet* UBulletPoolComponent::GetPooledBullet(const bool bIsPlayerBullet)
{
	auto& TargetPool = bIsPlayerBullet ? M_PooledPlayerBullets : M_PooledEnemyBullets;

	for (ABullet* Bullet : TargetPool)
	{
		if (Bullet && Bullet->IsActorTickEnabled()) // Check if the bullet is active
		{
			continue;
		}
		return Bullet;
	}

	// If no bullets are available in the pool, create a new one.
	if (M_BulletClass)
	{
		ABullet* NewBullet = GetWorld()->SpawnActor<ABullet>(M_BulletClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (NewBullet)
		{
			NewBullet->SetActive(false);
			NewBullet->bIsPlayerBullet = bIsPlayerBullet;
			TargetPool.Add(NewBullet);
			M_BulletPool.Add(NewBullet); // Add to the main pool as well
			return NewBullet;
		}
	}

	return nullptr;
}

void UBulletPoolComponent::ReturnBulletToPool(ABullet* Bullet)
{
	if (Bullet)
	{
		Bullet->SetActive(false);
	}
}
