
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ColorShootingGameMode.generated.h"

class AColorShootingGameState;
class APlayerHUD;
class APlayerCharacter;

UCLASS()
class COLORSHOOTING_API AColorShootingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AColorShootingGameMode();

	void AddScore(const int32& scoreValue);

	/** Called when the player has died. */
	void PlayerDied(APlayerCharacter* DeadPlayer);

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	FName M_StageBGMSoundName = TEXT("StageBGM");

	/** The delay before the player respawns after dying. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	float M_RespawnDelay = 3.0f;

	/** The widget class to use for the game over screen. */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> M_GameOverWidgetClass;

private:
	/** Timer handle for respawning the player. */
	FTimerHandle M_RespawnTimerHandle;

	/** Function to handle respawning the player. */
	void RespawnPlayer();

	/** Function to handle game over logic. */
	void GameOver();

	/** Sets the main level camera as the active view target. */
	void SetLevelCameraActive();
};
