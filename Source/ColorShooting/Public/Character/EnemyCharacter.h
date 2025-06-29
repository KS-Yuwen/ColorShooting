#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "EnemyCharacter.generated.h"

// 敵キャラクタークラス
UCLASS()
class COLORSHOOTING_API AEnemyCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	// 色の種類 (0: 赤, 1: 緑, 2: 青)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	int32 ColorType = 0;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
