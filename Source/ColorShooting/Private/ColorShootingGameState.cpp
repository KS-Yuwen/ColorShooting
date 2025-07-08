
#include "ColorShootingGameState.h"

AColorShootingGameState::AColorShootingGameState()
{
    M_Score = 0;
}

void AColorShootingGameState::AddScore(const int32 ScoreValue)
{
    M_Score += ScoreValue;
}

int32 AColorShootingGameState::GetScore() const
{
    return M_Score;
}
