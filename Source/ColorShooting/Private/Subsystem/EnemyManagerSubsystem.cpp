// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystem/EnemyManagerSubsystem.h"
#include "Character/EnemyCharacter.h"

void UEnemyManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("EnemyManagerSubsystem Initialized"));
}

void UEnemyManagerSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Log, TEXT("EnemyManagerSubsystem Deinitialized"));
	M_ActiveEnemies.Empty();
	Super::Deinitialize();
}

void UEnemyManagerSubsystem::RegisterEnemy(AEnemyCharacter* Enemy)
{
	if (Enemy && !M_ActiveEnemies.Contains(Enemy))
	{
		M_ActiveEnemies.Add(Enemy);
		UE_LOG(LogTemp, Log, TEXT("Enemy registered: %s"), *Enemy->GetName());
	}
}

void UEnemyManagerSubsystem::UnregisterEnemy(AEnemyCharacter* Enemy)
{
	if (Enemy)
	{
		M_ActiveEnemies.Remove(Enemy);
		UE_LOG(LogTemp, Log, TEXT("Enemy unregistered: %s"), *Enemy->GetName());
	}
}

AEnemyCharacter* UEnemyManagerSubsystem::GetClosestEnemy(const FVector& Location) const
{
	AEnemyCharacter* ClosestEnemy = nullptr;
	float MinDistanceSq = TNumericLimits<float>::Max();

	for (AEnemyCharacter* Enemy : M_ActiveEnemies)
	{
		if (IsValid(Enemy) && !Enemy->IsPendingKillPending())
		{
			const float DistanceSq = FVector::DistSquared(Location, Enemy->GetActorLocation());
			if (DistanceSq < MinDistanceSq)
			{
				MinDistanceSq = DistanceSq;
				ClosestEnemy = Enemy;
			}
		}
	}

	return ClosestEnemy;
}
