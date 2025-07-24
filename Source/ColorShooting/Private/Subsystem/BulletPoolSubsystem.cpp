
#include "Subsystem/BulletPoolSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/GameConstantManager.h"

void UBulletPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UGameConstantManager* ConstantManager = GameInstance->GetSubsystem<UGameConstantManager>();
		if (ConstantManager)
		{
			M_PoolSize = ConstantManager->GetIntValue(TEXT("BulletPoolSize"));
		}
	}
}

void UBulletPoolSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UBulletPoolSubsystem::CreatePool(TSubclassOf<ABullet> BulletClass)
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

	FBulletPool& Pool = M_BulletPools.FindOrAdd(BulletClass);
	Pool.AllBullets.Reserve(M_PoolSize);
	Pool.AvailableBullets.Reserve(M_PoolSize);

	for (uint32 i = 0; i < M_PoolSize; ++i)
	{
		ABullet* Bullet = World->SpawnActor<ABullet>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (Bullet)
		{
			Bullet->SetActive(false);
			Pool.AllBullets.Add(Bullet);
			Pool.AvailableBullets.Add(Bullet);
		}
	}
}

ABullet* UBulletPoolSubsystem::GetBulletFromPool(TSubclassOf<ABullet> BulletClass, const bool bIsPlayerBullet)
{
	if (BulletClass == nullptr)
	{
		return nullptr;
	}

	if (!M_BulletPools.Contains(BulletClass))
	{
		CreatePool(BulletClass);
	}

	FBulletPool& Pool = M_BulletPools.FindOrAdd(BulletClass);
	if (Pool.AvailableBullets.Num() > 0)
	{
		ABullet* Bullet = Pool.AvailableBullets.Pop();
		if (Bullet)
		{
			Bullet->bIsPlayerBullet = bIsPlayerBullet;
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
		NewBullet->bIsPlayerBullet = bIsPlayerBullet;
		Pool.AllBullets.Add(NewBullet);
		return NewBullet;
	}

	return nullptr;
}

void UBulletPoolSubsystem::ReturnBulletToPool(ABullet* Bullet)
{
	if (Bullet == nullptr)
	{
		return;
	}

	Bullet->SetActive(false);

	TSubclassOf<ABullet> BulletClass = Bullet->GetClass();
	if (FBulletPool* Pool = M_BulletPools.Find(BulletClass))
	{
		Pool->AvailableBullets.Add(Bullet);
	}
}
