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
	// 体力
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	float Health = 100.0f;

	// 死亡時の処理
	virtual void OnDeath();
};
