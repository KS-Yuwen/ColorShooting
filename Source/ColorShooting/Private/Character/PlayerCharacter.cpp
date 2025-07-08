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

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

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
	if (M_RedShotLevel <= 0 || M_Muzzle == nullptr)
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
		if (ABullet* NewBullet = Cast<ABullet>(BulletPoolSubsystem->GetBulletFromPool()))
		{
			NewBullet->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
			NewBullet->bIsPlayerBullet = true;
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
			if (ABullet* NewBullet = Cast<ABullet>(BulletPoolSubsystem->GetBulletFromPool()))
			{
				NewBullet->SetActorLocationAndRotation(CurrentSpawnLocation, SpawnRotation);
				NewBullet->bIsPlayerBullet = true;
				NewBullet->SetActive(true);
			}
		}
	}
}

void APlayerCharacter::FireGreenShot()
{
	if (M_GreenShotLevel <= 0 || M_Muzzle == nullptr)
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
		if (AGreenBullet* NewBullet = Cast<AGreenBullet>(BulletPoolSubsystem->GetBulletFromPool()))
		{
			NewBullet->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
			NewBullet->bIsPlayerBullet = true;
			if (ClosestEnemy)
			{
				NewBullet->SetTarget(ClosestEnemy);
			}
			NewBullet->SetActive(true);
		}
	}
}

void APlayerCharacter::FireBlueShot()
{
	if (M_BlueShotLevel <= 0 || M_Muzzle == nullptr)
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
		if (ABullet* NewBullet = Cast<ABullet>(BulletPoolSubsystem->GetBulletFromPool()))
		{
			NewBullet->SetActorLocationAndRotation(SpawnLocation, BaseRotation);
			NewBullet->bIsPlayerBullet = true;
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
			if (ABullet* NewBullet = Cast<ABullet>(BulletPoolSubsystem->GetBulletFromPool()))
			{
				NewBullet->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
				NewBullet->bIsPlayerBullet = true;
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
	M_BombStock++;
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
		if (M_RedShotLevel < 5)
		{
			M_RedShotLevel++;
		}
		else
		{
			GameMode->AddScore(100);
		}
		break;
	case EShotType::Green:
		if (M_GreenShotLevel < 5)
		{
			M_GreenShotLevel++;
		}
		else
		{
			GameMode->AddScore(100);
		}
		break;
	case EShotType::Blue:
		if (M_BlueShotLevel < 5)
		{
			M_BlueShotLevel++;
		}
		else
		{
			GameMode->AddScore(100);
		}
		break;
	}
}

