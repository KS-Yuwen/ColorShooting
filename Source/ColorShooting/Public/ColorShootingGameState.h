
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

    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetHighScore() const;

    UFUNCTION(BlueprintCallable, Category = "Lives")
    void RemoveLife();

    UFUNCTION(BlueprintPure, Category = "Lives")
    int32 GetLives() const;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score", meta = (AllowPrivateAccess = "true"))
    int32 M_Score;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score", meta = (AllowPrivateAccess = "true"))
    int32 M_HighScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lives", meta = (AllowPrivateAccess = "true"))
    int32 M_Lives;
};
