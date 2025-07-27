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

public:
	virtual void Tick(float DeltaTime) override;

	// TODO: ボス固有の攻撃パターンや移動パターンをここに追加
};
