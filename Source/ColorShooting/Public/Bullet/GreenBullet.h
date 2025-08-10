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
	virtual void BeginPlay() override;

	/** ターゲットを設定します */
	void SetTarget(AActor* newTarget);

	/** 貫通プロパティを設定します */
	void SetPierceProperties(int32 MaxPierces);

	/** 弾のアクティブ状態を切り替えます */
	virtual void SetActive(bool bIsActive) override;

protected:
	/** 弾が何かにオーバーラップしたときに呼び出されます */
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	/** 追尾する敵 */
	UPROPERTY(VisibleAnywhere, Category = "Homing")
	TWeakObjectPtr<AActor> M_TargetEnemy;

	/** ターゲットに命中したかどうか */
	bool M_bHasHitTarget;

	/** 画面外に出たかどうかをチェックします */
	void CheckIfOffScreen();

	/** 貫通できる最大回数 */
	int32 M_MaxPierces = 0;

	/** 現在の貫通回数 */
	int32 M_CurrentPierces = 0;

	/** すでにヒットした敵のリスト */
	TSet<AActor*> M_HitEnemies;
};
