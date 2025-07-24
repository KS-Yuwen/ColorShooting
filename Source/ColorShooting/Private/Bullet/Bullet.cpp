// Copyright Epic Games, Inc. All Rights Reserved.

#include "Bullet/Bullet.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/EnemyCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	M_CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	M_CollisionComponent->InitSphereRadius(5.0f);
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
	M_BulletMeshComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));


	// Use this component to drive this projectile's movement.
	M_ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	M_ProjectileMovementComponent->SetUpdatedComponent(M_CollisionComponent);
	M_ProjectileMovementComponent->InitialSpeed = 3000.f;
	M_ProjectileMovementComponent->MaxSpeed = 3000.f;
	M_ProjectileMovementComponent->bRotationFollowsVelocity = true;
	M_ProjectileMovementComponent->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	// Initialize the bullet as not being from the player by default
	bIsPlayerBullet = false;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	}
	else
	{
		M_ProjectileMovementComponent->Deactivate();
		SetActorLocation(FVector::ZeroVector);
	}
}

void ABullet::SetDirection(const FVector& Direction)
{
	M_ProjectileMovementComponent->Velocity = Direction.GetSafeNormal() * M_ProjectileMovementComponent->InitialSpeed;
}

// Function that is called when the projectile hits something.
void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// If the other actor is invalid or is this bullet itself, deactivate and return.
	if (OtherActor == nullptr || OtherActor == this || OtherComponent == nullptr)
	{
		SetActive(false);
		return;
	}

	// Handle player bullet hitting an enemy
	if (bIsPlayerBullet)
	{
		if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OtherActor))
		{
			// If the enemy's color matches the bullet's color, reflect the bullet.
			if (EnemyCharacter->GetColorType() == M_ShotType)
			{
				const FVector ReflectionVector = FMath::GetReflectionVector(GetVelocity(), Hit.ImpactNormal);
				const FVector RandomizedReflectionVector = ReflectionVector + FMath::VRand() * 500.0f;
				M_ProjectileMovementComponent->Velocity = RandomizedReflectionVector.GetSafeNormal() * M_ProjectileMovementComponent->InitialSpeed;
				bWasReflected = true;
				return; // Do not deactivate the bullet, as it has been reflected.
			}
			
			// If the colors do not match, apply damage to the enemy.
			UGameplayStatics::ApplyDamage(EnemyCharacter, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}

	// If the hit component is simulating physics, add an impulse.
	if (OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}

	// Deactivate the bullet after handling the hit.
	SetActive(false);
}
