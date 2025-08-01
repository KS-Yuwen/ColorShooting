#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Common/ShotType.h"
#include "EnemyCharacter.generated.h"

// 敵キャラクタークラス
UCLASS()
class COLORSHOOTING_API AEnemyCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	// Color type of the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	EShotType M_ColorType = EShotType::Red;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	EShotType GetColorType() const { return M_ColorType; }

protected:
	virtual void OnDeath();

private:
	bool M_bKilledByReflectedBullet = false;
};
