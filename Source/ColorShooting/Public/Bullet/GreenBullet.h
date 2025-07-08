// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bullet/Bullet.h"
#include "GreenBullet.generated.h"

UCLASS()
class COLORSHOOTING_API AGreenBullet : public ABullet
{
	GENERATED_BODY()

public:
	AGreenBullet();

	virtual void Tick(float DeltaTime) override;

	/** ターゲットを設定します */
	void SetTarget(AActor* NewTarget);

protected:
	/** 弾が何かに当たったときに呼び出されます */
	UFUNCTION()
	void OnGreenBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

private:
	/** 追尾する敵 */
	UPROPERTY(VisibleAnywhere, Category = "Homing")
	TWeakObjectPtr<AActor> M_TargetEnemy;

	/** ターゲットに命中したかどうか */
	bool M_bHasHitTarget;

	/** 画面外に出たかどうかをチェックします */
	void CheckIfOffScreen();
};
