#include "Character/BossCharacter.h"
#include "Bullet/Bullet.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "ColorShootingGameMode.h"
#include "Subsystem/GameConstantManager.h"

ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ボスの最大体力を設定
	M_MaxHealth = 2000.0f;

	// ボスの攻撃頻度を設定
	M_FireRate = 0.8f;

	// 発射する弾のクラスを設定 (BP_EnemyBullet)
	static ConstructorHelpers::FClassFinder<ABullet> ProjectileClassFinder(TEXT("/Game/BluePrints/Bullet/Enemy/BP_EnemyBullet.BP_EnemyBullet_C"));
	if (ProjectileClassFinder.Succeeded())
	{
		M_ProjectileClass = ProjectileClassFinder.Class;
	}

	// 左の銃口を作成し、メッシュにアタッチ
	M_Muzzle_Left = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle_Left"));
	M_Muzzle_Left->SetupAttachment(GetMesh());

	// 右の銃口を作成し、メッシュにアタッチ
	M_Muzzle_Right = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle_Right"));
	M_Muzzle_Right->SetupAttachment(GetMesh());
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 初期位置を記録
	M_InitialLocation = GetActorLocation();

	// 一定時間ごとに攻撃パターンを変更するタイマーを開始
	GetWorldTimerManager().SetTimer(M_AttackPatternTimerHandle, this, &ABossCharacter::ChangeAttackPattern, M_AttackPatternChangeInterval, true, M_AttackPatternChangeInterval);
}

void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 移動方向を決定 (Y軸方向)
	const FVector Direction = M_bMoveRight ? FVector::RightVector : -FVector::RightVector;

	// 移動
	AddActorWorldOffset(Direction * M_MovementSpeed * DeltaTime);

	// 現在位置を取得
	const FVector CurrentLocation = GetActorLocation();

	// 範囲チェックと方向転換
	if ((M_bMoveRight && CurrentLocation.Y >= M_InitialLocation.Y + M_MovementRange) ||
		(!M_bMoveRight && CurrentLocation.Y <= M_InitialLocation.Y - M_MovementRange))
	{
		M_bMoveRight = !M_bMoveRight;
	}
}

float ABossCharacter::TakeDamage(float damageAmount, FDamageEvent const& damageEvent, AController* eventInstigator, AActor* damageCauser)
{
	const float actualDamage = Super::TakeDamage(damageAmount, damageEvent, eventInstigator, damageCauser);

	if (actualDamage > 0.f)
	{
		// 必要であればここに被弾時の追加処理を記述
	}

	return actualDamage;
}

void ABossCharacter::OnDeath_Implementation()
{
	// Ensure death logic only runs once.
	if (M_bIsDead)
	{
		return;
	}
	M_bIsDead = true;

	// Add score
	AColorShootingGameMode* gameMode = Cast<AColorShootingGameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode != nullptr)
	{
		UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
		if (constantManager != nullptr)
		{
			const int32 score = constantManager->GetIntValue(FName("Score.BossKill"));
			gameMode->AddScore(score);
		}
	}

	// Broadcast death event
	M_OnBossDied.Broadcast();

	// Stop all timers
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// Call parent implementation (handles item drops, etc.)
	Super::OnDeath_Implementation();
}

void ABossCharacter::ChangeAttackPattern()
{
	// 現在の攻撃パターンを基に次のパターンを決定（現在は2種類のトグル）
	if (M_CurrentAttackPattern == EBossAttackPattern::Burst)
	{
		M_CurrentAttackPattern = EBossAttackPattern::Fan;
	}
	else
	{
		M_CurrentAttackPattern = EBossAttackPattern::Burst;
	}
}

void ABossCharacter::Fire()
{
	// 現在の攻撃パターンに応じて処理を分岐
	switch (M_CurrentAttackPattern)
	{
	case EBossAttackPattern::Burst:
		Fire_Burst();
		break;
	case EBossAttackPattern::Fan:
		Fire_FanShot();
		break;
	}
}

void ABossCharacter::Fire_Burst()
{
	if (M_ProjectileClass == nullptr)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn == nullptr)
	{
		return;
	}

	// 発射パラメータ
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 3つの銃口からプレイヤーを狙って弾を発射
	const TArray<USceneComponent*> Muzzles = { M_Muzzle, M_Muzzle_Left, M_Muzzle_Right };
	for (USceneComponent* Muzzle : Muzzles)
	{
		if (Muzzle == nullptr)
		{
			continue;
		}

		const FVector SpawnLocation = Muzzle->GetComponentLocation();
		const FRotator SpawnRotation = (PlayerPawn->GetActorLocation() - SpawnLocation).Rotation();

		ABullet* Bullet = World->SpawnActor<ABullet>(M_ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (Bullet)
		{
			Bullet->M_bIsPlayerBullet = false;
			Bullet->M_ShotType = M_ColorType;
		}

		// 発射エフェクトとサウンドを再生
		if (M_MuzzleFlashEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), M_MuzzleFlashEffect, SpawnLocation, SpawnRotation);
		}
		if (M_FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, M_FireSound, SpawnLocation);
		}
	}
}

void ABossCharacter::Fire_FanShot()
{
	if (M_ProjectileClass == nullptr || M_FanShot_BulletCount <= 0)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn == nullptr)
	{
		return;
	}

	// 中央の銃口位置とプレイヤーへの向きを基準とする
	const FVector SpawnLocation = M_Muzzle->GetComponentLocation();
	const FRotator BaseRotation = (PlayerPawn->GetActorLocation() - SpawnLocation).Rotation();

	// 発射パラメータ
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const float HalfAngle = M_FanShot_Angle / 2.0f;
	// 弾が1つの場合は中央に発射
	const float AngleStep = (M_FanShot_BulletCount > 1) ? (M_FanShot_Angle / (M_FanShot_BulletCount - 1)) : 0.0f;

	for (int32 i = 0; i < M_FanShot_BulletCount; ++i)
	{
		const float AngleOffset = (AngleStep * i) - HalfAngle;
		const FRotator SpawnRotation = BaseRotation + FRotator(0.0f, AngleOffset, 0.0f);

		ABullet* Bullet = World->SpawnActor<ABullet>(M_ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (Bullet)
		{
			Bullet->M_bIsPlayerBullet = false;
			Bullet->M_ShotType = M_ColorType;
		}
	}

	// 発射エフェクトとサウンドは中央の銃口でのみ再生
	if (M_MuzzleFlashEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), M_MuzzleFlashEffect, SpawnLocation, BaseRotation);
	}
	if (M_FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, M_FireSound, SpawnLocation);
	}
}
