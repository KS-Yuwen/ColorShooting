
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Bullet/Bullet.h" // ABulletの定義をインクルード
#include "BulletPoolSubsystem.generated.h"

USTRUCT()
struct FBulletPool
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<TObjectPtr<ABullet>> AllBullets;

    UPROPERTY()
    TArray<TObjectPtr<ABullet>> AvailableBullets;
};

UCLASS()
class COLORSHOOTING_API UBulletPoolSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Bullet Pool")
    ABullet* GetBulletFromPool(TSubclassOf<ABullet> BulletClass, bool bIsPlayerBullet);

    UFUNCTION(BlueprintCallable, Category = "Bullet Pool")
    void ReturnBulletToPool(ABullet* Bullet);

private:
	void CreatePool(TSubclassOf<ABullet> BulletClass);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Bullet Pool")
	uint32 M_PoolSize = 20;

private:
	UPROPERTY()
	TMap<TSubclassOf<ABullet>, FBulletPool> M_BulletPools;
};
