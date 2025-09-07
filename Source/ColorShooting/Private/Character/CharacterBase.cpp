// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/CharacterBase.h"
#include "Components/SceneComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	M_Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	M_Muzzle->SetupAttachment(GetRootComponent());
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	// Set health to max health at the start of the game
	M_Health = M_MaxHealth;
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACharacterBase::Fire_Implementation()
{
	if (M_MuzzleFlashEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), M_MuzzleFlashEffect, M_Muzzle->GetComponentLocation(), M_Muzzle->GetComponentRotation());
	}
	if (M_FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, M_FireSound, M_Muzzle->GetComponentLocation());
	}
}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Do not take damage if already dead
	if (M_bIsDead)
	{
		return 0.0f;
	}

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.0f)
	{
		const float OldHealth = M_Health;
		M_Health = FMath::Clamp(M_Health - ActualDamage, 0.0f, M_MaxHealth);

		// Play impact effects
		if (M_ImpactEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), M_ImpactEffect, GetActorLocation());
		}
		if (M_ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, M_ImpactSound, GetActorLocation());
		}

		// Broadcast the health change
		OnHealthChanged.Broadcast(OldHealth, M_Health);

		if (M_Health <= 0.0f)
		{
			// Call the OnDeath event. This will call the _Implementation version and can be overridden in Blueprints.
			OnDeath();
		}
	}

	return ActualDamage;
}

void ACharacterBase::OnDeath_Implementation()
{
	// Ensure death logic only runs once
	if (M_bIsDead)
	{
		return;
	}
	M_bIsDead = true;

	// Default death logic: destroy the actor
	// This can be overridden in Blueprints or child classes to add custom death effects, sounds, etc.
	Destroy();
}
