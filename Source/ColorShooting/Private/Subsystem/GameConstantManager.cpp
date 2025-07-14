#include "Subsystem/GameConstantManager.h"
#include "Engine/DataTable.h"

void UGameConstantManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FString DataTablePath = TEXT("/Game/Data/GameConstants.GameConstants");
	ConstantDataTable = LoadObject<UDataTable>(nullptr, *DataTablePath);

	if (ConstantDataTable)
	{
		FString ContextString;
		TArray<FName> RowNames = ConstantDataTable->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			FGameConstant* Row = ConstantDataTable->FindRow<FGameConstant>(RowName, ContextString);
			if (Row)
			{
				ConstantMap.Add(RowName, Row->Value);
			}
		}
	}
}

void UGameConstantManager::Deinitialize()
{
	Super::Deinitialize();
}

int32 UGameConstantManager::GetIntValue(const FName& ConstantId) const
{
	if (ConstantMap.Contains(ConstantId))
	{
		return ConstantMap[ConstantId];
	}

	UE_LOG(LogTemp, Warning, TEXT("GameConstant with ID '%s' not found."), *ConstantId.ToString());
	return 0;
}
