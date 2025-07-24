// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Common/ShotType.h"
#include "Bullet.generated.h"

UCLASS()
class COLORSHOOTING_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void LifeSpanExpired() override;

	void SetActive(bool bIsActive);

	/**
	 * @brief Sets the direction of the bullet's movement.
	 * @param Direction The direction vector for the bullet.
	 */
	void SetDirection(const FVector &Direction);

	// Static mesh component for the bullet's appearance
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	TObjectPtr<class UStaticMeshComponent> M_BulletMeshComponent;

	// Default mesh for the bullet
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TObjectPtr<class UStaticMesh> M_DefaultMesh;

	// Default material for the bullet
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TObjectPtr<class UMaterialInterface> M_DefaultMaterial;

	// Sphere collision component
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	TObjectPtr<class USphereComponent> M_CollisionComponent;

	// Projectile movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UProjectileMovementComponent> M_ProjectileMovementComponent;

	/** この弾がプレイヤーによって発射されたかどうかを判定します。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bullet")
	bool bIsPlayerBullet;

	/** この弾が反射されたものかどうかを判定します。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bullet")
	bool bWasReflected;

	/** The color type of this bullet. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bullet")
	EShotType M_ShotType;

	/** The amount of damage this bullet inflicts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet", meta = (ClampMin = "0.0"))
	float Damage = 1.0f;

	// Function that is called when the projectile hits something.
	UFUNCTION()
	void OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComponent, FVector NormalImpulse, const FHitResult &Hit);
};
