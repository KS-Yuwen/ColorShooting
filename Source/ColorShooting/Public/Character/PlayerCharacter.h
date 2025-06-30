#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "InputActionValue.h"
#include "Common/ShotType.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;

// プレイヤーキャラクタークラス
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

	// 入力コンポーネントの設定
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 入力マッピングコンテキスト
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* PlayerMappingContext;

	// 移動アクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	// 見回すアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	// 発射アクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* FireAction;

	// ボムアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BombAction;

	// 武器変更アクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ChangeWeaponAction;

	// 移動処理
	void Move(const FInputActionValue& Value);
	// 見回す処理
	void Look(const FInputActionValue& Value);
	// 発射処理
	void Fire(const FInputActionValue& Value);
	// ボム処理
	void Bomb(const FInputActionValue& Value);
	// 武器変更処理
	void ChangeWeapon(const FInputActionValue& Value);

public:
	/** ボムを1つ追加します */
	UFUNCTION(BlueprintCallable, Category = "Status")
	void AddBomb();

	/**
	 * ショットレベルを加算します
	 * @param ShotType ショットタイプ
	 */
	UFUNCTION(BlueprintCallable, Category = "Status")
	void AddShotLevel(EShotType ShotType);

	/**
	 * スコアを加算します
	 * @param Score スコア
	 */
	UFUNCTION(BlueprintCallable, Category = "Status")
	void AddScore(int32 Score);

private:
	/** ボムのストック数 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 BombStock = 0;

	/** 現在のショットタイプ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	EShotType CurrentShotType = EShotType::Red;

	/** 赤ショットのレベル */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 RedShotLevel = 0;

	/** 緑ショットのレベル */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 GreenShotLevel = 0;

	/** 青ショットのレベル */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 BlueShotLevel = 0;

	/** スコア */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	int32 Score = 0;
};
