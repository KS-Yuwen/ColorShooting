// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "ColorShootingGameMode.h"
#include "Bullet/Bullet.h"
#include "Bullet/GreenBullet.h"
#include "Character/EnemyCharacter.h"
#include "Subsystem/BulletPoolSubsystem.h"
#include "Subsystem/EnemyManagerSubsystem.h"
#include "InputActionValue.h"
#include "Subsystem/GameConstantManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<ABullet> PlayerBulletBPClass(TEXT("/Game/BluePrints/Bullet/Player/BP_PlayerBullet"));
	if (PlayerBulletBPClass.Succeeded())
	{
		M_PlayerBulletBP = PlayerBulletBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AGreenBullet> PlayerBulletGreenBPClass(TEXT("/Game/BluePrints/Bullet/Player/BP_PlayerBulletGreen"));
	if (PlayerBulletGreenBPClass.Succeeded())
	{
		M_PlayerBulletGreenBP = PlayerBulletGreenBPClass.Class;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RedBulletMaterialAsset(TEXT("/Game/Assets/Materials/M_BulletColorRed"));
	if (RedBulletMaterialAsset.Succeeded())
	{
		M_RedBulletMaterial = RedBulletMaterialAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BlueBulletMaterialAsset(TEXT("/Game/Assets/Materials/M_BulletColorBlue"));
	if (BlueBulletMaterialAsset.Succeeded())
	{
		M_BlueBulletMaterial = BlueBulletMaterialAsset.Object;
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UGameConstantManager *ConstantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
	if (ConstantManager != nullptr)
	{
		M_BombStock = ConstantManager->GetIntValue(FName("Player.InitialBombs"));
		M_RedShotLevel = ConstantManager->GetIntValue(FName("Player.InitialRedShotLevel"));
		M_GreenShotLevel = ConstantManager->GetIntValue(FName("Player.InitialGreenShotLevel"));
		M_BlueShotLevel = ConstantManager->GetIntValue(FName("Player.InitialBlueShotLevel"));
		M_MaxShotLevel = ConstantManager->GetIntValue(FName("Player.MaxShotLevel"));
		M_MaxBombStock = ConstantManager->GetIntValue(FName("Player.MaxBombs"));
	}

	// Get the player controller
	APlayerController *PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController == nullptr)
	{
		return;
	}

	// Get the Enhanced Input local player subsystem
	UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (Subsystem == nullptr)
	{
		return;
	}

	// Add the mapping context
	Subsystem->AddMappingContext(M_PlayerMappingContext, 0);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent *playerInputComponent)
{
	Super::SetupPlayerInputComponent(playerInputComponent);

	// Cast to Enhanced Input Component
	UEnhancedInputComponent *enhancedInputComponent = CastChecked<UEnhancedInputComponent>(playerInputComponent);
	if (enhancedInputComponent == nullptr)
	{
		return;
	}

	// Bind actions
	enhancedInputComponent->BindAction(M_MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	enhancedInputComponent->BindAction(M_LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	enhancedInputComponent->BindAction(M_FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Fire);
	enhancedInputComponent->BindAction(M_BombAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Bomb);
	enhancedInputComponent->BindAction(M_ChangeWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ChangeWeapon);
}

void APlayerCharacter::Move(const FInputActionValue &value)
{
	if (Controller == nullptr)
	{
		return;
	}
	const FVector2D movementVector = value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), movementVector.Y);
	AddMovementInput(GetActorRightVector(), movementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue &value)
{
	if (Controller == nullptr)
	{
		return;
	}
	const FVector2D lookAxisVector = value.Get<FVector2D>();
	AddControllerYawInput(lookAxisVector.X);
	AddControllerPitchInput(lookAxisVector.Y);
}

void APlayerCharacter::Fire(const FInputActionValue &value)
{
	switch (M_CurrentShotType)
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

void APlayerCharacter::FireRedShot()
{
	if (M_RedShotLevel <= 0 || M_Muzzle == nullptr || M_PlayerBulletBP == nullptr || M_RedBulletMaterial == nullptr)
	{
		return;
	}

	UBulletPoolSubsystem *BulletPoolSubsystem = GetGameInstance()->GetSubsystem<UBulletPoolSubsystem>();
	if (BulletPoolSubsystem == nullptr)
	{
		return;
	}

	UGameConstantManager* constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
	if (constantManager == nullptr)
	{
		return;
	}

	const FRotator SpawnRotation = M_Muzzle->GetComponentRotation();
	const FVector SpawnLocation = M_Muzzle->GetComponentLocation();

	if (M_RedShotLevel == 1)
	{
		// Level 1: Fire a single bullet
		if (ABullet *NewBullet = Cast<ABullet>(BulletPoolSubsystem->GetBulletFromPool(M_PlayerBulletBP, true)))
		{
			InitializeAndActivateBullet(NewBullet, SpawnLocation, SpawnRotation, M_RedBulletMaterial);
		}
	}
	else
	{
		// Level 2+: Fire multiple bullets spread out
		const int32 NumBullets = M_RedShotLevel;
		const float BulletSpacing = constantManager->GetFloatValue(FName("Player.RedShot.BulletSpacing"));
		const FVector RightVector = M_Muzzle->GetRightVector();
		const float TotalWidth = (NumBullets - 1) * BulletSpacing;
		const FVector StartLocation = SpawnLocation - (RightVector * (TotalWidth / 2.0f));

		for (int32 i = 0; i < NumBullets; ++i)
		{
			const FVector CurrentSpawnLocation = StartLocation + (RightVector * i * BulletSpacing);
			if (ABullet *NewBullet = Cast<ABullet>(BulletPoolSubsystem->GetBulletFromPool(M_PlayerBulletBP, true)))
			{
				InitializeAndActivateBullet(NewBullet, CurrentSpawnLocation, SpawnRotation, M_RedBulletMaterial);
			}
		}
	}
}

void APlayerCharacter::FireGreenShot()
{
	if (M_GreenShotLevel <= 0 || M_Muzzle == nullptr || M_PlayerBulletGreenBP == nullptr)
	{
		return;
	}

	// Check fire rate cooldown
	float FireRate = 0.5f; // Default for levels 1 & 2
	if (M_GreenShotLevel >= 5)
	{
		FireRate = 0.2f;
	}
	else if (M_GreenShotLevel >= 3)
	{
		FireRate = 0.3f;
	}

	const double CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime < M_LastGreenShotTime + FireRate)
	{
		return;
	}
	M_LastGreenShotTime = CurrentTime;

	UBulletPoolSubsystem *BulletPoolSubsystem = GetGameInstance()->GetSubsystem<UBulletPoolSubsystem>();
	if (BulletPoolSubsystem == nullptr)
	{
		return;
	}

	UEnemyManagerSubsystem* EnemyManager = GetGameInstance()->GetSubsystem<UEnemyManagerSubsystem>();
	if (EnemyManager == nullptr)
	{
		return;
	}

	// Find the closest enemy
	AActor *ClosestEnemy = EnemyManager->GetClosestEnemy(GetActorLocation());

	FRotator SpawnRotation = ClosestEnemy ? (ClosestEnemy->GetActorLocation() - GetActorLocation()).Rotation() : GetControlRotation();

	const FVector SpawnLocation = M_Muzzle->GetComponentLocation();

	// Determine number of bullets based on level
	int32 NumBullets = 1;
	if (M_GreenShotLevel >= 4)
	{
		NumBullets = 3;
	}
	else if (M_GreenShotLevel >= 2)
	{
		NumBullets = 2;
	}

	for (int32 i = 0; i < NumBullets; ++i)
	{
		if (AGreenBullet *NewBullet = Cast<AGreenBullet>(BulletPoolSubsystem->GetBulletFromPool(M_PlayerBulletGreenBP, true)))
		{
			InitializeAndActivateBullet(NewBullet, SpawnLocation, SpawnRotation);
			if (ClosestEnemy)
			{
				NewBullet->SetTarget(ClosestEnemy);
			}
		}
	}
}

void APlayerCharacter::FireBlueShot()
{
	if (M_BlueShotLevel <= 0 || M_Muzzle == nullptr || M_PlayerBulletBP == nullptr || M_BlueBulletMaterial == nullptr)
	{
		return;
	}

	UBulletPoolSubsystem *BulletPoolSubsystem = GetGameInstance()->GetSubsystem<UBulletPoolSubsystem>();
	if (BulletPoolSubsystem == nullptr)
	{
		return;
	}

	UGameConstantManager* ConstantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
	if (ConstantManager == nullptr)
	{
		return;
	}

	const FVector SpawnLocation = M_Muzzle->GetComponentLocation();
	const FRotator BaseRotation = M_Muzzle->GetComponentRotation();
	const int32 NumBullets = (M_BlueShotLevel * 2) - 1;

	if (NumBullets <= 1)
	{
		// Level 1: Fire a single bullet forward
		if (ABullet *NewBullet = Cast<ABullet>(BulletPoolSubsystem->GetBulletFromPool(M_PlayerBulletBP, true)))
		{
			InitializeAndActivateBullet(NewBullet, SpawnLocation, BaseRotation, M_BlueBulletMaterial, EShotType::Blue);
		}
	}
	else
	{
		// Level 2+: Fire a spread of bullets
		const float TotalAngle = ConstantManager->GetFloatValue(FName("Player.BlueShot.TotalAngle"));
		const float AngleStep = TotalAngle / (NumBullets - 1);
		const float StartAngle = -TotalAngle / 2.0f;

		for (int32 i = 0; i < NumBullets; ++i)
		{
			const float CurrentAngle = StartAngle + (i * AngleStep);
			const FRotator SpawnRotation = BaseRotation + FRotator(0.0f, CurrentAngle, 0.0f);
			if (ABullet *NewBullet = Cast<ABullet>(BulletPoolSubsystem->GetBulletFromPool(M_PlayerBulletBP, true)))
			{
				InitializeAndActivateBullet(NewBullet, SpawnLocation, SpawnRotation, M_BlueBulletMaterial, EShotType::Blue);
			}
		}
	}
}

void APlayerCharacter::Bomb(const FInputActionValue &value)
{
	if (M_BombStock <= 0)
	{
		return;
	}

	M_BombStock--;
	UE_LOG(LogTemp, Log, TEXT("Fired Bomb!"));
}

void APlayerCharacter::ChangeWeapon(const FInputActionValue &value)
{
	UE_LOG(LogTemp, Log, TEXT("Changed Weapon!"));
	switch (M_CurrentShotType)
	{
	case EShotType::Red:
		M_CurrentShotType = EShotType::Blue;
		break;
	case EShotType::Blue:
		M_CurrentShotType = EShotType::Green;
		break;
	case EShotType::Green:
		M_CurrentShotType = EShotType::Red;
		break;
	}
}

void APlayerCharacter::AddBomb()
{
	if (M_BombStock >= M_MaxBombStock)
	{
		AColorShootingGameMode *gameMode = Cast<AColorShootingGameMode>(UGameplayStatics::GetGameMode(this));
		if (gameMode == nullptr)
		{
			return;
		}

		UGameConstantManager *constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
		if (constantManager != nullptr)
		{
			gameMode->AddScore(constantManager->GetIntValue(FName("Score.ItemGet")));
		}
		return;
	}
	M_BombStock++;
}

int32 APlayerCharacter::GetBombStock() const
{
	return M_BombStock;
}

void APlayerCharacter::AddShotLevel(const EShotType shotType)
{
	AColorShootingGameMode *gameMode = Cast<AColorShootingGameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode == nullptr)
	{
		return;
	}

	int32* shotLevel = nullptr;
	switch (shotType)
	{
	case EShotType::Red:
		shotLevel = &M_RedShotLevel;
		break;
	case EShotType::Green:
		shotLevel = &M_GreenShotLevel;
		break;
	case EShotType::Blue:
		shotLevel = &M_BlueShotLevel;
		break;
	}

	if (shotLevel == nullptr)
	{
		return;
	}

	if (*shotLevel < M_MaxShotLevel)
	{
		(*shotLevel)++;
	}
	else
	{
		UGameConstantManager *constantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
		if (constantManager != nullptr)
		{
			gameMode->AddScore(constantManager->GetIntValue(FName("Score.ItemGet")));
		}
	}
}

void APlayerCharacter::InitializeAndActivateBullet(ABullet* newBullet, const FVector& spawnLocation, const FRotator& spawnRotation, UMaterialInterface* bulletMaterial, EShotType shotType)
{
	if (newBullet == nullptr)
	{
		return;
	}

	newBullet->SetActorLocationAndRotation(spawnLocation, spawnRotation);
	if (bulletMaterial != nullptr)
	{
		newBullet->M_BulletMeshComponent->SetMaterial(0, bulletMaterial);
	}
	if (shotType != EShotType::Max)
	{
		newBullet->M_ShotType = shotType;
	}
	newBullet->SetActive(true);
	newBullet->SetDirection(spawnRotation.Vector());
}
