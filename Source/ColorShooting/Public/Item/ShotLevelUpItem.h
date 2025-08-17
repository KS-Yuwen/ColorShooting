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

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	//~ Begin UPrimitiveComponent Interface
	virtual void OnOverlapBegin(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult) override;
	//~ End UPrimitiveComponent Interface

private:
	/** Changes the shot type of the item periodically. */
	UFUNCTION()
	void ChangeShotType();

	/** The type of shot this item upgrades. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Type", meta = (AllowPrivateAccess = "true"))
	EShotType M_ShotType = EShotType::Red;

	/** Handle for the timer that changes the shot type. */
	FTimerHandle M_TimerHandle;

	/** Material for the red shot type. */
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMaterialInstanceConstant> M_RedMaterial;

	/** Material for the green shot type. */
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMaterialInstanceConstant> M_GreenMaterial;

	/** Material for the blue shot type. */
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMaterialInstanceConstant> M_BlueMaterial;

	/** The name of the sound to play when the item is picked up. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	FName M_PickupSoundName = TEXT("ItemPickup");
};
