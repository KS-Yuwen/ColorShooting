#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Common/ShotType.h"
#include "EnemyCharacter.generated.h"

class ABullet;
class AItemBase;

// ドロップアイテムの情報
USTRUCT(BlueprintType)
struct FDropItemInfo
{
	GENERATED_BODY()

	// ドロップするアイテムのクラス
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AItemBase> ItemClass;

	// ドロップする確率 (0.0 to 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DropChance = 1.0f;
};

// 敵キャラクタークラス
UCLASS()
class COLORSHOOTING_API AEnemyCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	// Color type of the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	EShotType M_ColorType = EShotType::Red;

	// 発射する弾のクラス
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<ABullet> M_ProjectileClass;

	// 攻撃の頻度（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float M_FireRate = 1.0f;

	// ドロップする可能性のあるアイテムのリスト
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<FDropItemInfo> M_DropItems;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	EShotType GetColorType() const { return M_ColorType; }

	// 攻撃処理
	virtual void Fire();

protected:
	/**
	 * @brief Fires a projectile in a specified direction.
	 * @param FireRotation The rotation that determines the projectile's direction.
	 */
	void FireInDirection(const FRotator& FireRotation);

	virtual void OnDeath_Implementation() override;

private:
	bool M_bKilledByReflectedBullet = false;

protected:
	// 攻撃タイマーのハンドル
	FTimerHandle M_FireTimerHandle;
};
