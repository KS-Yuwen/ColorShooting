#include "Character/BossCharacter.h"
#include "Bullet/Bullet.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

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

void ABossCharacter::Fire()
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
