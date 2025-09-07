
#include "ColorShootingGameMode.h"
#include "Subsystem/SoundManagerSubsystem.h"
#include "Subsystem/GameConstantManager.h"
#include "ColorShootingGameState.h"
#include "UI/PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PlayerCharacter.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

AColorShootingGameMode::AColorShootingGameMode()
{
	GameStateClass = AColorShootingGameState::StaticClass();
	HUDClass = APlayerHUD::StaticClass();
}

void AColorShootingGameMode::BeginPlay()
{
    Super::BeginPlay();

	USoundManagerSubsystem* soundManager = GetGameInstance()->GetSubsystem<USoundManagerSubsystem>();
	if (soundManager != nullptr)
	{
		soundManager->PlayBGM(M_StageBGMSoundName);
	}

    SetLevelCameraActive();
}

void AColorShootingGameMode::AddScore(const int32 scoreValue)
{
    AColorShootingGameState* gameState = GetGameState<AColorShootingGameState>();
    if (gameState == nullptr)
    {
        return;
    }
    gameState->AddScore(scoreValue);
}

void AColorShootingGameMode::PlayerDied(APlayerCharacter* DeadPlayer)
{
	AColorShootingGameState* gameState = GetGameState<AColorShootingGameState>();
	if (gameState == nullptr)
	{
		return;
	}

	gameState->RemoveLife();

	if (gameState->GetLives() > 0)
	{
		// Respawn the player after a delay
		GetWorldTimerManager().SetTimer(M_RespawnTimerHandle, this, &AColorShootingGameMode::RespawnPlayer, M_RespawnDelay, false);
	}
	else
	{
		// No lives left, game over
		GameOver();
	}
}

void AColorShootingGameMode::RespawnPlayer()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController)
	{
		RestartPlayer(playerController);
		SetLevelCameraActive();
	}
}

void AColorShootingGameMode::GameOver()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController == nullptr)
	{
		return;
	}

	// Create and add the GameOver widget to the viewport
	UUserWidget* gameOverWidget = nullptr;
	if (M_GameOverWidgetClass)
	{
		gameOverWidget = CreateWidget<UUserWidget>(playerController, M_GameOverWidgetClass);
		if (gameOverWidget)
		{
			gameOverWidget->AddToViewport();
		}
	}

	// Pause the game. The PlayerController will still be able to tick and process input.
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	// Set input mode to UI only.
	FInputModeUIOnly inputMode;
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	// If the widget was created successfully, set it as the focus for gamepad/keyboard control.
	if (gameOverWidget)
	{
		inputMode.SetWidgetToFocus(gameOverWidget->TakeWidget());
	}

	playerController->SetInputMode(inputMode);
	playerController->bShowMouseCursor = true;
}

void AColorShootingGameMode::SetLevelCameraActive()
{
    // Search Level Camera
    TArray<AActor*> foundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("LevelCamera"), foundActors);
    if (foundActors.Num() <= 0)
    {
        return;
    }

    AActor* levelCamera = foundActors[0];
    if (levelCamera == nullptr)
    {
        return;
    }

    // Set the view target to the level camera
    APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (playerController == nullptr)
    {
        return;
    }

    playerController->SetViewTargetWithBlend(levelCamera);
}
