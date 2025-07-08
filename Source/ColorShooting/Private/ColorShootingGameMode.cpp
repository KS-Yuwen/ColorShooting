
#include "ColorShootingGameMode.h"
#include "ColorShootingGameState.h"
#include "Kismet/GameplayStatics.h"

AColorShootingGameMode::AColorShootingGameMode()
{
	GameStateClass = AColorShootingGameState::StaticClass();
}

void AColorShootingGameMode::AddScore(const int32 ScoreValue)
{
    AColorShootingGameState* GS = GetGameState<AColorShootingGameState>();
    if (GS == nullptr)
    {
        return;
    }
    GS->AddScore(ScoreValue);
}
