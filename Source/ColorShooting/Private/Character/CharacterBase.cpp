// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/CharacterBase.h"
#include "Components/SceneComponent.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	M_Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	M_Muzzle->SetupAttachment(GetRootComponent());
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	M_Health -= ActualDamage;
	if (M_Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ACharacterBase::OnDeath()
{
	// Default death logic: destroy the actor
	Destroy();
}
