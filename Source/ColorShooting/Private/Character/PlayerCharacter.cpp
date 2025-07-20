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

	UGameConstantManager* ConstantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
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
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController == nullptr)
	{
		return;
	}

	// Get the Enhanced Input local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
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

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast to Enhanced Input Component
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent == nullptr)
	{
		return;
	}

	// Bind actions
	EnhancedInputComponent->BindAction(M_MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	EnhancedInputComponent->BindAction(M_LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	EnhancedInputComponent->BindAction(M_FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Fire);
	EnhancedInputComponent->BindAction(M_BombAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Bomb);
	EnhancedInputComponent->BindAction(M_ChangeWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ChangeWeapon);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (Controller == nullptr)
	{
		return;
	}
	const FVector2D MovementVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (Controller == nullptr)
	{
		return;
	}
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void APlayerCharacter::Fire(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Fire!"));

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

	UBulletPoolSubsystem* BulletPoolSubsystem = GetGameInstance()->GetSubsystem<UBulletPoolSubsystem>();
	if (BulletPoolSubsystem == nullptr)
	{
		return;
	}

	const FRotator SpawnRotation = M_Muzzle->GetComponentRotation();
	const FVector SpawnLocation = M_Muzzle->GetComponentLocation();

	if (M_RedShotLevel == 1)
	{
		// Level 1: Fire a single bullet
		if (ABullet* NewBullet = Cast<ABullet>(BulletPoolSubsystem->GetBulletFromPool(M_PlayerBulletBP)))
		{
			NewBullet->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
			NewBullet->M_BulletMeshComponent->SetMaterial(0, M_RedBulletMaterial);
			NewBullet->SetActive(true);
		}
	}
	else
	{
		// Level 2+: Fire multiple bullets spread out
		const int32 NumBullets = M_RedShotLevel;
		constexpr float BulletSpacing = 15.0f;
		const FVector RightVector = M_Muzzle->GetRightVector();
		const float TotalWidth = (NumBullets - 1) * BulletSpacing;
		const FVector StartLocation = SpawnLocation - (RightVector * (TotalWidth / 2.0f));

		for (int32 i = 0; i < NumBullets; ++i)
		{
			const FVector CurrentSpawnLocation = StartLocation + (RightVector * i * BulletSpacing);
			if (ABullet* NewBullet = Cast<ABullet>(BulletPoolSubsystem->GetBulletFromPool(M_PlayerBulletBP)))
			{
				NewBullet->SetActorLocationAndRotation(CurrentSpawnLocation, SpawnRotation);
				NewBullet->M_BulletMeshComponent->SetMaterial(0, M_RedBulletMaterial);
				NewBullet->SetActive(true);
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

	UBulletPoolSubsystem* BulletPoolSubsystem = GetGameInstance()->GetSubsystem<UBulletPoolSubsystem>();
	if (BulletPoolSubsystem == nullptr)
	{
		return;
	}

	// Find the closest enemy
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
		if (AGreenBullet* NewBullet = Cast<AGreenBullet>(BulletPoolSubsystem->GetBulletFromPool(M_PlayerBulletGreenBP)))
		{
			NewBullet->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
			NewBullet->SetTarget(ClosestEnemy);
			NewBullet->SetActive(true);
		}
	}
}

void APlayerCharacter::FireBlueShot()
{
	if (M_BlueShotLevel <= 0 || M_Muzzle == nullptr || M_PlayerBulletBP == nullptr || M_BlueBulletMaterial == nullptr)
	{
		return;
	}

	UBulletPoolSubsystem* BulletPoolSubsystem = GetGameInstance()->GetSubsystem<UBulletPoolSubsystem>();
	if (BulletPoolSubsystem == nullptr)
	{
		return;
	}

	const FVector SpawnLocation = M_Muzzle->GetComponentLocation();
	const FRotator BaseRotation = M_Muzzle->GetComponentRotation();
	const int32 NumBullets = (M_BlueShotLevel * 2) - 1;

	if (NumBullets <= 1)
	{
		// Level 1: Fire a single bullet forward
		if (ABullet* NewBullet = Cast<ABullet>(BulletPoolSubsystem->GetBulletFromPool(M_PlayerBulletBP)))
		{
			NewBullet->SetActorLocationAndRotation(SpawnLocation, BaseRotation);
			NewBullet->M_BulletMeshComponent->SetMaterial(0, M_BlueBulletMaterial);
			NewBullet->M_ShotType = EShotType::Blue;
			NewBullet->SetActive(true);
		}
	}
	else
	{
		// Level 2+: Fire a spread of bullets
		constexpr float TotalAngle = 60.0f;
		const float AngleStep = TotalAngle / (NumBullets - 1);
		const float StartAngle = -TotalAngle / 2.0f;

		for (int32 i = 0; i < NumBullets; ++i)
		{
			const float CurrentAngle = StartAngle + (i * AngleStep);
			const FRotator SpawnRotation = BaseRotation + FRotator(0.0f, CurrentAngle, 0.0f);
			if (ABullet* NewBullet = Cast<ABullet>(BulletPoolSubsystem->GetBulletFromPool(M_PlayerBulletBP)))
			{
				NewBullet->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
				NewBullet->M_BulletMeshComponent->SetMaterial(0, M_BlueBulletMaterial);
				NewBullet->M_ShotType = EShotType::Blue;
				NewBullet->SetActive(true);
			}
		}
	}
}

void APlayerCharacter::Bomb(const FInputActionValue& Value)
{
	if (M_BombStock <= 0)
	{
		return;
	}

	M_BombStock--;
	UE_LOG(LogTemp, Log, TEXT("Fired Bomb!"));
}

void APlayerCharacter::ChangeWeapon(const FInputActionValue& Value)
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
	if (M_BombStock < M_MaxBombStock)
	{
		M_BombStock++;
	}
	else
	{
		AColorShootingGameMode* GameMode = Cast<AColorShootingGameMode>(UGameplayStatics::GetGameMode(this));
		if (GameMode == nullptr)
		{
			return;
		}

		UGameConstantManager* ConstantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
		if (ConstantManager != nullptr)
		{
			GameMode->AddScore(ConstantManager->GetIntValue(FName("Score.ItemGet")));
		}
	}
}

void APlayerCharacter::AddShotLevel(const EShotType ShotType)
{
	AColorShootingGameMode* GameMode = Cast<AColorShootingGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode == nullptr)
	{
		return;
	}

	switch (ShotType)
	{
	case EShotType::Red:
		if (M_RedShotLevel < M_MaxShotLevel)
		{
			M_RedShotLevel++;
		}
		else
		{
			UGameConstantManager* ConstantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
			if (ConstantManager != nullptr)
			{
				GameMode->AddScore(ConstantManager->GetIntValue(FName("Score.ItemGet")));
			}
		}
		break;
	case EShotType::Green:
		if (M_GreenShotLevel < M_MaxShotLevel)
		{
			M_GreenShotLevel++;
		}
		else
		{
			UGameConstantManager* ConstantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
			if (ConstantManager != nullptr)
			{
				GameMode->AddScore(ConstantManager->GetIntValue(FName("Score.ItemGet")));
			}
		}
		break;
	case EShotType::Blue:
		if (M_BlueShotLevel < M_MaxShotLevel)
		{
			M_BlueShotLevel++;
		}
		else
		{
			UGameConstantManager* ConstantManager = GetGameInstance()->GetSubsystem<UGameConstantManager>();
			if (ConstantManager != nullptr)
			{
				GameMode->AddScore(ConstantManager->GetIntValue(FName("Score.ItemGet")));
			}
		}
		break;
	}
}

