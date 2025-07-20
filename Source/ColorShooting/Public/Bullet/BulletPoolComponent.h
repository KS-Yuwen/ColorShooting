
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Bullet/Bullet.h"
#include "BulletPoolComponent.generated.h"

USTRUCT()
struct FBulletArray
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<TObjectPtr<ABullet>> Bullets;
};

class ABullet;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COLORSHOOTING_API UBulletPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBulletPoolComponent();

protected:
	virtual void BeginPlay() override;

public:	
	ABullet* GetPooledBullet(TSubclassOf<ABullet> BulletClass, bool bIsPlayerBullet);
	void ReturnBulletToPool(ABullet* Bullet);

private:
	void CreatePool(TSubclassOf<ABullet> BulletClass);

protected:
	UPROPERTY(EditAnywhere, Category = "Bullet Pool")
	TSubclassOf<ABullet> M_BulletClass;

	UPROPERTY(VisibleAnywhere, Category = "Bullet Pool")
	uint32 M_PoolSize = 20;

private:
	UPROPERTY()
	TMap<TSubclassOf<ABullet>, FBulletArray> M_BulletPools;

	UPROPERTY()
	TMap<TSubclassOf<ABullet>, FBulletArray> M_PooledPlayerBullets;

	UPROPERTY()
	TMap<TSubclassOf<ABullet>, FBulletArray> M_PooledEnemyBullets;
};
