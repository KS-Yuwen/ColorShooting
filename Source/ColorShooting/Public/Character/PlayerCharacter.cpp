#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "ColorShootingGameMode.h"

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
	// TODO: 発射ロジックを実装
}

// ボム処理
void APlayerCharacter::Bomb(const FInputActionValue& Value)
{
	// TODO: ボムロジックを実装
}

// 武器変更処理
void APlayerCharacter::ChangeWeapon(const FInputActionValue& Value)
{
	// TODO: 武器変更ロジックを実装
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
