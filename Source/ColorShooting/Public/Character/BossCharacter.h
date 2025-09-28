#pragma once

#include "CoreMinimal.h"
#include "Character/EnemyCharacter.h"
#include "BossCharacter.generated.h"

UENUM(BlueprintType)
enum class EBossAttackPattern : uint8
{
	Burst UMETA(DisplayName = "3-Way Burst"),
	Fan   UMETA(DisplayName = "Fan Shot")
};

/**
 * 
 */
UCLASS()
class COLORSHOOTING_API ABossCharacter : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	DECLARE_EVENT(ABossCharacter, FOnBossDied)

public:
	ABossCharacter();

protected:
	virtual void BeginPlay() override;

	// 死亡処理
	virtual void OnDeath_Implementation() override;

	// 攻撃パターンに応じて攻撃を実行
	virtual void Fire() override;

	// 攻撃パターンを切り替える
	virtual void ChangeAttackPattern();

	// 3点バースト攻撃
	virtual void Fire_Burst();

	// 扇状攻撃
	virtual void Fire_FanShot();

public:
	virtual void Tick(float DeltaTime) override;

	// ボス死亡イベント
	FOnBossDied& OnBossDied() { return M_OnBossDied; }

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// 左の銃口
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USceneComponent> M_Muzzle_Left;

	// 右の銃口
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USceneComponent> M_Muzzle_Right;

	// --- 攻撃パターン関連 ---
	// 現在の攻撃パターン
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Attack")
	EBossAttackPattern M_CurrentAttackPattern = EBossAttackPattern::Burst;

	// パターンを変更する間隔（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
	float M_AttackPatternChangeInterval = 10.0f;

	// --- 扇状攻撃のパラメータ ---
	// 扇状攻撃の弾の数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack|Fan", meta = (ClampMin = "1"))
	int32 M_FanShot_BulletCount = 5;

	// 扇状攻撃の広がる角度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack|Fan", meta = (ClampMin = "0.0"))
	float M_FanShot_Angle = 60.0f;

	// --- 移動関連 ---
	// 移動範囲（Y軸方向）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Movement")
	float M_MovementRange = 500.0f;

	// 移動速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Movement")
	float M_MovementSpeed = 200.0f;

private:
	// 初期位置
	FVector M_InitialLocation;

	// 現在の移動方向（true: 右, false: 左）
	bool M_bMoveRight = true;

	// 攻撃パターン変更タイマーのハンドル
	FTimerHandle M_AttackPatternTimerHandle;

	// ボス死亡イベント
	FOnBossDied M_OnBossDied;
};
