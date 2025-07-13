// Copyright Epic Games, Inc. All Rights Reserved.

#include "Bullet/GreenBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/EnemyCharacter.h"

AGreenBullet::AGreenBullet()
{
	// Setup homing properties
	if (M_ProjectileMovementComponent)
	{
		M_ProjectileMovementComponent->bIsHomingProjectile = true;
		M_ProjectileMovementComponent->HomingAccelerationMagnitude = 2000.0f;
	}

	// Re-bind the OnHit event to our specific function
	if (M_CollisionComponent)
	{
		M_CollisionComponent->OnComponentHit.RemoveDynamic(this, &ABullet::OnHit);
		M_CollisionComponent->OnComponentHit.AddDynamic(this, &AGreenBullet::OnGreenBulletHit);
	}

	M_bHasHitTarget = false;
}

void AGreenBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Stop homing if the target has been hit
	if (M_bHasHitTarget)
	{
		if (M_ProjectileMovementComponent)
		{
			M_ProjectileMovementComponent->bIsHomingProjectile = false;
		}
	}
	else if (M_TargetEnemy.IsValid())
	{
		// Continue homing towards the target
		if (M_ProjectileMovementComponent)
		{
			M_ProjectileMovementComponent->HomingTargetComponent = M_TargetEnemy->GetRootComponent();
		}
	}

	CheckIfOffScreen();
}

void AGreenBullet::SetTarget(AActor* NewTarget)
{
	M_TargetEnemy = NewTarget;
}

void AGreenBullet::OnGreenBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == nullptr || OtherActor == this)
	{
		return;
	}

	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OtherActor);
	if (bIsPlayerBullet && EnemyCharacter != nullptr)
	{
		if (EnemyCharacter->GetColorType() == M_ShotType)
		{
			// Reflect the bullet with a random angle
			const FVector ReflectionVector = FMath::GetReflectionVector(GetVelocity(), Hit.ImpactNormal);
			const FVector RandomizedReflectionVector = ReflectionVector + FMath::VRand() * 500.0f;
			M_ProjectileMovementComponent->Velocity = RandomizedReflectionVector.GetSafeNormal() * M_ProjectileMovementComponent->InitialSpeed;

			// Stop homing and prevent hitting the player
			M_ProjectileMovementComponent->bIsHomingProjectile = false;
			bIsPlayerBullet = false;
			return; // Don't destroy the bullet
		}
	}

	// If it hits an enemy, mark as hit but don't destroy (it penetrates)
	if (Cast<AEnemyCharacter>(OtherActor))
	{
		M_bHasHitTarget = true;
	}
	else
	{
		// If it hits anything else (like a wall), destroy it
		Destroy();
	}
}

void AGreenBullet::CheckIfOffScreen()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController == nullptr)
	{
		return;
	}

	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	FVector2D ScreenLocation;
	if (PlayerController->ProjectWorldLocationToScreen(GetActorLocation(), ScreenLocation))
	{
		// Destroy if off-screen
		const bool bIsOffScreen = ScreenLocation.X < 0 || ScreenLocation.X > ViewportSizeX || ScreenLocation.Y < 0 || ScreenLocation.Y > ViewportSizeY;
		if (bIsOffScreen)
		{
			Destroy();
		}
	}
}
