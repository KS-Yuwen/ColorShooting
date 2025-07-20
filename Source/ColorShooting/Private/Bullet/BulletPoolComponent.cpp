
#include "Bullet/BulletPoolComponent.h"
#include "Bullet/Bullet.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Subsystem/GameConstantManager.h"
#include "Kismet/GameplayStatics.h"

UBulletPoolComponent::UBulletPoolComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBulletPoolComponent::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (GameInstance)
	{
		UGameConstantManager* ConstantManager = GameInstance->GetSubsystem<UGameConstantManager>();
		if (ConstantManager)
		{
			M_PoolSize = ConstantManager->GetIntValue(TEXT("BulletPoolSize"));
		}
	}
}

void UBulletPoolComponent::CreatePool(TSubclassOf<ABullet> BulletClass)
{
	if (BulletClass == nullptr)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FBulletArray NewPool;
	NewPool.Bullets.Reserve(M_PoolSize);

	FBulletArray PlayerPool;
	PlayerPool.Bullets.Reserve(M_PoolSize / 2);

	FBulletArray EnemyPool;
	EnemyPool.Bullets.Reserve(M_PoolSize - (M_PoolSize / 2));

	for (uint32 i = 0; i < M_PoolSize; ++i)
	{
		ABullet* Bullet = World->SpawnActor<ABullet>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (Bullet)
		{
			Bullet->SetActive(false);
			NewPool.Bullets.Add(Bullet);

			if (i < M_PoolSize / 2)
			{
				Bullet->bIsPlayerBullet = true;
				PlayerPool.Bullets.Add(Bullet);
			}
			else
			{
				Bullet->bIsPlayerBullet = false;
				EnemyPool.Bullets.Add(Bullet);
			}
		}
	}

	M_BulletPools.Add(BulletClass, NewPool);
	M_PooledPlayerBullets.Add(BulletClass, PlayerPool);
	M_PooledEnemyBullets.Add(BulletClass, EnemyPool);
}

ABullet* UBulletPoolComponent::GetPooledBullet(TSubclassOf<ABullet> BulletClass, const bool bIsPlayerBullet)
{
	if (BulletClass == nullptr)
	{
		return nullptr;
	}

	if (!M_BulletPools.Contains(BulletClass))
	{
		CreatePool(BulletClass);
	}

	FBulletArray& TargetPoolArray = bIsPlayerBullet ? M_PooledPlayerBullets.FindOrAdd(BulletClass) : M_PooledEnemyBullets.FindOrAdd(BulletClass);

	for (ABullet* Bullet : TargetPoolArray.Bullets)
	{
		if (Bullet && !Bullet->IsActorTickEnabled()) // Check if the bullet is inactive
		{
			return Bullet;
		}
	}

	// If no bullets are available in the pool, create a new one.
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	ABullet* NewBullet = World->SpawnActor<ABullet>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator);
	if (NewBullet)
	{
		NewBullet->SetActive(false);
		NewBullet->bIsPlayerBullet = bIsPlayerBullet;
		TargetPoolArray.Bullets.Add(NewBullet);
		M_BulletPools.FindOrAdd(BulletClass).Bullets.Add(NewBullet);
		return NewBullet;
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
