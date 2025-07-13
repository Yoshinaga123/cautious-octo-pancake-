// ===== 多数決選挙プログラム =====
// このプログラムは、複数の候補者の中から最も多くの票を獲得した候補者を
// 勝者として決定する多数決選挙システムを実装したものです。

// ===== ヘッダーファイルのインクルード =====
#include <cs50.h>      // CS50ライブラリ（string型、get_string関数など）
#include <stdio.h>     // 標準入出力関数（printf など）
#include <string.h>    // 文字列操作関数（strcmp など）

// ===== 定数定義 =====
// #define: プリプロセッサディレクティブ（コンパイル前に置換される）
// MAX: 最大候補者数を9人に制限
#define MAX 9

// ===== 構造体定義 =====
// typedef struct: 新しいデータ型を定義
// candidate: 候補者の情報を格納する構造体
typedef struct
{
    string name;    // 候補者の名前
    int votes;      // 候補者の得票数
} candidate;

// ===== グローバル変数の定義 =====
// 関数間でデータを共有するための変数
candidate candidates[MAX];  // 候補者の配列（最大MAX人）
int candidate_count;        // 実際の候補者数

// ===== 関数プロトタイプ宣言 =====
// C言語では使用前に関数の存在を宣言する必要がある
bool vote(string name);     // 投票処理を行う関数
void print_winner(void);    // 勝者を表示する関数

// ===== メイン関数 =====
// プログラムの実行開始点
int main(int argc, string argv[])
{
    // ===== 1. コマンドライン引数の検証 =====
    // ./plurality Alice Bob Charlie のように実行する
    // argc < 2 は候補者が1人もいないことを意味する
    if (argc < 2)
    {
        printf("Usage: plurality [candidate ...]\n");
        return 1; // エラーコード1で終了
    }

    // ===== 2. 候補者情報の初期化 =====
    // argc - 1: プログラム名を除いた引数の数（候補者数）
    candidate_count = argc - 1;
    
    // 候補者数が上限を超えていないかチェック
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2; // エラーコード2で終了
    }
    
    // 候補者配列の初期化
    // i + 1: argv[0]はプログラム名なので、候補者名はargv[1]から始まる
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];  // 候補者名を設定
        candidates[i].votes = 0;           // 得票数を0で初期化
    }

    // ===== 3. 投票者数の取得 =====
    // get_int関数: ユーザーから整数を取得（CS50ライブラリ）
    int voter_count = get_int("Number of voters: ");

    // ===== 4. 投票処理のループ =====
    // 各投票者から投票を受け付ける
    for (int i = 0; i < voter_count; i++)
    {
        // 投票者から候補者名を取得
        string name = get_string("Vote: ");

        // vote関数で投票を処理
        // 戻り値がfalseの場合は無効票
        if (!vote(name))
        {
            printf("Invalid vote.\n");
        }
    }

    // ===== 5. 選挙結果の表示 =====
    print_winner();
    
    return 0; // 正常終了
}


bool vote(string name)
{
    // 全候補者をループで確認
    for (int i = 0; i < candidate_count; i++)
    {
        // ===== 文字列比較による候補者の特定 =====
        // strcmp関数: 2つの文字列を比較
        // 戻り値が0の場合は文字列が完全一致
        // 戻り値が0以外の場合は文字列が異なる
        if (strcmp(name, candidates[i].name) == 0)
        {
            // 該当する候補者の得票数を1増やす
            candidates[i].votes++;
            
            // 投票が成功したのでtrueを返す
            return true;
        }
    }
    
    // ここまで到達した場合、該当する候補者がいない（無効票）
    return false;
}

// ===== 勝者表示関数 =====
/**
 * @brief 選挙の勝者（最高得票者）を表示する
 * @param void 引数なし
 * @return なし
 * 
 * この関数の処理:
 * 1. 全候補者の中から最高得票数を見つける
 * 2. 最高得票数を獲得した候補者を全て表示する
 * 3. 同数の場合は複数の勝者が表示される（引き分け）
 */
void print_winner(void)
{
    // ===== 段階1: 最高得票数を見つける =====
    int max_votes = 0;  // 最高得票数を記録する変数
    
    // 全候補者の得票数を確認
    for (int i = 0; i < candidate_count; i++)
    {
        // 現在の候補者の得票数が記録されている最高得票数より多い場合
        if (candidates[i].votes > max_votes)
        {
            max_votes = candidates[i].votes;  // 最高得票数を更新
        }
    }

    // ===== 段階2: 最高得票数を持つ候補者を表示 =====
    // 同数で勝者が複数いる可能性があるため、全候補者を再度確認
    for (int i = 0; i < candidate_count; i++)
    {
        // 候補者の得票数が最高得票数と同じ場合
        if (candidates[i].votes == max_votes)
        {
            printf("%s\n", candidates[i].name);  // 勝者の名前を表示
        }
    }
    
    return; // 関数終了（voidなので戻り値なし）
}

// ===== プログラムの学習ポイント =====
/*
 * 1. 構造体の活用:
 *    - 関連するデータ（名前と得票数）をまとめて管理
 *    - typedef を使った新しいデータ型の定義
 * 
 * 2. 配列とループ:
 *    - 複数の候補者データを配列で管理
 *    - for文を使った効率的なデータ処理
 * 
 * 3. 文字列操作:
 *    - strcmp関数を使った文字列比較
 *    - C言語における文字列の扱い方
 * 
 * 4. 関数設計:
 *    - 機能ごとに関数を分割（vote, print_winner）
 *    - 戻り値を使った処理結果の通知
 * 
 * 5. エラーハンドリング:
 *    - 不正な入力に対する適切な対応
 *    - 複数のエラーコードによる異なる終了状態
 * 
 * 6. アルゴリズムの考え方:
 *    - 最大値探索アルゴリズム
 *    - 2段階処理（最大値発見 → 該当者特定）
 * 
 * 7. データ構造の選択:
 *    - 構造体配列による効率的なデータ管理
 *    - グローバル変数の適切な使用
 * 
 * 8. 実世界の問題解決:
 *    - 選挙システムという実際の問題をプログラムで解決
 *    - 引き分けの処理など、現実的な状況への対応
 */
