
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ColorShootingGameState.generated.h"

UCLASS()
class COLORSHOOTING_API AColorShootingGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AColorShootingGameState();

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(const int32 ScoreValue);

    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score", meta = (AllowPrivateAccess = "true"))
    int32 M_Score;
};
