
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
	UE_LOG(LogTemp, Log, TEXT("UBulletPoolSubsystem::CreatePool() called for BulletClass: %s"), *GetNameSafe(BulletClass));

	if (BulletClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBulletPoolSubsystem::CreatePool: BulletClass is nullptr."));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBulletPoolSubsystem::CreatePool: GetWorld() returned nullptr."));
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
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UBulletPoolSubsystem::CreatePool: Failed to spawn bullet for class %s at index %d."), *GetNameSafe(BulletClass), i);
		}
	}

	M_BulletPools.Add(BulletClass, NewPool);
	M_PooledPlayerBullets.Add(BulletClass, PlayerPool);
	M_PooledEnemyBullets.Add(BulletClass, EnemyPool);

	UE_LOG(LogTemp, Log, TEXT("UBulletPoolSubsystem::CreatePool: Pool created for %s with %d bullets."), *GetNameSafe(BulletClass), NewPool.Bullets.Num());
}

ABullet* UBulletPoolSubsystem::GetBulletFromPool(TSubclassOf<ABullet> BulletClass, const bool bIsPlayerBullet)
{
	UE_LOG(LogTemp, Log, TEXT("UBulletPoolSubsystem::GetBulletFromPool() called for BulletClass: %s, bIsPlayerBullet: %s"), *GetNameSafe(BulletClass), bIsPlayerBullet ? TEXT("true") : TEXT("false"));

	if (BulletClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBulletPoolSubsystem::GetBulletFromPool: BulletClass is nullptr."));
		return nullptr;
	}

	if (!M_BulletPools.Contains(BulletClass))
	{
		UE_LOG(LogTemp, Log, TEXT("UBulletPoolSubsystem::GetBulletFromPool: Pool not found for %s, creating new pool."), *GetNameSafe(BulletClass));
		CreatePool(BulletClass);
	}

	FBulletArray& TargetPoolArray = bIsPlayerBullet ? M_PooledPlayerBullets.FindOrAdd(BulletClass) : M_PooledEnemyBullets.FindOrAdd(BulletClass);

	for (ABullet* Bullet : TargetPoolArray.Bullets)
	{
		if (Bullet && !Bullet->IsActorTickEnabled()) // Check if the bullet is inactive
		{
			UE_LOG(LogTemp, Log, TEXT("UBulletPoolSubsystem::GetBulletFromPool: Reusing inactive bullet."));
			return Bullet;
		}
	}

	// If no bullets are available in the pool, create a new one.
	UE_LOG(LogTemp, Log, TEXT("UBulletPoolSubsystem::GetBulletFromPool: No inactive bullets found, spawning new one."));
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBulletPoolSubsystem::GetBulletFromPool: GetWorld() returned nullptr when spawning new bullet."));
		return nullptr;
	}

	ABullet* NewBullet = World->SpawnActor<ABullet>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator);
	if (NewBullet)
	{
		NewBullet->SetActive(false);
		NewBullet->bIsPlayerBullet = bIsPlayerBullet;
		TargetPoolArray.Bullets.Add(NewBullet);
		M_BulletPools.FindOrAdd(BulletClass).Bullets.Add(NewBullet);
		UE_LOG(LogTemp, Log, TEXT("UBulletPoolSubsystem::GetBulletFromPool: Successfully spawned new bullet."));
		return NewBullet;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBulletPoolSubsystem::GetBulletFromPool: Failed to spawn new bullet for class %s."), *GetNameSafe(BulletClass));
	}

	return nullptr;
}

void UBulletPoolSubsystem::ReturnBulletToPool(ABullet* Bullet)
{
	if (Bullet)
	{
		Bullet->SetActive(false);
	}
}
