#include "Character/EnemySineMover.h"

AEnemySineMover::AEnemySineMover()
{
	PrimaryActorTick.bCanEverTick = true;
	RunningTime = 0.0f;
}

void AEnemySineMover::BeginPlay()
{
	Super::BeginPlay();

	InitialPosition = GetActorLocation();
	RunningTime = 0.0f;
}

void AEnemySineMover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	FVector NewLocation = InitialPosition;
	NewLocation.Y += RunningTime * MoveSpeedY;
	NewLocation.X += Amplitude * FMath::Sin(RunningTime * Frequency);

	SetActorLocation(NewLocation);
}
