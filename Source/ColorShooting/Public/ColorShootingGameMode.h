
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ColorShootingGameMode.generated.h"

class AColorShootingGameState;
class APlayerHUD;

UCLASS()
class COLORSHOOTING_API AColorShootingGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	AColorShootingGameMode();

	void AddScore(const int32 ScoreValue);
};
