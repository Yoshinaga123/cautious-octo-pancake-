// Modifies the volume of an audio file
// 音声ファイルの音量を変更するプログラム

#include <stdint.h>  // 固定幅整数型（uint8_t, int16_t等）を使用するため
#include <stdio.h>   // ファイル操作（fopen, fread, fwrite等）を使用するため
#include <stdlib.h>  // atof関数（文字列を浮動小数点数に変換）を使用するため

// Number of bytes in .wav header
// WAVファイルのヘッダーサイズは44バイト固定
// ヘッダーには音声データの情報（サンプリングレート、チャンネル数、ビット深度等）が含まれる
const int HEADER_SIZE = 44;

int main(int argc, char *argv[])
{
    // Check command-line arguments
    // コマンドライン引数をチェック
    // argc: 引数の個数（プログラム名も含む）
    // argv: 引数の配列（argv[0]=プログラム名, argv[1]=入力ファイル名, argv[2]=出力ファイル名, argv[3]=倍率）
    if (argc != 4)
    {
        printf("Usage: ./volume input.wav output.wav factor\n");
        return 1;  // エラー終了
    }

    // Open files and determine scaling factor
    // ファイルを開き、音量変更の倍率を決定

    // 入力ファイルを読み取り専用で開く
    // "r"モード: 読み取り専用でテキストファイルを開く
    // WAVファイルはバイナリファイルなので本来は"rb"が適切だが、この例では"r"を使用
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Could not open file.\n");
        return 1;  // ファイルが開けない場合はエラー終了
    }

    // 出力ファイルを書き込み専用で開く
    // "w"モード: 書き込み専用でファイルを開く（既存ファイルがあれば上書き）
    // 同様にバイナリファイルなので本来は"wb"が適切
    FILE *output = fopen(argv[2], "w");
    if (output == NULL)
    {
        printf("Could not open file.\n");
        return 1;  // ファイルが開けない場合はエラー終了
    }

    // 音量変更の倍率を文字列から浮動小数点数に変換
    // atof: ASCII to Float（文字列を浮動小数点数に変換する関数）
    // 例：argv[3]が"2.0"なら factor = 2.0（音量2倍）
    //     argv[3]が"0.5"なら factor = 0.5（音量半分）
    float factor = atof(argv[3]);

    // --- ここからが実装部分 ---

    // TODO: Copy header from input file to output file
    // 1. ヘッダーのコピー
    // WAVファイルの最初の44バイトはヘッダー情報で、音声データではない
    // このヘッダーには音声の形式情報が含まれているので、そのままコピーする必要がある
    
    // uint8_t: 8ビット符号なし整数（0〜255の値を取る）
    // ヘッダーを格納するためのバイト配列を宣言
    uint8_t header[HEADER_SIZE];
    
    // fread: ファイルからデータを読み込む関数
    // fread(読み込み先のバッファ, 1要素のサイズ, 要素数, ファイルポインタ)
    // ここでは: header配列に, HEADER_SIZEバイトを, 1回で, inputファイルから読み込む
    fread(header, HEADER_SIZE, 1, input);
    
    // fwrite: ファイルにデータを書き込む関数
    // fwrite(書き込み元のバッファ, 1要素のサイズ, 要素数, ファイルポインタ)
    // 読み込んだヘッダーをそのまま出力ファイルに書き込む
    fwrite(header, HEADER_SIZE, 1, output);

    // TODO: Read samples from input file and write updated data to output file
    // 2. サンプルの読み込み、音量変更、書き込み
    // ヘッダー以降は音声データ（サンプル）が続く
    // WAVファイルでは通常、1つのサンプルは16ビット（2バイト）の符号付き整数
    
    // int16_t: 16ビット符号付き整数（-32,768〜32,767の値を取る）
    // 1つのサンプル（音の瞬間的な振幅値）を格納する変数
    int16_t sample;
    
    // whileループでファイルの終端まで1サンプルずつ処理
    // fread(&sample, sizeof(int16_t), 1, input) の戻り値:
    // - 成功時: 読み込んだ要素数（ここでは1）
    // - ファイル終端や失敗時: 0
    // 0以外（真）の間ループが継続される
    while (fread(&sample, sizeof(int16_t), 1, input))
    {
        // サンプルに係数を掛けて音量を変更
        // 音量の変更は単純に振幅値に倍率を掛けるだけ
        // factor = 2.0 なら音量2倍（振幅2倍）
        // factor = 0.5 なら音量半分（振幅半分）
        // factor = 0.0 なら無音
        // 
        // 注意: int16_tの範囲を超える場合はオーバーフローが発生する可能性がある
        // 例：sample=20000, factor=2.0 の場合、40000となり正常
        //     sample=30000, factor=2.0 の場合、60000となるが、int16_tの最大値32767を超える
        sample = sample * factor;

        // 変更したサンプルを出力ファイルに書き込む
        // sizeof(int16_t): int16_t型のサイズ（通常2バイト）
        // &sample: sample変数のアドレス（ポインタ）
        fwrite(&sample, sizeof(int16_t), 1, output);
    }

    // --- ここまでが実装部分 ---

    // Close files
    // 開いたファイルを必ず閉じる
    // メモリリークを防ぎ、他のプログラムがファイルにアクセスできるようにする
    fclose(input);   // 入力ファイルを閉じる
    fclose(output);  // 出力ファイルを閉じる
    
    // main関数の正常終了
    // return 0; を省略した場合、C99以降では自動的に0が返される
}
