// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Common/ShotType.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class ABullet;

// Player character class
UCLASS()
class COLORSHOOTING_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Adds one bomb to the stock */
	UFUNCTION(BlueprintCallable, Category = "Status")
	void AddBomb();

	UFUNCTION(BlueprintCallable, Category = "Status")
	int32 GetBombStock() const;

	/**
	 * Adds a level to the specified shot type.
	 * @param ShotType The type of shot to level up.
	 */
	UFUNCTION(BlueprintCallable, Category = "Status")
	void AddShotLevel(const EShotType ShotType);

protected:
	// Input Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> M_PlayerMappingContext;

	// Move Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> M_MoveAction;

	// Look Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> M_LookAction;

	// Fire Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> M_FireAction;

	// Bomb Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> M_BombAction;

	// Change Weapon Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> M_ChangeWeaponAction;

	// Handles move input
	void Move(const FInputActionValue& Value);
	// Handles look input
	void Look(const FInputActionValue& Value);
	// Handles fire input
	void Fire(const FInputActionValue& Value);
	// Handles bomb input
	void Bomb(const FInputActionValue& Value);
	// Handles weapon change input
	void ChangeWeapon(const FInputActionValue& Value);

private:
	// Fires the red shot type
	void FireRedShot();
	// Fires the green shot type
	void FireGreenShot();
	// Fires the blue shot type
	void FireBlueShot();

	/**
	 * @brief Initializes and activates a bullet with the given parameters.
	 * @param NewBullet The bullet actor to initialize.
	 * @param SpawnLocation The location to spawn the bullet.
	 * @param SpawnRotation The rotation of the bullet.
	 * @param BulletMaterial The material to apply to the bullet (optional).
	 * @param ShotType The shot type of the bullet (optional).
	 */
	void InitializeAndActivateBullet(ABullet* NewBullet, const FVector& SpawnLocation, const FRotator& SpawnRotation, UMaterialInterface* BulletMaterial = nullptr, EShotType ShotType = EShotType::Max);

private:
	/** Current number of bombs in stock */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 M_BombStock = 0;

	/** Current selected shot type */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	EShotType M_CurrentShotType = EShotType::Red;

	/** Current level of the red shot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 M_RedShotLevel = 0;

	/** Current level of the green shot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 M_GreenShotLevel = 0;

	/** Current level of the blue shot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 M_BlueShotLevel = 0;

	/** Max level of the shot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 M_MaxShotLevel = 0;

	/** Max number of bombs in stock */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 M_MaxBombStock = 0;

private:
	/** Timestamp of the last green shot fire */
	double M_LastGreenShotTime = 0.0;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class ABullet> M_PlayerBulletBP;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AGreenBullet> M_PlayerBulletGreenBP;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TObjectPtr<class UMaterialInterface> M_RedBulletMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TObjectPtr<class UMaterialInterface> M_BlueBulletMaterial;
};
