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
	// Color type of the enemy (0: Red, 1: Green, 2: Blue)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	int32 M_ColorType = 0;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
