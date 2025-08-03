// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystem/EnemyManagerSubsystem.h"
#include "Character/EnemyCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UEnemyManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("EnemyManagerSubsystem Initialized"));
#endif
}

void UEnemyManagerSubsystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SpawnLoopTimerHandle);
    }
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

void UEnemyManagerSubsystem::StartStage(UDataTable* StageDataTable)
{
    if (!StageDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("UEnemyManagerSubsystem::StartStage: StageDataTable is null."));
        return;
    }

    CurrentStageData = StageDataTable;
    StageTime = 0.0f;
    NextSpawnIndex = 0;
    SpawnInfos.Empty();

    FString ContextString(TEXT("UEnemyManagerSubsystem::StartStage"));
    TArray<FName> RowNames = CurrentStageData->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        FEnemySpawnInfo* Row = CurrentStageData->FindRow<FEnemySpawnInfo>(RowName, ContextString);
        if (Row)
        {
            SpawnInfos.Add(*Row);
        }
    }

    SpawnInfos.Sort([](const FEnemySpawnInfo& A, const FEnemySpawnInfo& B) {
        return A.SpawnTime < B.SpawnTime;
    });

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(SpawnLoopTimerHandle, this, &UEnemyManagerSubsystem::SpawnLoop, 0.1f, true, 0.0f);
        UE_LOG(LogTemp, Log, TEXT("Stage started with %d enemies to spawn."), SpawnInfos.Num());
    }
}

void UEnemyManagerSubsystem::SpawnLoop()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    if (NextSpawnIndex >= SpawnInfos.Num())
    {
        World->GetTimerManager().ClearTimer(SpawnLoopTimerHandle);
        UE_LOG(LogTemp, Log, TEXT("All enemies have been spawned for this stage."));
        // Broadcast a delegate here if other systems need to know the stage is clear.
        return;
    }

    StageTime += 0.1f; // Increment stage time by the timer interval

    while (NextSpawnIndex < SpawnInfos.Num() && StageTime >= SpawnInfos[NextSpawnIndex].SpawnTime)
    {
        const FEnemySpawnInfo& Info = SpawnInfos[NextSpawnIndex];
        if (Info.EnemyClass)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = nullptr;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            ACharacter* SpawnedEnemy = World->SpawnActor<ACharacter>(Info.EnemyClass, Info.SpawnLocation, FRotator::ZeroRotator, SpawnParams);
            if(AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(SpawnedEnemy))
            {
                RegisterEnemy(EnemyCharacter);
            }
            
            UE_LOG(LogTemp, Log, TEXT("Spawning enemy class %s at time %f."), *Info.EnemyClass->GetName(), StageTime);
        }
        NextSpawnIndex++;
    }
}
