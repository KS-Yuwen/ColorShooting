
#include "ColorShootingGameState.h"

AColorShootingGameState::AColorShootingGameState()
{
    M_Score = 0;
    M_HighScore = 0; // Initialize high score
    M_Lives = 3; // Default number of lives
}

void AColorShootingGameState::AddScore(const int32 ScoreValue)
{
    M_Score += ScoreValue;
    if (M_Score > M_HighScore)
    {
        M_HighScore = M_Score; // Update high score if current score exceeds it
    }
}

int32 AColorShootingGameState::GetScore() const
{
    return M_Score;
}

int32 AColorShootingGameState::GetHighScore() const
{
    return M_HighScore;
}

void AColorShootingGameState::RemoveLife()
{
    if (M_Lives > 0)
    {
        M_Lives--;
    }
}

int32 AColorShootingGameState::GetLives() const
{
    return M_Lives;
}
