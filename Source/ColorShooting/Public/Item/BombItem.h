
#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "BombItem.generated.h"

/**
 * 
 */
UCLASS()
class COLORSHOOTING_API ABombItem : public AItemBase
{
	GENERATED_BODY()

protected:
	virtual void OnOverlapBegin(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult) override;
	
};
