# プロジェクト概要とAIアシスタントへの指示

## 1. プロジェクトの目的

このプロジェクトは、Unreal Engine 5 (UE5) を使用して開発されている、横スクロールシューティングゲーム「ColorShooting」です。
プレイヤーはステージに登場するザコ敵を倒してドロップするアイテムで自機を強化し、ステージの最後に登場するボスを撃破します。

## 2. 開発環境と言語

* **ゲームエンジン:** Unreal Engine 5.6
* **主要言語:** C++ (UE5のReflection System, UObjects, Actors, Components を多用)
* **スクリプト:** Blueprint (C++で定義された機能を公開し、利用)
* **IDE:** Visual Studio Code

## 3. C++ コーディング規約とベストプラクティス

以下の Epic Games C++ コーディング規約を厳守してください。特に言及がない限り、この規約に従ってコードを生成・修正してください。
[Epic Games C++ コーディング規約へのリンクがあれば記載: 例: `https://dev.epicgames.com/documentation/ja-jp/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine`]

**全般的なコーディング原則:**

* **命名規則:**
    * クラス名: `A` (Actor), `U` (UObject), `F` (Struct), `T` (Template), `I` (Interface) プレフィックスを付ける。例: `AProjectile`, `UHealthComponent`, `FMyStruct`
    * 変数名: キャメルケース (`MyVariable`), メンバ変数は `M` プレフィックス (`M_MyMemberVariable`)。
    * 関数名: パスカルケース (`DoSomething`).
    * マクロ名: 全て大文字、アンダースコア区切り (`MY_MACRO`).
* **早期リターン (Early Exit):**
    * 関数内でガード条件やエラーチェックを行う場合、可能であれば早期リターン (early return) を積極的に使用して、ネストの深さを減らし、コードの可読性と保守性を向上させてください。
* **YAGNI (You Aren't Gonna Need It):**
    * 現時点で必要でない機能やコードは実装しないでください。将来の必要性を見越した過剰な汎用化や抽象化は避け、必要になったときに実装してください。
* **DRY (Don't Repeat Yourself):**
    * 同じロジックやコードの繰り返しを避けてください。共通の処理は関数やクラスとして抽出し、再利用可能な形で記述してください。
* **KISS (Keep It Simple, Stupid):**
    * コードは可能な限りシンプルに、そして分かりやすく記述してください。複雑な解決策よりも、シンプルで直接的なアプローチを優先してください。

**Unreal Engine 特有の注意点:**

* **Unreal Engine マクロと反射システム:**
    * クラス定義には `UCLASS()`, `USTRUCT()`, `UENUM()`, `UPROPERTY()`, `UFUNCTION()` などの適切なマクロを使用してください。これらは Unreal Engine のリフレクションシステムに必須であり、Blueprint との連携やシリアライズ、ガベージコレクションなどに利用されます。
    * `GENERATED_BODY()` または `GENERATED_UCLASS_BODY()` をクラス定義の最初に記述してください。
* **ポインタとメモリ管理:**
    * 生ポインタは極力避け、Unreal Engine 独自のスマートポインタ（例: `TSharedPtr`, `TWeakPtr`, `TUniquePtr`）を積極的に使用してください。
    * `UObject` を継承するクラスの参照には、ガベージコレクションによって管理される `TObjectPtr<T>` または `TSoftObjectPtr<T>` を推奨します。
    * `UPROPERTY()` でマークされたポインタは、Unreal Engine のガベージコレクションによって自動的に追跡され管理されます。
* **コンテナ:**
    * `TArray`, `TMap`, `TSet` などの Unreal Engine 独自のコンテナクラスを使用してください。
* **ヘッダーガード:**
    * `#pragma once` を使用してください。
* **ログ出力:**
    * `UE_LOG` マクロを使用してください。
* **Blueprint との連携:**
    * Blueprint からアクセス可能な変数には `UPROPERTY(BlueprintReadWrite)` または `UPROPERTY(EditAnywhere)` など、用途に応じたプロパティ指定子を付けてください。
    * Blueprint から呼び出し可能な関数には `UFUNCTION(BlueprintCallable)` または `UFUNCTION(BlueprintPure)` などを付けてください。C++で機能を追加する際、それがBlueprintで利用される可能性があるかどうかを考慮し、適切に公開してください。

## 4. プロジェクトのディレクトリ構造

典型的な Unreal Engine プロジェクトのディレクトリ構造を理解しています。特に `Source/` ディレクトリ以下はモジュールごとに分割されており、その中に Public と Private フォルダが存在することを考慮してください。

## 5. 特定のタスクに関する指示

* **新規クラス作成:**
    * 新しい C++ クラスを生成する場合、常に適切な UE マクロ (`UCLASS`, `UPROPERTY`, `UFUNCTION` など) を含め、Epic Games のコーディング規約に沿った形で記述してください。
    * ヘッダーファイル (`.h`) とソースファイル (`.cpp`) の両方を生成してください。
    * 関連する既存のクラス（例: `AActor`, `UGameInstance`, `UComponent`）を継承することを考慮してください。
* **既存コードの変更:**
    * コードの変更を提案する際は、既存の命名規則やスタイルを尊重し、一貫性を保つようにしてください。
    * 可能であれば、リファクタリングの提案と、なぜその変更が良いのかの理由も添えてください。
* **バグ修正:**
    * 提示されたバグに対して、根本原因を特定し、最小限の変更で問題を解決するコードを提案してください。
    * 必要に応じて、デバッグのヒントやテスト方法も提案してください。
* **パフォーマンスに関する考慮:**
    * 特にゲーム開発ではパフォーマンスが重要です。生成するコードは効率的であるように考慮してください。例: ループの最適化、不要なメモリ割り当ての回避。

## 6. その他

* 質問や不明点があれば、明確に尋ねてください。
* 複雑なタスクの場合、段階的に指示を与えてください。
* 私が提供する情報は、可能な限り詳細かつ正確であるように努めます。