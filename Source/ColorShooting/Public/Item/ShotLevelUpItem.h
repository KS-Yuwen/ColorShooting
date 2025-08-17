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
	GENERATED_UCLASS_BODY()

public:
	AShotLevelUpItem();

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

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<class UMaterialInstanceConstant> M_RedMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<class UMaterialInstanceConstant> M_GreenMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<class UMaterialInstanceConstant> M_BlueMaterial;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	FName M_PickupSoundName = TEXT("ItemPickup");
};
