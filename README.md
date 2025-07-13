# ColorShooting
ColorShooting プロジェクト概要
1. プロジェクトの目的と概要
ColorShootingは、Unreal Engine 5で開発された3Dシューティングゲームです。プレイヤーはキャラクターを操作し、3種類（赤・緑・青）の弾を使い分けて敵を倒していきます。

このプロジェクトの主な特徴は、色をテーマにした戦略的なゲームプレイです。
プレイヤーが撃つ弾と敵キャラクターにはそれぞれ色が設定されており、同じ色の弾を敵に当てることで弾を反射させるなど、特殊なアクションが発生します。

また、ワールドに配置されたアイテムを収集することで、プレイヤーのショットを強化したり、特殊なボムを使用したりと、ローグライクな成長要素も取り入れています。
パフォーマンス向上のため、弾の発射にはオブジェクトプーリングパターンが採用されています。

2. 主要な機能一覧
プレイヤーキャラクター (APlayerCharacter)

移動およびマウスによる視点操作
3種類のショット（赤、緑、青）の発射と切り替え
ボムの使用
アイテム取得による能力強化（ショットレベル、ボム所持数）
弾システム (ABullet, AGreenBullet)

通常弾 (赤・青): 直進し、壁で跳ね返る。敵に当たると色に応じて消滅または反射する。
ホーミング弾 (緑): 最も近い敵を自動的に追尾する。
弾の反射: プレイヤーの弾が同じ色の敵に当たると、弾が反射して再利用される。
オブジェクトプーリング: UBulletPoolSubsystemにより、弾を再利用してパフォーマンスを最適化。
アイテムシステム (AItemBase)

プレイヤーが接触することで収集される。
ボムアイテム (ABombItem): ボムの所持数を増やす。
ショットレベルアップアイテム (AShotLevelUpItem): 各色のショットのレベルを上げ、攻撃を強化する。
敵キャラクター (AEnemyCharacter)

固有の色を持つ。
プレイヤーの弾とのインタラクション（被ダメージ、弾の反射）。
ゲーム進行管理

スコアシステム: AColorShootingGameModeとAColorShootingGameStateでスコアを管理。
入力システム: Unreal EngineのEnhanced Input Systemを利用したモダンな入力管理。

3. 使用されている主な技術スタック
ゲームエンジン: Unreal Engine
プログラミング言語: C++
主要なUnreal Engineの機能/クラス:
Enhanced Input System: UEnhancedInputComponent, UEnhancedInputLocalPlayerSubsystem
Actor, Pawn, ACharacterBase を基底としたキャラクタークラス
ActorComponent, SceneComponent
SphereComponent (当たり判定), StaticMeshComponent (メッシュ表示)
ProjectileMovementComponent (弾の物理挙動)
GameInstanceSubsystem (オブジェクトプーリング)
GameModeBase, GameStateBase (ゲームフローと状態管理)

4. プロジェクトのディレクトリ構成
plaintext
<ProjectRoot>/
└── Source/
    └── ColorShooting/
        ├── Private/
        │   ├── Bullet/
        │   ├── Character/
        │   ├── Item/
        │   ├── Subsystem/
        │   ├── ColorShooting.cpp
        │   ├── ColorShootingGameMode.cpp
        │   └── ColorShootingGameState.cpp
        └── Public/
            ├── Bullet/
            ├── Character/
            ├── Common/
            ├── Item/
            ├── Subsystem/
            ├── ColorShooting.h
            ├── ColorShootingGameMode.h
            └── ColorShootingGameState.h
Source/ColorShooting/: プロジェクトの全ソースコードが格納されるメインモジュールです。
Private/: .cppファイル（実装部）が配置されます。外部モジュールからはアクセスできません。
Public/: .hファイル（ヘッダー、宣言部）が配置されます。外部モジュールに公開されます。
Bullet/: ABulletやAGreenBulletなど、弾に関連するクラス群です。
Character/: APlayerCharacterやAEnemyCharacterなど、キャラクターに関連するクラス群です。
Item/: AItemBaseを基底とするアイテム関連のクラス群です。
Subsystem/: UBulletPoolSubsystemなど、ゲームの裏側で動作するシステムを管理します。
Common/: EShotTypeなど、プロジェクト全体で共有される定義（Enumなど）を格納します。

5. 各主要ファイルの簡単な説明
PlayerCharacter.cpp / .h

プレイヤーキャラクターの具体的な実装です。移動、射撃、武器チェンジ、ボム使用などのアクションがEnhanced Input Systemを通じてバインドされています。
Bullet.cpp / .h

全ての弾の基底クラスです。弾の基本的な物理挙動、衝突判定、ライフサイクルを定義しています。
GreenBullet.cpp / .h

ABulletを継承した緑色の弾です。敵を追尾するホーミング機能が追加実装されています。
BulletPoolComponent.cpp / .h, BulletPoolSubsystem.h

弾の生成・破棄コストを削減するためのオブジェクトプーリングシステムです。非アクティブな弾をプールに保持し、必要に応じて再利用します。
ItemBase.cpp / .h

全てのアイテムの基底クラスです。プレイヤーとの接触判定や、収集された際の消滅処理など、共通のロジックを担います。
BombItem.cpp / .h, ShotLevelUpItem.cpp / .h

AItemBaseを継承した具体的なアイテムクラスです。OnOverlapBeginをオーバーライドし、それぞれ固有の強化効果をプレイヤーに与えます。
ColorShootingGameMode.cpp / .h

ゲームのルールを定義します。このプロジェクトでは主にスコアの加算処理を担っています。
ColorShootingGameState.cpp / .h

ゲーム全体で共有される状態（現在のスコアなど）を保持します。

6. データの流れやアーキテクチャの概要
本プロジェクトは、Unreal Engineの標準的なゲームプレイアーキテクチャに基づきつつ、パフォーマンスと拡張性を考慮した設計になっています。

設計思想:

継承による機能拡張: ACharacterBaseやAItemBaseのような基底クラスに共通機能を実装し、それを継承した派生クラスで固有の機能を追加することで、コードの再利用性と保守性を高めています。
コンポーネントベース: URotatingMovementComponentやUProjectileMovementComponentなど、エンジン標準のコンポーネントを積極的に活用し、アクターの振る舞いを組み立てています。
パフォーマンス: 弾のように頻繁に生成・破棄されるオブジェクトは、オブジェクトプーリング (UBulletPoolSubsystem) を用いて負荷を軽減しています。
プレイヤーの射撃フロー:

プレイヤーが入力を行うとEnhanced Input Componentが検知し、APlayerCharacter::Fire関数を呼び出します。
現在の武器タイプ (M_CurrentShotType) に応じて、FireRedShot、FireGreenShot、FireBlueShotのいずれかが実行されます。
UBulletPoolSubsystemにアクセスし、プールから非アクティブな弾を取得します。
弾の初期位置、角度、色などのプロパティを設定し、SetActive(true)で弾を有効化（発射）します。
アイテム収集フロー:

APlayerCharacterがアイテムのUSphereComponentとオーバーラップします。
OnOverlapBeginイベントがトリガーされ、各アイテムクラス（ABombItemなど）でオーバーライドされた関数が呼び出されます。
関数内で接触したアクターがAPlayerCharacterであることを確認し、AddBomb()やAddShotLevel()といった関数を呼び出してプレイヤーのステータスを更新します。
最後にOnCollected()を呼び出し、アイテム自身をDestroy()してワールドから消去します。
