// Copyright Epic Games, Inc. All Rights Reserved.

#include "Item/ShotLevelUpItem.h"
#include "Character/PlayerCharacter.h"
#include "TimerManager.h"
#include "Subsystem/GameConstantManager.h"

void AShotLevelUpItem::BeginPlay()
{
	Super::BeginPlay();

	UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
	if (constantManager != nullptr)
	{
		GetWorldTimerManager().SetTimer(M_TimerHandle, this, &AShotLevelUpItem::ChangeShotType, constantManager->GetFloatValue(FName("Item.ShotLevelUp.ChangeInterval")), true);
	}
	else
	{
		GetWorldTimerManager().SetTimer(M_TimerHandle, this, &AShotLevelUpItem::ChangeShotType, 3.0f, true); // Fallback value
	}
}

void AShotLevelUpItem::OnOverlapBegin(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult)
{
    Super::OnOverlapBegin(overlappedComponent, otherActor, otherComp, otherBodyIndex, bFromSweep, sweepResult);

    APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(otherActor);
    if (playerCharacter == nullptr)
    {
        return;
    }

    playerCharacter->AddShotLevel(M_ShotType);
    OnCollected();
}

void AShotLevelUpItem::ChangeShotType()
{
	UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
	int32 minShotType = 0;
	int32 maxShotType = static_cast<int32>(EShotType::Max) - 1;

	if (constantManager != nullptr)
	{
		minShotType = constantManager->GetIntValue(FName("Item.ShotLevelUp.MinShotType"));
		maxShotType = constantManager->GetIntValue(FName("Item.ShotLevelUp.MaxShotType"));
	}

	M_ShotType = static_cast<EShotType>(FMath::RandRange(minShotType, maxShotType));
}
