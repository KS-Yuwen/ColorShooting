
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Subsystem/SoundManagerSubsystem.h"
#include "Data/SoundDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Engine/ObjectLibrary.h"

void USoundManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadSoundDataAssets();
}

void USoundManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USoundManagerSubsystem::LoadSoundDataAssets()
{
	UObjectLibrary* objectLibrary = UObjectLibrary::CreateLibrary(USoundDataAsset::StaticClass(), false, GIsEditor);
	if (objectLibrary == nullptr)
	{
		return;
	}

	objectLibrary->AddToRoot();
	objectLibrary->LoadAssetDataFromPath(M_SoundDataPath);

	TArray<FAssetData> assetDatas;
	objectLibrary->GetAssetDataList(assetDatas);

	for (const FAssetData& assetData : assetDatas)
	{
		USoundDataAsset* soundDataAsset = Cast<USoundDataAsset>(assetData.GetAsset());
		if (soundDataAsset != nullptr)
		{
			M_SoundDataAssets.Add(assetData.AssetName, soundDataAsset);
		}
	}
}

void USoundManagerSubsystem::PlayBGM(const FName SoundName)
{
	StopBGM();

	const TObjectPtr<USoundDataAsset>* soundDataAsset = M_SoundDataAssets.Find(SoundName);
	if (soundDataAsset == nullptr || (*soundDataAsset)->Sound == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BGM not found: %s"), *SoundName.ToString());
		return;
	}

	M_BGMComponent = UGameplayStatics::CreateSound2D(GetWorld(), (*soundDataAsset)->Sound);
	if (M_BGMComponent != nullptr)
	{
		M_BGMComponent->Play();
	}
}

void USoundManagerSubsystem::StopBGM()
{
	if (M_BGMComponent != nullptr && M_BGMComponent->IsPlaying())
	{
		M_BGMComponent->Stop();
	}
	M_BGMComponent = nullptr;
}

void USoundManagerSubsystem::PlaySE(const FName SoundName)
{
	const TObjectPtr<USoundDataAsset>* soundDataAsset = M_SoundDataAssets.Find(SoundName);
	if (soundDataAsset == nullptr || (*soundDataAsset)->Sound == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SE not found: %s"), *SoundName.ToString());
		return;
	}

	UGameplayStatics::PlaySound2D(GetWorld(), (*soundDataAsset)->Sound);
}
