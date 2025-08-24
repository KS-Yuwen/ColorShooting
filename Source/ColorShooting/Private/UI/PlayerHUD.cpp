#include "UI/PlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "ColorShootingGameState.h"

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

	// Bind to the OnLivesChanged event in the GameState
	AColorShootingGameState* gameState = GetWorld()->GetGameState<AColorShootingGameState>();
	if (gameState)
	{
		// Bind to receive all FUTURE updates.
		gameState->OnLivesChanged.AddDynamic(this, &APlayerHUD::HandleLivesChanged);

		// Manually get the CURRENT state to initialize the HUD.
		HandleLivesChanged(gameState->GetLives());
	}
}

void APlayerHUD::HandleLivesChanged(int32 NewLives)
{
	// This C++ function is called, which in turn calls the Blueprint-implementable event.
	OnUpdateLives(NewLives);
}
