
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ColorShootingGameMode.generated.h"

class AColorShootingGameState;

UCLASS()
class COLORSHOOTING_API AColorShootingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AColorShootingGameMode();

	void AddScore(int32 ScoreValue);
};
