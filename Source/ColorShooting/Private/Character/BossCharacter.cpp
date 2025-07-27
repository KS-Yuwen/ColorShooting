#include "Character/BossCharacter.h"

ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// TODO: ボスキャラクターの初期設定（メッシュ、当たり判定など）
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TODO: ボス固有のフレーム毎の処理
}
