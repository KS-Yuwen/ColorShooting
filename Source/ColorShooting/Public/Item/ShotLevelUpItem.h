#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "Common/ShotType.h"
#include "ShotLevelUpItem.generated.h"

/**
 * 
 */
UCLASS()
class COLORSHOOTING_API AShotLevelUpItem : public AItemBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnOverlapBegin(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult) override;

private:
	UFUNCTION()
	void ChangeShotType();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Type", meta = (AllowPrivateAccess = "true"))
	EShotType M_ShotType = EShotType::Red;

	FTimerHandle M_TimerHandle;
	
};
