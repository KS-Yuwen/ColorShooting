#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlowingField.generated.h"

UCLASS()
class COLORSHOOTING_API ASlowingField : public AActor
{
	GENERATED_BODY()
	
public:	
	ASlowingField();

	/** Initializes the field's properties */
	void InitializeField(float Radius, float Duration, float SlowFactor);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> M_SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> M_MeshComponent;

	/** Default mesh for visualizing the field's area. Set in Blueprint defaults. */
	UPROPERTY(EditDefaultsOnly, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMesh> M_DefaultMesh;

	/** Default material for visualizing the field's area. Set in Blueprint defaults. */
	UPROPERTY(EditDefaultsOnly, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMaterialInterface> M_DefaultMaterial;

	/** The factor by which to slow down actors (e.g., 0.5 for 50% speed) */
	float M_SlowFactor = 1.0f;

	/** Set of actors currently being slowed by this field */
	UPROPERTY()
	TSet<TObjectPtr<AActor>> M_AffectedActors;
};
