#include "UI/PlayerHUD.h"
#include "Blueprint/UserWidget.h"

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
}

void APlayerHUD::DrawHUD()
{
	Super::DrawHUD();
}
