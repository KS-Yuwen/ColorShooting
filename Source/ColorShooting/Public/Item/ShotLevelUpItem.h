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
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Type", meta = (AllowPrivateAccess = "true"))
	EShotType M_ShotType = EShotType::Red;
	
};
