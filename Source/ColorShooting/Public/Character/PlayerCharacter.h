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
class UNiagaraSystem;

// Player character class
UCLASS()
class COLORSHOOTING_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	//~ Begin AActor Interface
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	//~ End AActor Interface

	/** Adds one bomb to the stock */
	UFUNCTION(BlueprintCallable, Category = "Status")
	void AddBomb();

	/** Gets the current number of bombs in stock */
	UFUNCTION(BlueprintCallable, Category = "Status")
	int32 GetBombStock() const;

	/**
	 * Adds a level to the specified shot type.
	 * @param shotType The type of shot to level up.
	 */
	UFUNCTION(BlueprintCallable, Category = "Status")
	void AddShotLevel(const EShotType shotType);

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void OnDeath_Implementation() override;
	//~ End AActor Interface

	//~ Begin Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> M_PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> M_MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> M_LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> M_FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> M_BombAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> M_ChangeWeaponAction;

	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Fire(const FInputActionValue& value);
	void Bomb(const FInputActionValue& value);
	void ChangeWeapon(const FInputActionValue& value);
	//~ End Input

	//~ Begin Sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	FName M_FireSoundName = TEXT("PlayerFire");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	FName M_BombSoundName = TEXT("PlayerBomb");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	FName M_ChangeWeaponSoundName = TEXT("PlayerChangeWeapon");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	FName M_DamageSoundName = TEXT("PlayerDamage");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	FName M_DeathSoundName = TEXT("PlayerDeath");
	//~ End Sound

	//~ Begin Health & Damage
	// 無敵時間（秒）
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float M_InvincibilityDuration = 2.0f;

	// 死亡時に再生するVFX
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> M_DeathVFX;
	//~ End Health & Damage

private:
	void FireRedShot();
	void FireGreenShot();
	void FireBlueShot();

	void InitializeAndActivateBullet(ABullet* newBullet, const FVector& spawnLocation, const FRotator& spawnRotation, UMaterialInterface* bulletMaterial = nullptr, EShotType shotType = EShotType::Max);

	void EndInvincibility();

	//~ Begin Status
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 M_BombStock = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	EShotType M_CurrentShotType = EShotType::Red;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 M_RedShotLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 M_GreenShotLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 M_BlueShotLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 M_MaxShotLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 M_MaxBombStock = 0;
	//~ End Status

	//~ Begin Internal State
	double M_LastGreenShotTime = 0.0;
	bool M_bIsInvincible = false;
	FTimerHandle M_InvincibilityTimerHandle;
	//~ End Internal State

	//~ Begin Projectile Assets
	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ABullet> M_PlayerBulletBP;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AGreenBullet> M_PlayerBulletGreenBP;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMaterialInterface> M_RedBulletMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMaterialInterface> M_BlueBulletMaterial;
	//~ End Projectile Assets
};
