// Copyright Epic Games, Inc. All Rights Reserved.

#include "Item/ShotLevelUpItem.h"
#include "Subsystem/SoundManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PlayerCharacter.h"
#include "TimerManager.h"
#include "Subsystem/GameConstantManager.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "UObject/ConstructorHelpers.h"

AShotLevelUpItem::AShotLevelUpItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> RedMaterialAsset(TEXT("/Game/Assets/Materials/M_Red"));
	if (RedMaterialAsset.Succeeded())
	{
		M_RedMaterial = RedMaterialAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> GreenMaterialAsset(TEXT("/Game/Assets/Materials/M_Green"));
	if (GreenMaterialAsset.Succeeded())
	{
		M_GreenMaterial = GreenMaterialAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BlueMaterialAsset(TEXT("/Game/Assets/Materials/M_Blue"));
	if (BlueMaterialAsset.Succeeded())
	{
		M_BlueMaterial = BlueMaterialAsset.Object;
	}
}

void AShotLevelUpItem::BeginPlay()
{
	Super::BeginPlay();

	ChangeShotType();

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

	USoundManagerSubsystem* soundManager = GetGameInstance()->GetSubsystem<USoundManagerSubsystem>();
	if (soundManager != nullptr)
	{
		soundManager->PlaySE(M_PickupSoundName);
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

	UMaterialInstanceConstant* newMaterial = nullptr;
	switch (M_ShotType)
	{
	case EShotType::Red:
		newMaterial = M_RedMaterial;
		break;
	case EShotType::Green:
		newMaterial = M_GreenMaterial;
		break;
	case EShotType::Blue:
		newMaterial = M_BlueMaterial;
		break;
	default:
		break;
	}

	if (newMaterial != nullptr && M_MeshComponent != nullptr)
	{
		M_MeshComponent->SetMaterial(0, newMaterial);
	}
}
