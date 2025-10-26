#include "Effect/SlowingField.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/EnemyCharacter.h"
#include "Bullet/Bullet.h"
#include "UObject/ConstructorHelpers.h"

ASlowingField::ASlowingField()
{
	PrimaryActorTick.bCanEverTick = false;

	M_SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = M_SphereComponent;
	M_SphereComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	M_MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	M_MeshComponent->SetupAttachment(RootComponent);
	M_MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Set default mesh and material from properties set in Blueprint
	if (M_DefaultMesh)
	{
		M_MeshComponent->SetStaticMesh(M_DefaultMesh);
	}
	if (M_DefaultMaterial)
	{
		M_MeshComponent->SetMaterial(0, M_DefaultMaterial);
	}
}

void ASlowingField::BeginPlay()
{
	Super::BeginPlay();
	M_SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASlowingField::OnOverlapBegin);
	M_SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ASlowingField::OnOverlapEnd);
}

void ASlowingField::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Reset time dilation for all affected actors when the field is destroyed
	for (TObjectPtr<AActor> actor : M_AffectedActors)
	{
		if (IsValid(actor.Get()))
		{
			actor->CustomTimeDilation = 1.0f;
		}
	}
	M_AffectedActors.Empty();

	Super::EndPlay(EndPlayReason);
}


void ASlowingField::InitializeField(float Radius, float Duration, float SlowFactor)
{
	M_SphereComponent->SetSphereRadius(Radius);
	M_MeshComponent->SetRelativeScale3D(FVector(Radius / 50.0f)); // Adjust mesh size to match sphere radius (default sphere radius is 50)
	M_SlowFactor = SlowFactor;
	SetLifeSpan(Duration);
}

void ASlowingField::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || OtherActor == this)
	{
		return;
	}

	// Check if the actor is an enemy or an enemy bullet
	AEnemyCharacter* enemyCharacter = Cast<AEnemyCharacter>(OtherActor);
	ABullet* bullet = Cast<ABullet>(OtherActor);

	bool isTarget = (enemyCharacter != nullptr) || (bullet != nullptr && !bullet->M_bIsPlayerBullet);

	if (isTarget && !M_AffectedActors.Contains(OtherActor))
	{
		OtherActor->CustomTimeDilation = M_SlowFactor;
		M_AffectedActors.Add(OtherActor);
	}
}

void ASlowingField::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr)
	{
		return;
	}

	if (M_AffectedActors.Contains(OtherActor))
	{
		// Actor is valid if it's in the set and not null
		OtherActor->CustomTimeDilation = 1.0f;
		M_AffectedActors.Remove(OtherActor);
	}
}
