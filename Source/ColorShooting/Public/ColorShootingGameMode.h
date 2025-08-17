
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ColorShootingGameMode.generated.h"

class AColorShootingGameState;
class APlayerHUD;

UCLASS()
class COLORSHOOTING_API AColorShootingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AColorShootingGameMode();

	void AddScore(const int32 scoreValue);

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	FName M_StageBGMSoundName = TEXT("StageBGM");
};
