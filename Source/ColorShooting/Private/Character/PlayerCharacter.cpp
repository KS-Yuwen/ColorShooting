#include "Character/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "ColorShootingGameMode.h"
#include "Bullet/Bullet.h"
#include "Bullet/GreenBullet.h"
#include "Character/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// プレイヤーコントローラーを取得
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// Enhanced Inputのローカルプレイヤーサブシステムを取得
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// マッピングコンテキストを追加
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced Inputコンポーネントにキャスト
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// アクションをバインド
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Fire);
		EnhancedInputComponent->BindAction(BombAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Bomb);
		EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ChangeWeapon);
	}
}

// 移動処理
void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// 入力値を取得
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 前後左右に移動
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

// 見回す処理
void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// 入力値を取得
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 左右の回転
		AddControllerYawInput(LookAxisVector.X);
		// 上下の回転
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

// 発射処理
void APlayerCharacter::Fire(const FInputActionValue& Value)
{
	switch (CurrentShotType)
	{
	case EShotType::Red:
		FireRedShot();
		break;
	case EShotType::Green:
		FireGreenShot();
		break;
	case EShotType::Blue:
		FireBlueShot();
		break;
	}
}

// 赤ショットを発射
void APlayerCharacter::FireRedShot()
{
	if (RedShotLevel <= 0 || !Muzzle)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	const FRotator SpawnRotation = Muzzle->GetComponentRotation();
	const FVector SpawnLocation = Muzzle->GetComponentLocation();

	// Bulletの半径が5.0fなので、重ならないように15.0fの間隔を空ける
	const float BulletSpacing = 15.0f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	if (RedShotLevel == 1)
	{
		// レベル1の時は1発だけ発射
		ABullet* NewBullet = World->SpawnActor<ABullet>(SpawnLocation, SpawnRotation, SpawnParams);
		if (NewBullet)
		{
			NewBullet->bIsPlayerBullet = true;
		}
	}
	else
	{
		// レベル2以上は左右均等に発射
		const int32 NumBullets = RedShotLevel;
		const FVector RightVector = Muzzle->GetRightVector();
		const float TotalWidth = (NumBullets - 1) * BulletSpacing;
		const FVector StartLocation = SpawnLocation - (RightVector * (TotalWidth / 2.0f));

		for (int32 i = 0; i < NumBullets; ++i)
		{
			const FVector CurrentSpawnLocation = StartLocation + (RightVector * i * BulletSpacing);
			ABullet* NewBullet = World->SpawnActor<ABullet>(CurrentSpawnLocation, SpawnRotation, SpawnParams);
			if (NewBullet)
			{
				NewBullet->bIsPlayerBullet = true;
			}
		}
	}
}

// 緑ショットを発射
void APlayerCharacter::FireGreenShot()
{
	if (GreenShotLevel <= 0 || !Muzzle)
	{
		return;
	}

	// レベルに応じたクールダウンを設定
	float FireRate = 0.0f;
	switch (GreenShotLevel)
	{
	case 1:
	case 2:
		FireRate = 0.5f;
		break;
	case 3:
	case 4:
		FireRate = 0.3f;
		break;
	case 5:
		FireRate = 0.2f;
		break;
	}

	// クールダウンを確認
	const double CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime < LastGreenShotTime + FireRate)
	{
		return;
	}
	LastGreenShotTime = CurrentTime;

	// 最も近い敵を探す
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), FoundEnemies);

	AActor* ClosestEnemy = nullptr;
	float MinDistance = TNumericLimits<float>::Max();

	for (AActor* Enemy : FoundEnemies)
	{
		const float Distance = GetDistanceTo(Enemy);
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestEnemy = Enemy;
		}
	}

	// 敵がいない場合は正面に発射
	FRotator SpawnRotation = Muzzle->GetComponentRotation();
	if (!ClosestEnemy)
	{
		SpawnRotation = GetControlRotation();
	}

	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector SpawnLocation = Muzzle->GetComponentLocation();

	// レベルに応じた弾数を設定
	int32 NumBullets = 0;
	switch (GreenShotLevel)
	{
	case 1:
		NumBullets = 1;
		break;
	case 2:
	case 3:
		NumBullets = 2;
		break;
	case 4:
	case 5:
		NumBullets = 3;
		break;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	for (int32 i = 0; i < NumBullets; ++i)
	{
		AGreenBullet* NewBullet = World->SpawnActor<AGreenBullet>(SpawnLocation, SpawnRotation, SpawnParams);
		if (NewBullet)
		{
			NewBullet->bIsPlayerBullet = true;
			if (ClosestEnemy)
			{
				NewBullet->SetTarget(ClosestEnemy);
			}
		}
	}
}

// 青ショットを発射
void APlayerCharacter::FireBlueShot()
{
	if (BlueShotLevel <= 0 || !Muzzle)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector SpawnLocation = Muzzle->GetComponentLocation();
	const FRotator BaseRotation = Muzzle->GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// BlueShotLevelに応じて発射する弾の数を決定 (1, 3, 5, 7, 9)
	const int32 NumBullets = (BlueShotLevel * 2) - 1;

	if (NumBullets == 1)
	{
		// レベル1の時は正面に1発だけ発射
		ABullet* NewBullet = World->SpawnActor<ABullet>(SpawnLocation, BaseRotation, SpawnParams);
		if (NewBullet)
		{
			NewBullet->bIsPlayerBullet = true;
		}
	}
	else
	{
		// レベル2以上はWAYショット
		const float TotalAngle = 60.0f;
		const float AngleStep = TotalAngle / (NumBullets - 1);
		const float StartAngle = -TotalAngle / 2.0f;

		for (int32 i = 0; i < NumBullets; ++i)
		{
			const float CurrentAngle = StartAngle + (i * AngleStep);
			const FRotator SpawnRotation = BaseRotation + FRotator(0.0f, CurrentAngle, 0.0f);
			ABullet* NewBullet = World->SpawnActor<ABullet>(SpawnLocation, SpawnRotation, SpawnParams);
			if (NewBullet)
			{
				NewBullet->bIsPlayerBullet = true;
			}
		}
	}
}

// ボム処理
void APlayerCharacter::Bomb(const FInputActionValue& Value)
{
	if (BombStock <= 0)
	{
		return;
	}

	BombStock--;
	UE_LOG(LogTemp, Log, TEXT("Fire Bomb!"));
}

// 武器変更処理
void APlayerCharacter::ChangeWeapon(const FInputActionValue& Value)
{
	switch (CurrentShotType)
	{
	case EShotType::Red:
		CurrentShotType = EShotType::Blue;
		break;
	case EShotType::Blue:
		CurrentShotType = EShotType::Green;
		break;
	case EShotType::Green:
		CurrentShotType = EShotType::Red;
		break;
	}
}

/** ボムを1つ追加します */
void APlayerCharacter::AddBomb()
{
	BombStock++;
}

/**
 * ショットレベルを加算します
 * @param ShotType ショットタイプ
 */
void APlayerCharacter::AddShotLevel(EShotType ShotType)
{
	AColorShootingGameMode* GameMode = Cast<AColorShootingGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode)
	{
		return;
	}

	switch (ShotType)
	{
	case EShotType::Red:
		if (RedShotLevel < 5)
		{
			RedShotLevel++;
		}
		else
		{
			GameMode->AddScore(100);
		}
		break;
	case EShotType::Green:
		if (GreenShotLevel < 5)
		{
			GreenShotLevel++;
		}
		else
		{
			GameMode->AddScore(100);
		}
		break;
	case EShotType::Blue:
		if (BlueShotLevel < 5)
		{
			BlueShotLevel++;
		}
		else
		{
			GameMode->AddScore(100);
		}
		break;
	}
}
