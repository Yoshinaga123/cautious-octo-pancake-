/**
 * @file runoff_commented.c
 * @brief 即座決選投票（Instant Runoff Voting）システムの実装
 * @author C言語学習プロジェクト
 * @date 2025年7月10日
 * @version 1.0
 * 
 * このファイルは、オーストラリアなどで使用される即座決選投票システムを
 * 詳細なコメント付きで実装したC言語学習用ファイルです。
 * 
 * @details
 * 主な機能:
 * - 候補者への優先順位付き投票
 * - 過半数獲得まで最下位候補者の段階的除外
 * - 動的な票の再配分アルゴリズム
 * - 同点処理とエラーハンドリング
 * 
 * @note
 * このファイルの関数は即座決選投票専用です。
 * 多数決選挙の関数とは異なる実装になっています。
 */

// ===== 即座決選投票（Instant Runoff Voting）プログラム =====
// このプログラムは、オーストラリアなどで使用される即座決選投票システムを実装したものです。
// 投票者は候補者を優先順位付けし、過半数を獲得する候補者が出るまで最下位の候補者を除外していきます。

// ===== ヘッダーファイルのインクルード =====
#include <cs50.h>   // CS50ライブラリ（string型、get_string関数、get_int関数など）
#include <stdio.h>  // 標準入出力関数（printf など）
#include <string.h> // 文字列操作関数（strcmp など）
#include <math.h>   // 数学関数（必要に応じて）

// ===== 定数定義 =====
// #define: プリプロセッサディレクティブ（コンパイル前に置換される）
#define MAX_VOTERS 100   // 最大投票者数（メモリ制限）
#define MAX_CANDIDATES 9 // 最大候補者数（メモリ制限）

// ===== 優先順位記録用の二次元配列 =====
// preferences[i][j]: 投票者i番目の、j番目の優先順位の候補者インデックス
// 例: preferences[0][0] = 2 → 投票者0番目の1位候補者は候補者2番目
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// ===== 構造体定義 =====
// typedef struct: 新しいデータ型を定義
// candidate: 候補者の情報を格納する構造体
typedef struct
{
    string name;     // 候補者の名前
    int votes;       // 現在のラウンドでの得票数
    bool eliminated; // 脱落状態（true: 脱落済み, false: 残存）
} candidate;

// ===== グローバル変数の定義 =====
candidate candidates[MAX_CANDIDATES]; // 候補者の配列
int voter_count;                      // 実際の投票者数（実行時に決定）
int candidate_count;                  // 実際の候補者数（実行時に決定）

/**
 * @defgroup runoff_functions 即座決選投票関数群
 * @brief 即座決選投票システムの関数群
 * @{
 */

// ===== 関数プロトタイプ宣言 =====
bool vote(int voter, int rank, string name); // 投票記録処理
void tabulate(void);                         // 票の集計処理
bool print_winner(void);                     // 勝者判定・表示処理
int find_min(void);                          // 最小得票数の発見
bool is_tie(int min);                        // 同点判定
void eliminate(int min);                     // 候補者脱落処理

// ===== メイン関数 =====
int main(int argc, string argv[])
{
    // ===== 1. コマンドライン引数の検証 =====
    // ./runoff Alice Bob Charlie のように実行
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1; // エラーコード1: 引数不足
    }

    // ===== 2. 候補者情報の初期化 =====
    candidate_count = argc - 1; // プログラム名を除いた引数数

    // 候補者数の上限チェック
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2; // エラーコード2: 候補者数超過
    }

    // 候補者配列の初期化
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1]; // 候補者名を設定
        candidates[i].votes = 0;          // 得票数を0で初期化
        candidates[i].eliminated = false; // 脱落状態をfalseで初期化
    }

    // ===== 3. 投票者数の取得と検証 =====
    do
    {

        voter_count = get_int("Number of voters: ");
        {
            printf("Maximum number of voters is %i\n", MAX_VOTERS);
            return 3; // エラーコード3: 投票者数超過
        }
    } while (voter_count > MAX_VOTERS); // 投票者数が上限を超えないまで再入力を促す

    // ===== 4. 投票の受付処理 =====
    // 各投票者から優先順位付きの投票を受け付ける
    for (int i = 0; i < voter_count; i++)
    {
        // 各候補者の順位を尋ねる（1位から候補者数分まで）
        for (int j = 0; j < candidate_count; j++)
        {
            // j + 1: 配列インデックスは0からだが、順位は1からなので+1
            string name = get_string("Rank %i: ", j + 1);

            // vote関数で投票を記録
            // 無効な投票の場合はプログラム終了
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4; // エラーコード4: 無効投票
            }
        }
        printf("\n"); // 投票者間の区切りとして改行
    }

    // ===== 5. 決選投票のメインループ =====
    // 過半数の候補者が出るまで繰り返す
    while (true)
    {
        // ===== 5-1. 票の集計 =====
        // 脱落していない候補者の得票数を計算
        tabulate();

        // ===== 5-2. 勝者の判定 =====
        // 過半数を獲得した候補者がいるかチェック
        bool won = print_winner();
        if (won)
        {
            break; // 勝者が決定したのでループ終了
        }

        // ===== 5-3. 最下位候補者の特定 =====
        int min = find_min();   // 最小得票数を取得
        bool tie = is_tie(min); // 同点かどうかを判定

        // ===== 5-4. 同点の場合の処理 =====
        // 残存候補者全員が同点の場合、全員が勝者
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                // 脱落していない候補者を全て表示
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break; // 結果が確定したのでループ終了
        }

        // ===== 5-5. 最下位候補者の脱落処理 =====
        eliminate(min); // 最小得票数の候補者を脱落させる

        // ===== 5-6. 次ラウンドの準備 =====
        // 全候補者の得票数をリセット（次の集計のため）
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0; // 正常終了
}

// ===== 投票記録関数 =====
/**
 * @brief 【即座決選投票用】投票を記録し、その成否を返す
 * @ingroup runoff_functions
 * 
 * この関数は即座決選投票システム専用の関数で、指定された候補者名が有効か検証し、
 * 有効であればpreferences配列に投票を記録します。
 * 
 * @param voter 投票者のインデックス（0以上の整数）
 * @param rank  優先順位（0は1位、1は2位...）
 * @param name  投票された候補者の名前
 * 
 * @return 有効な投票の場合はtrue、無効な場合はfalse
 * 
 * @details
 * 処理手順:
 * 1. 候補者名が候補者リストに存在するか線形探索で検索する
 * 2. 存在すれば、その候補者のインデックスをpreferences配列に保存する
 * 3. 存在しなければ、無効票として扱いfalseを返す
 * 
 * @note
 * この関数は即座決選投票専用です。多数決選挙の vote(string name) とは
 * 引数の数と処理内容が異なります。
 * 
 * @see plurality.c の vote(string name) - 多数決選挙用
 * @see runoff_commented.c - このファイルの即座決選投票用
 */
bool vote(int voter, int rank, string name)
{
    // ===== 線形探索による候補者の特定 =====
    // 候補者リストをループして名前を探す
    for (int i = 0; i < candidate_count; i++)
    {
        // 入力された名前と候補者の名前が一致するかチェック
        // strcmp関数: 完全一致の場合0を返す
        if (strcmp(name, candidates[i].name) == 0)
        {
            // 一致したら、preferences配列に候補者のインデックスを記録
            // preferences[voter][rank] = i: 投票者voter番目の、rank番目の優先順位は候補者i番目
            preferences[voter][rank] = i;
            return true; // 有効投票として成功を返す
        }
    }
    // ここまで到達した場合、該当する候補者がいない（無効投票）
    return false;
}

// ===== 票集計関数 =====
/**
 * @brief 【即座決選投票用】現在のラウンドでの各候補者の得票数を集計する
 * @ingroup runoff_functions
 * 
 * この関数の重要な処理:
 * 1. 各投票者の優先順位リストを確認
 * 2. 脱落していない最上位候補者を見つける  
 * 3. その候補者の得票数を1増やす
 *
 * @details
 * 即座決選投票の核心的なアルゴリズム:
 * - 脱落した候補者への票は次の優先順位の候補者に移る
 * - 各投票者について、脱落していない最初の候補者に票を与える
 * 
 * @note
 * この関数は即座決選投票専用です。多数決選挙とは異なり、
 * 候補者の脱落状態を考慮した動的な票の再配分を行います。
 */
void tabulate(void)
{
    // 全ての投票者をループする
    for (int i = 0; i < voter_count; i++)
    {
        // 各投票者の優先順位をループする（1位から順に確認）
        for (int j = 0; j < candidate_count; j++)
        {
            // 優先順位j番目の候補者のインデックスを取得
            int candidate_index = preferences[i][j];

            // その候補者が脱落していないかチェック
            if (!candidates[candidate_index].eliminated)
            {
                // 脱落していなければ、その候補者の票を1増やす
                candidates[candidate_index].votes++;

                // この投票者の票は確定したので、次の投票者に移る
                // 重要: break文により内側のループを抜ける
                break;
            }
            // 脱落している場合は次の優先順位の候補者をチェック
        }
    }
    return;
}

// ===== 勝者判定・表示関数 =====
/**
 * @brief 過半数を獲得した候補者がいるかチェックし、いれば表示する
 * @ingroup runoff_functions
 * 
 * この関数の重要な処理:
 * 1. 過半数の基準を計算（投票者数の半分）
 * 2. 各候補者の得票数が過半数を超えているかチェック
 * 3. 過半数の候補者がいれば勝者として表示
 * 
 * @return 勝者がいる場合true、いない場合false
 * 
 * @details
 * 過半数判定のロジック:
 * - 投票者数をvoter_countとする
 * - 過半数に必要な票数 = voter_count / 2 + 1
 * - 例：投票者5人の場合、3票以上で過半数
 */
bool print_winner(void)
{
    // 過半数に必要な票数を計算
    // voter_count / 2: 整数除算により過半数のボーダーラインを計算
    // 例: 投票者5人の場合、5/2=2、つまり3票以上で過半数
    int majority = voter_count / 2;

    // 全ての候補者をループする
    for (int i = 0; i < candidate_count; i++)
    {
        // 候補者が過半数の票を獲得したかチェック
        // > majority: 過半数は「半分より多い」という意味
        if (candidates[i].votes > majority)
        {
            // 勝者なので名前を表示
            printf("%s\n", candidates[i].name);
            return true; // 勝者が見つかったのでtrueを返す
        }
    }
    // 勝者が見つからなかったのでfalseを返す
    return false;
}

// ===== 最小得票数発見関数 =====
/**
 * @brief 脱落していない候補者の中での最小得票数を見つける
 * @ingroup runoff_functions
 * 
 * この関数の重要な処理:
 * 1. 最小値探索アルゴリズムを使用
 * 2. 脱落していない候補者のみを対象とする
 * 3. 理論的最大値で初期化して確実に最小値を見つける
 * 
 * @return 最小得票数（整数値）
 * 
 * @details
 * アルゴリズム:
 * - 最小値をvoter_count（理論上の最大値）で初期化
 * - 全候補者をチェックし、脱落していない候補者の得票数を比較
 * - より少ない得票数が見つかれば最小値を更新
 */
int find_min(void)
{
    // 最小得票数を、理論的に不可能な大きな値で初期化
    // voter_count: 全投票者数が理論上の最大得票数
    int min_votes = voter_count;

    // 全ての候補者をループする
    for (int i = 0; i < candidate_count; i++)
    {
        // 候補者が脱落しておらず、かつ現在の最小票数より少ないかチェック
        // 論理AND演算子(&&): 両条件が真の場合のみ実行
        if (!candidates[i].eliminated && candidates[i].votes < min_votes)
        {
            // 最小票数を更新
            min_votes = candidates[i].votes;
        }
    }
    return min_votes; // 最小得票数を返す
}

// ===== 同点判定関数 =====
/**
 * @brief 残存候補者全員が同点（最小得票数）かどうかを判定する
 * @ingroup runoff_functions
 * 
 * この関数の重要な処理:
 * 1. 脱落していない候補者数をカウント
 * 2. 最小得票数を持つ候補者数をカウント
 * 3. 両者が一致すれば全員同点と判定
 * 
 * @param min 最小得票数
 * 
 * @return 全員同点の場合true、そうでなければfalse
 * 
 * @details
 * 同点判定のロジック:
 * - 脱落していない候補者の総数をカウント
 * - その中で最小得票数を持つ候補者数をカウント
 * - 両者が等しい場合、全員が同じ得票数で同点状態
 */
bool is_tie(int min)
{
    // カウンター変数の初期化
    int remaining_candidates = 0;      // 脱落していない候補者の数
    int candidates_with_min_votes = 0; // 最小得票数を持つ候補者の数

    // 全候補者をループしてカウント
    for (int i = 0; i < candidate_count; i++)
    {
        // 候補者が脱落していない場合
        if (!candidates[i].eliminated)
        {
            remaining_candidates++; // 残存候補者数を増加

            // その候補者の票が最小得票数と等しい場合
            if (candidates[i].votes == min)
            {
                candidates_with_min_votes++; // 最小得票数候補者数を増加
            }
        }
    }

    // 脱落していない候補者の数と、最小得票数を持つ候補者の数が同じなら同点
    // 例: 残存3人、全員が5票 → 同点
    // 例: 残存3人、2人が5票、1人が7票 → 同点ではない
    if (remaining_candidates == candidates_with_min_votes)
    {
        return true; // 同点
    }
    return false; // 同点ではない
}

// ===== 候補者脱落処理関数 =====
/**
 * @brief 最小得票数の候補者を脱落させる
 * @ingroup runoff_functions
 * 
 * この関数の重要な処理:
 * 1. 全候補者の得票数をチェック
 * 2. 最小得票数と同じ得票数の候補者を特定  
 * 3. 該当する候補者の脱落フラグをtrueに設定
 * 
 * @param min 最小得票数（脱落させる基準となる得票数）
 * 
 * @details
 * 脱落処理のロジック:
 * - 全候補者をループして得票数をチェック
 * - min と同じ得票数の候補者を特定
 * - 該当候補者のeliminatedフラグをtrueに設定
 * 
 * @note
 * 同票の候補者は複数同時に脱落する可能性があります
 */
void eliminate(int min)
{
    // 全ての候補者をループする
    for (int i = 0; i < candidate_count; i++)
    {
        // 候補者の票が最小得票数と等しいかチェック
        if (candidates[i].votes == min)
        {
            // 等しければ、その候補者を脱落させる
            // eliminatedフラグをtrueに設定
            candidates[i].eliminated = true;
        }
    }
    return;
}

/**
 * @}
 */

// ===== プログラムの学習ポイント =====
/*
 * 1. 複雑なデータ構造の管理:
 *    - 二次元配列（preferences）による優先順位の記録
 *    - 構造体配列（candidates）による候補者情報の管理
 *    - 複数の状態変数（votes, eliminated）の同期
 *
 * 2. 高度なアルゴリズム:
 *    - 即座決選投票の実装
 *    - 動的な票の再配分
 *    - 反復的な候補者除外プロセス
 *
 * 3. 状態管理:
 *    - 候補者の脱落状態の追跡
 *    - ラウンドごとの得票数リセット
 *    - 投票の優先順位の動的解釈
 *
 * 4. 複数の判定ロジック:
 *    - 過半数判定
 *    - 最小値探索
 *    - 同点判定
 *
 * 5. ループ制御の高度な使用:
 *    - 無限ループ（while(true)）
 *    - 条件付きbreak文
 *    - ネストしたループの効率的な制御
 *
 * 6. エラーハンドリング:
 *    - 複数の異なるエラーコード
 *    - 境界値チェック
 *    - 無効入力の検出
 *
 * 7. 実世界のシステム設計:
 *    - 実際の選挙制度の実装
 *    - 公正性を保つアルゴリズム
 *    - エッジケース（同点など）の処理
 *
 * 8. プログラムの実行例:
 *    $ ./runoff Alice Bob Charlie
 *    Number of voters: 3
 *    Rank 1: Alice
 *    Rank 2: Bob
 *    Rank 3: Charlie
 *
 *    Rank 1: Bob
 *    Rank 2: Charlie
 *    Rank 3: Alice
 *
 *    Rank 1: Charlie
 *    Rank 2: Alice
 *    Rank 3: Bob
 *
 *    [集計・脱落処理を繰り返し]
 *    Alice
 *
 * 9. アルゴリズムの時間複雑度:
 *    - vote(): O(n) - 候補者数に比例
 *    - tabulate(): O(v×c) - 投票者数×候補者数
 *    - find_min(): O(c) - 候補者数に比例
 *    - 全体: O(r×v×c) - ラウンド数×投票者数×候補者数
 *
 * 10. 改善の可能性:
 *     - ハッシュテーブルによる候補者検索の高速化
 *     - より詳細な投票過程の表示
 *     - 投票の検証・修正機能
 *     - 結果の統計情報表示
 */
