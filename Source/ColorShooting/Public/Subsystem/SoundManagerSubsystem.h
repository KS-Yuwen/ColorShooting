
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SoundManagerSubsystem.generated.h"

class USoundDataAsset;
class UAudioComponent;

/**
 * Manages all game sounds (BGM and SE).
 */
UCLASS()
class COLORSHOOTING_API USoundManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayBGM(const FName SoundName);

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void StopBGM();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlaySE(const FName SoundName);

private:
	void LoadSoundDataAssets();

	UPROPERTY()
	TMap<FName, TObjectPtr<USoundDataAsset>> M_SoundDataAssets;

	UPROPERTY(EditDefaultsOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	FString M_SoundDataPath = TEXT("/Game/Data/Sounds");

	UPROPERTY()
	TObjectPtr<UAudioComponent> M_BGMComponent;
};
