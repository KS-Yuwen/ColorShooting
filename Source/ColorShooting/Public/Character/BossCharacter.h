#pragma once

#include "CoreMinimal.h"
#include "Character/EnemyCharacter.h"
#include "BossCharacter.generated.h"

/**
 * 
 */
UCLASS()
class COLORSHOOTING_API ABossCharacter : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	ABossCharacter();

protected:
	virtual void BeginPlay() override;

	// 3点バースト攻撃
	virtual void Fire() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	// 左の銃口
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USceneComponent> M_Muzzle_Left;

	// 右の銃口
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USceneComponent> M_Muzzle_Right;

	// 移動範囲（Y軸方向）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float M_MovementRange = 500.0f;

	// 移動速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float M_MovementSpeed = 200.0f;

private:
	// 初期位置
	FVector M_InitialLocation;

	// 現在の移動方向（true: 右, false: 左）
	bool M_bMoveRight = true;
};
