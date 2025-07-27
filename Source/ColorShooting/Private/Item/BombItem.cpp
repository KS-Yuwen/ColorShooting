
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Item/BombItem.h"
#include "Character/PlayerCharacter.h"

void ABombItem::OnOverlapBegin(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult)
{
    Super::OnOverlapBegin(overlappedComponent, otherActor, otherComp, otherBodyIndex, bFromSweep, sweepResult);

    APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(otherActor);
    if (playerCharacter == nullptr)
    {
        return;
    }

    playerCharacter->AddBomb();
    OnCollected();
}
