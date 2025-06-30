#include "Item/ShotLevelUpItem.h"
#include "Character/PlayerCharacter.h"

void AShotLevelUpItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor))
    {
        PlayerCharacter->AddShotLevel(ShotType);
        OnCollected();
    }
}
