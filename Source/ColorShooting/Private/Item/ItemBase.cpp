// Copyright Epic Games, Inc. All Rights Reserved.

#include "Item/ItemBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Character/PlayerCharacter.h"

AItemBase::AItemBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	M_SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = M_SphereComponent;
	
	// Set up collision to only overlap with Pawns
	M_SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	M_SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	M_SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	M_MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	M_MeshComponent->SetupAttachment(RootComponent);
	M_MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	M_RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (M_SphereComponent)
	{
		M_SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnOverlapBegin);
	}
}

void AItemBase::OnOverlapBegin(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult)
{
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(otherActor);
	if (playerCharacter == nullptr)
	{
		return;
	}

	OnCollected();
}

void AItemBase::OnCollected()
{
	Destroy();
}
