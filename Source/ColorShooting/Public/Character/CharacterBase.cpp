#include "CharacterBase.h"
#include "Components/SceneComponent.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(GetRootComponent());
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 機能を入力にバインドするために呼び出されます
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// ダメージを受ける処理
float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 親クラスのTakeDamageを呼び出し、実際のダメージ量を取得
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 体力を減算
	Health -= ActualDamage;
	// 体力が0以下になった場合
	if (Health <= 0.0f)
	{
		// 死亡処理を呼び出す
		OnDeath();
	}

	// 実際のダメージ量を返す
	return ActualDamage;
}

// 死亡時の処理
void ACharacterBase::OnDeath()
{
	// ここに死亡ロジックを実装します
	// アクターを破棄
	Destroy();
}
