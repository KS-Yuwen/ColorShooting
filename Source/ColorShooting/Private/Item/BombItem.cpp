
#include "Item/BombItem.h"
#include "Character/PlayerCharacter.h"

void ABombItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor))
    {
        PlayerCharacter->AddBomb();
        OnCollected();
    }
}
