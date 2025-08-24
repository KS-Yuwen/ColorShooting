#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

UCLASS()
class COLORSHOOTING_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	APlayerHUD();

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	/** Blueprint-implementable event for updating the lives display. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnUpdateLives(int32 NewLives);

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> PlayerHUDWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	TObjectPtr<class UUserWidget> PlayerHUDWidget;

private:
	/** Handler for the OnLivesChanged event from the GameState. */
	UFUNCTION()
	void HandleLivesChanged(int32 NewLives);
};
