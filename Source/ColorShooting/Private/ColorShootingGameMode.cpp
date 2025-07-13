
#include "ColorShootingGameMode.h"
#include "ColorShootingGameState.h"
#include "Kismet/GameplayStatics.h"

AColorShootingGameMode::AColorShootingGameMode()
{
	GameStateClass = AColorShootingGameState::StaticClass();
}

void AColorShootingGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Search Level Camera
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("LevelCamera"), FoundActors);
    if (FoundActors.Num() > 0)
    {
        AActor* LevelCamera = FoundActors[0];
        if (LevelCamera)
        {
            // Set the view target to the level camera
            APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PlayerController)
            {
                PlayerController->SetViewTargetWithBlend(LevelCamera);
            }
        }
    }
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
