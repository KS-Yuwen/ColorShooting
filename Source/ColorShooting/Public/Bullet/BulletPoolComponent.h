
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

private:
	void CreatePool();

protected:
	UPROPERTY(EditAnywhere, Category = "Bullet Pool")
	TSubclassOf<ABullet> BulletClass;

	UPROPERTY(EditAnywhere, Category = "Bullet Pool")
	int PoolSize = 20;

private:
	UPROPERTY()
	TArray<TObjectPtr<ABullet>> BulletPool;

	UPROPERTY()
	TArray<TObjectPtr<ABullet>> PooledPlayerBullets;

	UPROPERTY()
	TArray<TObjectPtr<ABullet>> PooledEnemyBullets;
};
