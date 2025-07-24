// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystem/EnemyManagerSubsystem.h"
#include "Character/EnemyCharacter.h"

void UEnemyManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("EnemyManagerSubsystem Initialized"));
#endif
}

void UEnemyManagerSubsystem::Deinitialize()
{
	#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("EnemyManagerSubsystem Deinitialized"));
#endif
	M_ActiveEnemies.Empty();
	Super::Deinitialize();
}

void UEnemyManagerSubsystem::RegisterEnemy(AEnemyCharacter* Enemy)
{
	if (Enemy && !M_ActiveEnemies.Contains(Enemy))
	{
		M_ActiveEnemies.Add(Enemy);
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("Enemy registered: %s"), *Enemy->GetName());
#endif
	}
}

void UEnemyManagerSubsystem::UnregisterEnemy(AEnemyCharacter* Enemy)
{
	if (Enemy)
	{
		M_ActiveEnemies.Remove(Enemy);
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("Enemy unregistered: %s"), *Enemy->GetName());
#endif
	}
}

AEnemyCharacter* UEnemyManagerSubsystem::GetClosestEnemy(const FVector& Location)
{
	AEnemyCharacter* ClosestEnemy = nullptr;
	float MinDistanceSq = TNumericLimits<float>::Max();

	// Use a reverse loop to safely remove invalid enemies during iteration
	for (int32 i = M_ActiveEnemies.Num() - 1; i >= 0; --i)
	{
		AEnemyCharacter* Enemy = M_ActiveEnemies[i];
		if (IsValid(Enemy) && !Enemy->IsPendingKillPending())
		{
			const float DistanceSq = FVector::DistSquared(Location, Enemy->GetActorLocation());
			if (DistanceSq < MinDistanceSq)
			{
				MinDistanceSq = DistanceSq;
				ClosestEnemy = Enemy;
			}
		}
		else
		{
			// Remove invalid enemy from the list
			M_ActiveEnemies.RemoveAt(i);
		}
	}

	return ClosestEnemy;
}
