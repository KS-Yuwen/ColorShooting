#include "UI/PlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "ColorShootingGameState.h"
#include "Character/PlayerCharacter.h"

APlayerHUD::APlayerHUD()
{

}

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), PlayerHUDWidgetClass);

		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	// Bind to GameState events
	AColorShootingGameState* gameState = GetWorld()->GetGameState<AColorShootingGameState>();
	if (gameState)
	{
		gameState->OnLivesChanged.AddDynamic(this, &APlayerHUD::HandleLivesChanged);
		gameState->OnScoreChanged.AddDynamic(this, &APlayerHUD::HandleScoreChanged);

		// Manually trigger initial updates
		HandleLivesChanged(gameState->GetLives());
		HandleScoreChanged(gameState->GetScore(), gameState->GetHighScore());
	}

	// Bind to PlayerCharacter events
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwningPawn());
	if (playerCharacter)
	{
		playerCharacter->OnBombsChanged.AddDynamic(this, &APlayerHUD::HandleBombsChanged);

		// Manually trigger initial update
		HandleBombsChanged(playerCharacter->GetBombStock());
	}
}

void APlayerHUD::HandleLivesChanged(int32 NewLives)
{
	OnUpdateLives(NewLives);
}

void APlayerHUD::HandleScoreChanged(int32 NewScore, int32 NewHighScore)
{
	OnUpdateScore(NewScore, NewHighScore);
}

void APlayerHUD::HandleBombsChanged(int32 NewBombs)
{
	OnUpdateBombs(NewBombs);
}
