
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

	FBulletArray NewPool;
	for (uint32 i = 0; i < M_PoolSize; ++i)
	{
		ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (Bullet)
		{
			Bullet->SetActive(false);
			NewPool.Bullets.Add(Bullet);
		}
	}
	M_BulletPools.Add(BulletClass, NewPool);
	M_PooledPlayerBullets.Add(BulletClass, FBulletArray());
	M_PooledEnemyBullets.Add(BulletClass, FBulletArray());

	for (uint32 i = 0; i < M_PoolSize; ++i)
	{
		ABullet* Bullet = NewPool.Bullets[i];
		if (i < M_PoolSize / 2)
		{
			Bullet->bIsPlayerBullet = true;
			M_PooledPlayerBullets[BulletClass].Bullets.Add(Bullet);
		}
		else
		{
			Bullet->bIsPlayerBullet = false;
			M_PooledEnemyBullets[BulletClass].Bullets.Add(Bullet);
		}
	}
}

ABullet* UBulletPoolComponent::GetPooledBullet(TSubclassOf<ABullet> BulletClass, const bool bIsPlayerBullet)
{
	if (!M_BulletPools.Contains(BulletClass))
	{
		CreatePool(BulletClass);
	}

	auto& TargetPool = bIsPlayerBullet ? M_PooledPlayerBullets[BulletClass].Bullets : M_PooledEnemyBullets[BulletClass].Bullets;

	for (ABullet* Bullet : TargetPool)
	{
		if (Bullet && !Bullet->IsActorTickEnabled()) // Check if the bullet is inactive
		{
			return Bullet;
		}
	}

	// If no bullets are available in the pool, create a new one.
	ABullet* NewBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator);
	if (NewBullet)
	{
		NewBullet->SetActive(false);
		NewBullet->bIsPlayerBullet = bIsPlayerBullet;
		TargetPool.Add(NewBullet);
		M_BulletPools[BulletClass].Bullets.Add(NewBullet);
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
