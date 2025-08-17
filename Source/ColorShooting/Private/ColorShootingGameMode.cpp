
#include "ColorShootingGameMode.h"
#include "Subsystem/SoundManagerSubsystem.h"
#include "ColorShootingGameState.h"
#include "UI/PlayerHUD.h"
#include "Kismet/GameplayStatics.h"

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

void AColorShootingGameMode::AddScore(const int32 scoreValue)
{
    AColorShootingGameState* gameState = GetGameState<AColorShootingGameState>();
    if (gameState == nullptr)
    {
        return;
    }
    gameState->AddScore(scoreValue);
}
