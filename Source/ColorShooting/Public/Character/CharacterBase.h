#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

// 全てのキャラクターの基底クラス
UCLASS()
class COLORSHOOTING_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// 機能を入力にバインドするために呼び出されます
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ダメージを受ける処理
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Muzzle for firing projectiles
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<USceneComponent> M_Muzzle;

	// Current health of the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	float M_Health = 100.0f;

	// Handles the character's death
	virtual void OnDeath();
};
