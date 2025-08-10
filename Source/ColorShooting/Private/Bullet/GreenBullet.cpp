// Copyright Epic Games, Inc. All Rights Reserved.

#include "Bullet/GreenBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/EnemyCharacter.h"
#include "Subsystem/GameConstantManager.h"
#include "Subsystem/BulletPoolSubsystem.h"

AGreenBullet::AGreenBullet()
{
	// Setup homing properties
	if (M_ProjectileMovementComponent)
	{
		M_ProjectileMovementComponent->bIsHomingProjectile = true;
		M_ProjectileMovementComponent->HomingAccelerationMagnitude = 2000.0f; // Fallback value
	}

	// Re-bind the OnHit event to our specific function
	if (M_CollisionComponent)
	{
		M_CollisionComponent->OnComponentHit.RemoveDynamic(this, &ABullet::OnHit);
		M_CollisionComponent->OnComponentHit.AddDynamic(this, &AGreenBullet::OnGreenBulletHit);
	}

	M_bHasHitTarget = false;
}

void AGreenBullet::BeginPlay()
{
	Super::BeginPlay();

	UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
	if (constantManager != nullptr)
	{
		M_ProjectileMovementComponent->HomingAccelerationMagnitude = constantManager->GetFloatValue(FName("Bullet.GreenBullet.HomingAccelerationMagnitude"));
	}
	else
	{
		M_ProjectileMovementComponent->HomingAccelerationMagnitude = 2000.0f; // Fallback value
	}
}

void AGreenBullet::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

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

void AGreenBullet::SetTarget(AActor* newTarget)
{
	M_TargetEnemy = newTarget;
}

void AGreenBullet::SetPierceProperties(int32 MaxPierces)
{
	M_MaxPierces = MaxPierces;
}

void AGreenBullet::SetActive(bool bIsActive)
{
	Super::SetActive(bIsActive);

	if (bIsActive)
	{
		M_CurrentPierces = 0;
		M_bHasHitTarget = false;
		M_HitEnemies.Empty();
		if (M_ProjectileMovementComponent)
		{
			M_ProjectileMovementComponent->bIsHomingProjectile = true;
		}
	}
}

void AGreenBullet::OnGreenBulletHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector normalImpulse, const FHitResult& hit)
{
	if (otherActor == nullptr || otherActor == this)
	{
		return;
	}

	UBulletPoolSubsystem* bulletPool = GetGameInstance()->GetSubsystem<UBulletPoolSubsystem>();
	if (bulletPool == nullptr)
	{
		return;
	}

	AEnemyCharacter* enemyCharacter = Cast<AEnemyCharacter>(otherActor);
	if (enemyCharacter != nullptr)
	{
		// If the bullet has already hit this enemy, ignore it.
		if (M_HitEnemies.Contains(enemyCharacter))
		{
			return;
		}

		// Handle same-color reflection
		if (M_bIsPlayerBullet && enemyCharacter->GetColorType() == M_ShotType)
		{
			UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
			float reflectionRandomness = 500.0f; // Default value
			if (constantManager != nullptr)
			{
				reflectionRandomness = constantManager->GetFloatValue(FName("Bullet.ReflectionRandomness"));
			}

			const FVector reflectionVector = FMath::GetReflectionVector(GetVelocity(), hit.ImpactNormal);
			const FVector randomizedReflectionVector = reflectionVector + FMath::VRand() * reflectionRandomness;
			M_ProjectileMovementComponent->Velocity = randomizedReflectionVector.GetSafeNormal() * M_ProjectileMovementComponent->InitialSpeed;

			M_ProjectileMovementComponent->bIsHomingProjectile = false;
			M_bWasReflected = true;
			return; 
		}

		// Apply damage
		UGameplayStatics::ApplyDamage(enemyCharacter, Damage, GetInstigatorController(), this, UDamageType::StaticClass());

		M_HitEnemies.Add(enemyCharacter);
		M_CurrentPierces++;
		M_bHasHitTarget = true; // Stop homing after first hit

		// Check if the bullet should be destroyed
		if (M_CurrentPierces >= M_MaxPierces)
		{
			bulletPool->ReturnBulletToPool(this);
		}
	}
	else
	{
		// If it hits anything other than an enemy (like a wall), return it to the pool
		bulletPool->ReturnBulletToPool(this);
	}
}

void AGreenBullet::CheckIfOffScreen()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	if (playerController == nullptr)
	{
		return;
	}

	int32 viewportSizeX, viewportSizeY;
	playerController->GetViewportSize(viewportSizeX, viewportSizeY);

	FVector2D screenLocation;
	if (playerController->ProjectWorldLocationToScreen(GetActorLocation(), screenLocation))
	{
		UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
		float screenMargin = 0.0f; // Default value
		if (constantManager != nullptr)
		{
			screenMargin = constantManager->GetFloatValue(FName("Bullet.OffScreenMargin"));
		}

		const bool bIsOffScreen = screenLocation.X < -screenMargin || screenLocation.X > viewportSizeX + screenMargin || screenLocation.Y < -screenMargin || screenLocation.Y > viewportSizeY + screenMargin;
		if (bIsOffScreen)
		{
			UBulletPoolSubsystem* bulletPool = GetGameInstance()->GetSubsystem<UBulletPoolSubsystem>();
			if (bulletPool != nullptr)
			{
				bulletPool->ReturnBulletToPool(this);
			}
			else
			{
				Destroy();
			}
		}
	}
}
