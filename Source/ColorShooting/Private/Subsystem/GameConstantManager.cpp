#include "Subsystem/GameConstantManager.h"
#include "Engine/DataTable.h"

void UGameConstantManager::Initialize(FSubsystemCollectionBase& collection)
{
	Super::Initialize(collection);

	FString dataTablePath = TEXT("/Game/Data/GameConstants.GameConstants");
	M_ConstantDataTable = LoadObject<UDataTable>(nullptr, *dataTablePath);

	if (M_ConstantDataTable)
	{
		FString contextString;
		TArray<FName> rowNames = M_ConstantDataTable->GetRowNames();
		for (const FName& rowName : rowNames)
		{
			FGameConstant* row = M_ConstantDataTable->FindRow<FGameConstant>(rowName, contextString);
			if (row)
			{
				M_ConstantMap.Add(rowName, *row);
			}
		}
	}
}

void UGameConstantManager::Deinitialize()
{
	Super::Deinitialize();
}

int32 UGameConstantManager::GetIntValue(const FName& constantId) const
{
	if (M_ConstantMap.Contains(constantId))
	{
		const FGameConstant& constant = M_ConstantMap[constantId];
		if (constant.Type == EConstantType::Int)
		{
			return constant.IntValue;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("GameConstant with ID '%s' not found or is not an int."), *constantId.ToString());
	return 0;
}

float UGameConstantManager::GetFloatValue(const FName& constantId) const
{
	if (M_ConstantMap.Contains(constantId))
	{
		const FGameConstant& constant = M_ConstantMap[constantId];
		if (constant.Type == EConstantType::Float)
		{
			return constant.FloatValue;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("GameConstant with ID '%s' not found or is not a float."), *constantId.ToString());
	return 0.0f;
}
