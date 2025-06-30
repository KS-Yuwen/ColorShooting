
#include "ColorShootingGameState.h"

AColorShootingGameState::AColorShootingGameState()
{
    Score = 0;
}

void AColorShootingGameState::AddScore(int32 ScoreValue)
{
    Score += ScoreValue;
}

int32 AColorShootingGameState::GetScore() const
{
    return Score;
}
