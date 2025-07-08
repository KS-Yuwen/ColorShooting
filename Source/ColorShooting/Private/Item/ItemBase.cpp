// Copyright Epic Games, Inc. All Rights Reserved.

#include "Item/ItemBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/Pawn.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	M_SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = M_SphereComponent;
	M_SphereComponent->SetCollisionProfileName(FName("OverlapAllDynamic"));

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

void AItemBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor == nullptr || !OtherActor->IsA(APawn::StaticClass()))
	{
		return;
	}

	OnCollected();
}

void AItemBase::OnCollected()
{
	Destroy();
}
