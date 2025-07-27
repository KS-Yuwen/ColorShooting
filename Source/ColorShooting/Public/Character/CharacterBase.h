#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, OldHealth, float, NewHealth);

// 全てのキャラクターの基底クラス
UCLASS()
class COLORSHOOTING_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handles taking damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const { return M_Health; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const { return M_MaxHealth; }

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedSignature OnHealthChanged;

protected:
	virtual void BeginPlay() override;

	// Muzzle for firing projectiles
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<USceneComponent> M_Muzzle;

	// Maximum health of the character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float M_MaxHealth = 100.0f;

	// Current health of the character
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	float M_Health;

	// Flag to indicate if the character is dead
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	bool M_bIsDead = false;

	// Handles the character's death
	UFUNCTION(BlueprintNativeEvent, Category = "Character")
	void OnDeath();
	virtual void OnDeath_Implementation();
};
