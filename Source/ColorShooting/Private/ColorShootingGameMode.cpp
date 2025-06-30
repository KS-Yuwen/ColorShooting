
#include "ColorShootingGameMode.h"
#include "ColorShootingGameState.h"
#include "Kismet/GameplayStatics.h"

AColorShootingGameMode::AColorShootingGameMode()
{
	GameStateClass = AColorShootingGameState::StaticClass();
}

void AColorShootingGameMode::AddScore(int32 ScoreValue)
{
    AColorShootingGameState* GS = GetGameState<AColorShootingGameState>();
    if (GS)
    {
        GS->AddScore(ScoreValue);
    }
}
