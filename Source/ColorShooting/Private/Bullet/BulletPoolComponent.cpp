
#include "Bullet/BulletPoolComponent.h"
#include "Bullet/Bullet.h"

UBulletPoolComponent::UBulletPoolComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBulletPoolComponent::BeginPlay()
{
	Super::BeginPlay();
	CreatePool();
}

void UBulletPoolComponent::CreatePool()
{
	if (BulletClass == nullptr) return;

	for (int32 i = 0; i < PoolSize; ++i)
	{
		ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (Bullet)
		{
			Bullet->SetActive(false);
			BulletPool.Add(Bullet);

			// Assign half to player and half to enemy
			if (i < PoolSize / 2)
			{
				Bullet->bIsPlayerBullet = true;
				PooledPlayerBullets.Add(Bullet);
			}
			else
			{
				Bullet->bIsPlayerBullet = false;
				PooledEnemyBullets.Add(Bullet);
			}
		}
	}
}

ABullet* UBulletPoolComponent::GetPooledBullet(bool bIsPlayerBullet)
{
	TArray<TObjectPtr<ABullet>>& TargetPool = bIsPlayerBullet ? PooledPlayerBullets : PooledEnemyBullets;

	for (ABullet* Bullet : TargetPool)
	{
		if (Bullet && Bullet->IsActorHidden())
		{
			return Bullet;
		}
	}

	// プールに利用可能な弾がない場合、新しく生成するか、nullを返すか選択できます。
    // ここでは、新しい弾を生成してプールに追加します。
    if (BulletClass)
    {
        ABullet* NewBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator);
        if (NewBullet)
        {
            NewBullet->SetActive(false);
            NewBullet->bIsPlayerBullet = bIsPlayerBullet;
            TargetPool.Add(NewBullet);
			BulletPool.Add(NewBullet); // Add to the main pool as well
            return NewBullet;
        }
    }

	return nullptr;
}
