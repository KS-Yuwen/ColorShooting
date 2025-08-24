
#include "ColorShootingGameState.h"
#include "Subsystem/GameConstantManager.h"

AColorShootingGameState::AColorShootingGameState()
{
    M_Score = 0;
    M_HighScore = 0; 
    M_Lives = 0; 
}

void AColorShootingGameState::BeginPlay()
{
    Super::BeginPlay();

    UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
	if (constantManager)
	{
		M_Lives = constantManager->GetIntValue(FName("Player.InitialLives"));
	}

    // Broadcast the initial values
    OnLivesChanged.Broadcast(M_Lives);
    OnScoreChanged.Broadcast(M_Score, M_HighScore);
}

void AColorShootingGameState::AddScore(const int32 ScoreValue)
{
    M_Score += ScoreValue;
    if (M_Score > M_HighScore)
    {
        M_HighScore = M_Score;
    }
    OnScoreChanged.Broadcast(M_Score, M_HighScore);
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
        OnLivesChanged.Broadcast(M_Lives);
    }
}

int32 AColorShootingGameState::GetLives() const
{
    return M_Lives;
}
