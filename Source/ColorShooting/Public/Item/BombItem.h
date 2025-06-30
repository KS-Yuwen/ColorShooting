
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
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	
};
