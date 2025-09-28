



#include "ColorShootingGameMode.h"
#include "Subsystem/SoundManagerSubsystem.h"
#include "Subsystem/GameConstantManager.h"
#include "ColorShootingGameState.h"
#include "UI/PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PlayerCharacter.h"
#include "Character/BossCharacter.h"
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
	if (soundManager == nullptr)
	{
		return;
	}

	soundManager->PlayBGM(M_StageBGMSoundName);

    SetLevelCameraActive();

	// Spawn the boss after a delay
	FTimerHandle unusedHandle;
	GetWorldTimerManager().SetTimer(unusedHandle, this, &AColorShootingGameMode::SpawnBoss, 5.0f, false);
}

void AColorShootingGameMode::AddScore(const int32& scoreValue)
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
	SetLevelCameraActive();

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
	if (playerController == nullptr)
	{
		return;
	}

	RestartPlayer(playerController);
	SetLevelCameraActive();
}

void AColorShootingGameMode::GameOver()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController == nullptr)
	{
		return;
	}

	// Pause the game. The PlayerController will still be able to tick and process input.
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	// Set input mode to UI only.
	FInputModeUIOnly inputMode;
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	// Create and add the GameOver widget to the viewport
	if (M_GameOverWidgetClass == nullptr)
	{
		return;
	}

	UUserWidget* gameOverWidget = CreateWidget<UUserWidget>(playerController, M_GameOverWidgetClass);
	if (gameOverWidget == nullptr)
	{
		return;
	}

	gameOverWidget->AddToViewport();
	inputMode.SetWidgetToFocus(gameOverWidget->TakeWidget());

	playerController->SetInputMode(inputMode);
	playerController->bShowMouseCursor = true;
}

void AColorShootingGameMode::SpawnBoss()
{
	if (M_BossClass == nullptr)
	{
		return;
	}

	UWorld* const world = GetWorld();
	if (world == nullptr)
	{
		return;
	}

	// Define spawn parameters
	// TODO: Make spawn location data-driven
	const FVector spawnLocation = FVector(0.f, 0.f, 500.f);
	const FRotator spawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn the boss
	ABossCharacter* boss = world->SpawnActor<ABossCharacter>(M_BossClass, spawnLocation, spawnRotation, spawnParams);
	if (boss)
	{
		// Bind to the boss's death event
		boss->OnBossDied().AddUObject(this, &AColorShootingGameMode::OnBossDied);
	}
}

void AColorShootingGameMode::OnBossDied()
{
	// Call StageClear after a delay
	FTimerHandle unusedHandle;
	GetWorldTimerManager().SetTimer(unusedHandle, this, &AColorShootingGameMode::StageClear, 3.0f, false);
}

void AColorShootingGameMode::StageClear()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController == nullptr)
	{
		return;
	}

	// Pause the game
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	// Set input mode to UI only
	FInputModeUIOnly inputMode;
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	// Create and add the StageClear widget to the viewport
	if (M_StageClearWidgetClass == nullptr)
	{
		return;
	}

	UUserWidget* stageClearWidget = CreateWidget<UUserWidget>(playerController, M_StageClearWidgetClass);
	if (stageClearWidget == nullptr)
	{
		return;
	}

	stageClearWidget->AddToViewport();
	inputMode.SetWidgetToFocus(stageClearWidget->TakeWidget());

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
