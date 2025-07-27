#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Common/GameConstant.h"
#include "GameConstantManager.generated.h"

UCLASS()
class COLORSHOOTING_API UGameConstantManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	int32 GetIntValue(const FName& constantId) const;
	float GetFloatValue(const FName& constantId) const;

private:
	UPROPERTY()
	UDataTable* M_ConstantDataTable;

	UPROPERTY()
	TMap<FName, FGameConstant> M_ConstantMap;
};
