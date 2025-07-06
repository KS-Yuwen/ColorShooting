// Fill out your copyright notice in the Description page of Project Settings.

#include "Bullet/GreenBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/EnemyCharacter.h"

AGreenBullet::AGreenBullet()
{
	// ホーミング弾の設定
	ProjectileMovementComponent->bIsHomingProjectile = true;
	ProjectileMovementComponent->HomingAccelerationMagnitude = 2000.0f;

	// OnHitイベントを新しい関数にバインドしなおす
	CollisionComponent->OnComponentHit.RemoveDynamic(this, &ABullet::OnHit);
	CollisionComponent->OnComponentHit.AddDynamic(this, &AGreenBullet::OnGreenBulletHit);

	bHasHitTarget = false;
}

void AGreenBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ターゲットに命中したら、ホーミングを停止
	if (bHasHitTarget)
	{
		ProjectileMovementComponent->bIsHomingProjectile = false;
	}
	else if (TargetEnemy.IsValid())
	{
		// ターゲットを追いかける
		ProjectileMovementComponent->HomingTargetComponent = TargetEnemy->GetRootComponent();
	}

	CheckIfOffScreen();
}

void AGreenBullet::SetTarget(AActor* NewTarget)
{
	TargetEnemy = NewTarget;
}

void AGreenBullet::OnGreenBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// 敵に当たった場合
	if (Cast<AEnemyCharacter>(OtherActor))
	{
		bHasHitTarget = true;
		// 貫通するのでDestroy()は呼び出さない
	}
	else if (OtherActor != this)
	{
		// 敵以外（壁など）に当たった場合は消滅
		Destroy();
	}
}

void AGreenBullet::CheckIfOffScreen()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		int32 ViewportSizeX, ViewportSizeY;
		PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

		FVector2D ScreenLocation;
		if (PlayerController->ProjectWorldLocationToScreen(GetActorLocation(), ScreenLocation))
		{
			// 画面外に出たら消滅
			if (ScreenLocation.X < 0 || ScreenLocation.X > ViewportSizeX || ScreenLocation.Y < 0 || ScreenLocation.Y > ViewportSizeY)
			{
				Destroy();
			}
		}
	}
}
