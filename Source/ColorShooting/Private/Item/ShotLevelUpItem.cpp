// Copyright Epic Games, Inc. All Rights Reserved.

#include "Item/ShotLevelUpItem.h"
#include "Character/PlayerCharacter.h"
#include "TimerManager.h"

void AShotLevelUpItem::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(M_TimerHandle, this, &AShotLevelUpItem::ChangeShotType, 3.0f, true);
}

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

void AShotLevelUpItem::ChangeShotType()
{
	M_ShotType = static_cast<EShotType>(FMath::RandRange(0, static_cast<int32>(EShotType::Max) - 1));
}
