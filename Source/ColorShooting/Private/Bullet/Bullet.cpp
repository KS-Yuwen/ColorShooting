// Copyright Epic Games, Inc. All Rights Reserved.

#include "Bullet/Bullet.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/EnemyCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/BulletPoolSubsystem.h"
#include "Subsystem/GameConstantManager.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	M_CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	M_CollisionComponent->InitSphereRadius(5.0f); // Fallback value
	M_CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	M_CollisionComponent->OnComponentHit.AddDynamic(this, &ABullet::OnHit);

	// Enable hit events
	M_CollisionComponent->SetNotifyRigidBodyCollision(true);

	// Set the sphere's collision profile name to "Projectile"
	RootComponent = M_CollisionComponent;

	// Add a static mesh component for the bullet's appearance
	M_BulletMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMeshComponent"));
	M_BulletMeshComponent->SetupAttachment(RootComponent);

	// Set a default mesh and material
	if (M_DefaultMesh)
	{
		M_BulletMeshComponent->SetStaticMesh(M_DefaultMesh);
	}
	if (M_DefaultMaterial)
	{
		M_BulletMeshComponent->SetMaterial(0, M_DefaultMaterial);
	}
	M_BulletMeshComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f)); // Fallback value


	// Use this component to drive this projectile's movement.
	M_ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	M_ProjectileMovementComponent->SetUpdatedComponent(M_CollisionComponent);
	M_ProjectileMovementComponent->InitialSpeed = 3000.f; // Fallback value
	M_ProjectileMovementComponent->MaxSpeed = 3000.f; // Fallback value
	M_ProjectileMovementComponent->bRotationFollowsVelocity = true;
	M_ProjectileMovementComponent->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f; // Fallback value

	// Initialize the bullet as not being from the player by default
	M_bIsPlayerBullet = false;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
	if (constantManager != nullptr)
	{
		M_CollisionComponent->InitSphereRadius(constantManager->GetFloatValue(FName("Bullet.CollisionRadius")));
		float scale = constantManager->GetFloatValue(FName("Bullet.MeshScale"));
		M_BulletMeshComponent->SetRelativeScale3D(FVector(scale, scale, scale));
		M_ProjectileMovementComponent->InitialSpeed = constantManager->GetFloatValue(FName("Bullet.InitialSpeed"));
		M_ProjectileMovementComponent->MaxSpeed = constantManager->GetFloatValue(FName("Bullet.MaxSpeed"));
		InitialLifeSpan = constantManager->GetFloatValue(FName("Bullet.LifeSpan"));
	}
	else
	{
		// Fallback values if constantManager is not available
		M_CollisionComponent->InitSphereRadius(5.0f);
		M_BulletMeshComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
		M_ProjectileMovementComponent->InitialSpeed = 3000.f;
		M_ProjectileMovementComponent->MaxSpeed = 3000.f;
		InitialLifeSpan = 3.0f;
	}
}

// Called every frame
void ABullet::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

}

void ABullet::SetActive(bool bIsActive)
{
	SetActorHiddenInGame(!bIsActive);
	SetActorEnableCollision(bIsActive);
	SetActorTickEnabled(bIsActive);

	if (bIsActive)
	{
		// When re-enabling collision, we must restore the collision profile.
		if (M_CollisionComponent)
		{
			M_CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
		}
		M_ProjectileMovementComponent->Activate();
		// 寿命をリセット
		UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
		if (constantManager != nullptr)
		{
			SetLifeSpan(constantManager->GetFloatValue(FName("Bullet.LifeSpan")));
		}
		else
		{
			SetLifeSpan(3.0f); // Fallback value
		}
	}
	else
	{
		M_ProjectileMovementComponent->Deactivate();
		SetActorLocation(FVector::ZeroVector);
		// 寿命を0にして即時破棄されないようにする
		SetLifeSpan(0.0f);
	}
}

void ABullet::SetDirection(const FVector& direction)
{
	M_ProjectileMovementComponent->Velocity = direction.GetSafeNormal() * M_ProjectileMovementComponent->InitialSpeed;
}

void ABullet::LifeSpanExpired()
{
	Super::LifeSpanExpired();

	UBulletPoolSubsystem* bulletPool = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UBulletPoolSubsystem>();
	if (bulletPool == nullptr)
	{
		return;
	}
	bulletPool->ReturnBulletToPool(this);
}

// Function that is called when the projectile hits something.
void ABullet::OnHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector normalImpulse, const FHitResult& hit)
{
	// If the other actor is this bullet itself, do nothing.
	if (otherActor == this)
	{
		return;
	}

	// Handle player bullet hitting an enemy
	if (M_bIsPlayerBullet)
	{
		AEnemyCharacter* enemyCharacter = Cast<AEnemyCharacter>(otherActor);
		if (enemyCharacter)
		{
			// If the enemy's color matches the bullet's color, reflect the bullet.
			if (enemyCharacter->GetColorType() == M_ShotType)
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
				M_bWasReflected = true;
				return; // Do not deactivate the bullet, as it has been reflected.
			}
			
			// If the colors do not match, apply damage to the enemy.
			UGameplayStatics::ApplyDamage(enemyCharacter, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}

	// If the hit component is simulating physics, add an impulse.
	if (otherComponent && otherComponent->IsSimulatingPhysics())
	{
		UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
		float impulseStrength = 100.0f; // Default value
		if (constantManager != nullptr)
		{
			impulseStrength = constantManager->GetFloatValue(FName("Bullet.ImpulseStrength"));
		}
		otherComponent->AddImpulseAtLocation(GetVelocity() * impulseStrength, GetActorLocation());
	}

	// Return the bullet to the pool.
	UBulletPoolSubsystem* bulletPool = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UBulletPoolSubsystem>();
	if (bulletPool == nullptr)
	{
		return;
	}
	bulletPool->ReturnBulletToPool(this);
}
