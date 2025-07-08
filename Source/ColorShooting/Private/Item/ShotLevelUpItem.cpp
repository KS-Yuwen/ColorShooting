// Copyright Epic Games, Inc. All Rights Reserved.

#include "Item/ShotLevelUpItem.h"
#include "Character/PlayerCharacter.h"

void AShotLevelUpItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
    if (PlayerCharacter == nullptr)
    {
        return;
    }

    PlayerCharacter->AddShotLevel(M_ShotType);
    OnCollected();
}
