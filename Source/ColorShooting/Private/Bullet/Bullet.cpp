// Copyright Epic Games, Inc. All Rights Reserved.

#include "Bullet/Bullet.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/EnemyCharacter.h"

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

	// Set the sphere's collision profile name to "Projectile"
	RootComponent = M_CollisionComponent;

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
		M_ProjectileMovementComponent->Activate();
	}
	else
	{
		M_ProjectileMovementComponent->Deactivate();
		SetActorLocation(FVector::ZeroVector);
	}
}

// Function that is called when the projectile hits something.
void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == nullptr || OtherActor == this || OtherComponent == nullptr)
	{
		SetActive(false);
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
			bWasReflected = true;
			return; // Don't destroy the bullet
		}
	}

	// Only add impulse and destroy projectile if we hit a physics
	if (OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}
	SetActive(false);
}
