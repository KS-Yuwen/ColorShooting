
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Bullet/Bullet.h" // ABulletの定義をインクルード
#include "BulletPoolSubsystem.generated.h"

class UBulletPoolComponent;

UCLASS()
class COLORSHOOTING_API UBulletPoolSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Bullet Pool")
    AActor* GetBulletFromPool();

    UFUNCTION(BlueprintCallable, Category = "Bullet Pool")
    void ReturnBulletToPool(AActor* Bullet);

protected:
    UPROPERTY()
    TObjectPtr<UBulletPoolComponent> M_BulletPoolComponent;
};
