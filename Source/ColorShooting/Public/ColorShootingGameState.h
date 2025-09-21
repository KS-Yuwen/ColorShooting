
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ColorShootingGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreChangedSignature, int32, NewScore, int32, NewHighScore);

UCLASS()
class COLORSHOOTING_API AColorShootingGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AColorShootingGameState();

    //~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(const int32& ScoreValue);

    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const;

    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetHighScore() const;

    UFUNCTION(BlueprintCallable, Category = "Lives")
    void RemoveLife();

    UFUNCTION(BlueprintPure, Category = "Lives")
    int32 GetLives() const;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnIntChangedSignature OnLivesChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnScoreChangedSignature OnScoreChanged;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score", meta = (AllowPrivateAccess = "true"))
    int32 M_Score;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score", meta = (AllowPrivateAccess = "true"))
    int32 M_HighScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lives", meta = (AllowPrivateAccess = "true"))
    int32 M_Lives;
};
