#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class COLORSHOOTING_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	APlayerHUD();

	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	class UUserWidget* PlayerHUDWidget;
};
