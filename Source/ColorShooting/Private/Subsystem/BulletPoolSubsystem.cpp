
#include "Subsystem/BulletPoolSubsystem.h"
#include "Bullet/BulletPoolComponent.h"

void UBulletPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    M_BulletPoolComponent = NewObject<UBulletPoolComponent>(this, TEXT("BulletPoolComponent"));
    if (M_BulletPoolComponent)
    {
        M_BulletPoolComponent->RegisterComponent();
    }
}

void UBulletPoolSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

AActor* UBulletPoolSubsystem::GetBulletFromPool(TSubclassOf<ABullet> BulletClass)
{
    if (M_BulletPoolComponent)
    {
        return M_BulletPoolComponent->GetPooledBullet(BulletClass, true);
    }
    return nullptr;
}

void UBulletPoolSubsystem::ReturnBulletToPool(AActor* Bullet)
{
    if (M_BulletPoolComponent)
    {
        if (ABullet* CastedBullet = Cast<ABullet>(Bullet))
        {
            M_BulletPoolComponent->ReturnBulletToPool(CastedBullet);
        }
    }
}
