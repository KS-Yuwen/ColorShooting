
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BulletPoolComponent.generated.h"

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
	ABullet* GetPooledBullet(bool bIsPlayerBullet);
	void ReturnBulletToPool(ABullet* Bullet);

private:
	void CreatePool();

protected:
	UPROPERTY(EditAnywhere, Category = "Bullet Pool")
	TSubclassOf<ABullet> M_BulletClass;

	UPROPERTY(EditAnywhere, Category = "Bullet Pool")
	uint32 M_PoolSize = 20;

private:
	UPROPERTY()
	TArray<TObjectPtr<ABullet>> M_BulletPool;

	UPROPERTY()
	TArray<TObjectPtr<ABullet>> M_PooledPlayerBullets;

	UPROPERTY()
	TArray<TObjectPtr<ABullet>> M_PooledEnemyBullets;
};
