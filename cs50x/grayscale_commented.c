#include "helpers.h"  // CS50の画像処理用ヘッダーファイル（RGBTRIPLE型等の定義）
#include <math.h>     // round()関数を使用するために必要

// Convert image to grayscale
// 画像をグレースケールに変換する関数
// グレースケール: カラー画像を白黒（灰色階調）に変換すること
// RGBの各成分値を平均化することで実現する
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    // 関数の引数について:
    // height: 画像の高さ（ピクセル数）
    // width: 画像の幅（ピクセル数）
    // image: 2次元配列として表現された画像データ
    //        RGBTRIPLE型: 1つのピクセルの赤(R)、緑(G)、青(B)の値を持つ構造体
    //        image[i][j]: i行j列目のピクセルデータ

    // Loop over all pixels (すべてのピクセルをループする)
    // 2重ループを使用して画像の全ピクセルを順番に処理
    // 外側のループ: 縦方向（行）を処理
    for (int i = 0; i < height; i++)
    {
        // 内側のループ: 横方向（列）を処理
        // この2重ループにより、左上から右下まで全ピクセルを網羅
        for (int j = 0; j < width; j++)
        {
            // Take average of red, green, and blue
            // 赤、緑、青の平均値を取る
            
            // 現在のピクセル（i行j列）のRGB値を個別の変数に取得
            // RGBTRIPLE構造体のメンバ変数にアクセス
            // rgbtRed: 赤成分（0-255の値）
            // rgbtGreen: 緑成分（0-255の値）  
            // rgbtBlue: 青成分（0-255の値）
            int red = image[i][j].rgbtRed;
            int green = image[i][j].rgbtGreen;
            int blue = image[i][j].rgbtBlue;
            
            // グレースケール変換の計算
            // 3つの色成分の算術平均を計算してグレー値を求める
            // 3.0で割ることで浮動小数点数の除算を行い、より正確な平均を得る
            // round()関数: 浮動小数点数を最も近い整数に四捨五入
            // 例: (100 + 150 + 200) / 3.0 = 150.0 → round(150.0) = 150
            //     (100 + 150 + 201) / 3.0 = 150.333... → round(150.333...) = 150
            //     (100 + 150 + 202) / 3.0 = 150.666... → round(150.666...) = 151
            int average = round((red + green + blue) / 3.0);

            // Set each color value to the average
            // 各色の値を平均値に設定する
            // グレースケールでは、R、G、Bすべてが同じ値になる
            // 同じ値のRGBは灰色を表現する
            // 例: (128, 128, 128) = 中間の灰色
            //     (0, 0, 0) = 黒
            //     (255, 255, 255) = 白
            image[i][j].rgbtRed = average;
            image[i][j].rgbtGreen = average;
            image[i][j].rgbtBlue = average;
        }
    }
    
    // return文は省略可能（void関数の場合）
    // 明示的に書くことで関数の終了を明確にしている
    return;
}

// Convert image to sepia
// 画像をセピア調に変換する関数
// セピア調: 古い写真のような茶褐色がかった色調
// 暖かみのある色合いで、ノスタルジックな雰囲気を演出する
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    // Loop over all pixels
    // すべてのピクセルをループして処理
    // grayscale関数と同じ2重ループ構造
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Get original colors
            // 元の色を取得
            // セピア変換では元のRGB値から新しいRGB値を計算する
            // 元の値を変数に保存してから計算に使用
            int originalRed = image[i][j].rgbtRed;
            int originalGreen = image[i][j].rgbtGreen;
            int originalBlue = image[i][j].rgbtBlue;

            // Calculate new colors using the sepia formula
            // セピアの公式を使って新しい色を計算する
            // セピアアルゴリズム: 各色成分に特定の係数を掛けて合計する
            // この係数は実際の古い写真の色調を分析して決められた値
            
            // 赤成分の計算: 元の赤×0.393 + 元の緑×0.769 + 元の青×0.189
            // セピアでは赤成分が強調されるため、緑の係数が最も大きい
            int sepiaRed = round(.393 * originalRed + .769 * originalGreen + .189 * originalBlue);
            
            // 緑成分の計算: 元の赤×0.349 + 元の緑×0.686 + 元の青×0.168
            // セピアの中間色（茶色っぽい緑）を作る
            int sepiaGreen = round(.349 * originalRed + .686 * originalGreen + .168 * originalBlue);
            
            // 青成分の計算: 元の赤×0.272 + 元の緑×0.534 + 元の青×0.131
            // セピアでは青成分が最も抑制される（暖かい色調にするため）
            int sepiaBlue = round(.272 * originalRed + .534 * originalGreen + .131 * originalBlue);

            // Cap the values at 255 if they exceed it
            // 値が255を超えた場合は255に制限する
            // RGB値の範囲は0-255なので、計算結果がこの範囲を超えないよう制限
            // オーバーフローを防ぐための重要な処理
            if (sepiaRed > 255)
            {
                sepiaRed = 255;  // 赤成分の上限制限
            }
            if (sepiaGreen > 255)
            {
                sepiaGreen = 255;  // 緑成分の上限制限
            }
            if (sepiaBlue > 255)
            {
                sepiaBlue = 255;  // 青成分の上限制限
            }

            // Update pixel with sepia values
            // ピクセルをセピアの値に更新する
            // 計算した新しいRGB値で元のピクセルを上書き
            image[i][j].rgbtRed = sepiaRed;
            image[i][j].rgbtGreen = sepiaGreen;
            image[i][j].rgbtBlue = sepiaBlue;
        }
    }
    return;
}

// Reflect image horizontally
// 画像を水平方向に反転する関数
// 鏡に映したような効果を作る（左右反転）
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // Loop over each row
    // 各行をループする
    // 行ごとに左右のピクセルを交換していく
    for (int i = 0; i < height; i++)
    {
        // Loop halfway through the row
        // 行の半分までループする
        // 重要: width/2まで。全部ループすると元に戻ってしまう
        // 例: 幅が6の場合、j=0,1,2のみ処理（j=3,4,5は既に交換済み）
        for (int j = 0; j < width / 2; j++)
        {
            // Swap pixels on opposite sides
            // 反対側のピクセルを交換する
            // 3つのステップで値を交換（temp変数を使った標準的な交換方法）
            
            // ステップ1: 一時的な変数に左側のピクセルを保存
            // temp = 左側の値
            RGBTRIPLE temp = image[i][j];
            
            // ステップ2: 左側のピクセルを右側のピクセルで上書き
            // 左側 = 右側の値
            // width-1-j: 右端からj番目の位置を計算
            // 例: width=6, j=0なら width-1-0=5 (最右端)
            //     width=6, j=1なら width-1-1=4 (右から2番目)
            image[i][j] = image[i][width - 1 - j];
            
            // ステップ3: 右側のピクセルを、保存しておいた元の左側のピクセルで上書き
            // 右側 = tempに保存した元の左側の値
            image[i][width - 1 - j] = temp;
        }
    }
    return;
}

// Blur image
// 画像にぼかしをかける関数
// ボックスブラー: 各ピクセルを周囲のピクセルとの平均値に置き換える
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Create a temporary copy of the image
    // 画像の一時的なコピーを作成する
    // 重要: ぼかし計算中に元のピクセル値を参照し続けるため
    // 元の配列を直接変更すると、後のピクセルの計算で既に変更された値を使ってしまう
    RGBTRIPLE temp[height][width];
    
    // 元の画像を一時配列にコピー
    // 2重ループですべてのピクセルをコピー
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            temp[i][j] = image[i][j];  // 構造体の値コピー
        }
    }

    // Loop over all pixels
    // すべてのピクセルに対してぼかし処理を実行
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // 各色成分の合計値を保存する変数を初期化
            int sumRed = 0, sumGreen = 0, sumBlue = 0;
            int count = 0;  // 有効なピクセル数をカウント

            // Loop over neighboring pixels (3x3 grid)
            // 周囲のピクセル(3x3のグリッド)をループする
            // 現在のピクセルを中心とした3×3の範囲を処理
            // k=-1,0,1: 上の行、現在の行、下の行
            // l=-1,0,1: 左の列、現在の列、右の列
            for (int k = -1; k <= 1; k++)
            {
                for (int l = -1; l <= 1; l++)
                {
                    // 隣接ピクセルの座標を計算
                    int new_i = i + k;  // 新しい行座標
                    int new_j = j + l;  // 新しい列座標

                    // Check if the neighbor pixel is within the image boundaries
                    // 隣のピクセルが画像の範囲内にあるかチェックする
                    // 境界チェック: 0以上、かつ画像サイズ未満であること
                    // 画像の端のピクセルでは、存在しない隣接ピクセルは無視する
                    if (new_i >= 0 && new_i < height && new_j >= 0 && new_j < width)
                    {
                        // Add the color values from the temporary copy
                        // 一時コピーから色の値を追加する
                        // 元の画像データ（temp）から値を取得することが重要
                        sumRed += temp[new_i][new_j].rgbtRed;
                        sumGreen += temp[new_i][new_j].rgbtGreen;
                        sumBlue += temp[new_i][new_j].rgbtBlue;
                        count++;  // 有効ピクセル数を増加
                    }
                }
            }

            // Calculate the average color and update the original image
            // 平均色を計算し、元の画像を更新する
            // 合計値を有効ピクセル数で割って平均を求める
            // (float)キャスト: 整数除算ではなく浮動小数点除算を行うため
            // round(): 四捨五入して整数に変換
            image[i][j].rgbtRed = round((float)sumRed / count);
            image[i][j].rgbtGreen = round((float)sumGreen / count);
            image[i][j].rgbtBlue = round((float)sumBlue / count);
        }
    }
    return;
}

/*
グレースケール変換の仕組み:

1. カラー画像の各ピクセルはRGB（赤・緑・青）の3つの成分値を持つ
2. 人間の目は緑に最も敏感で、赤、青の順に感度が下がる
3. 単純平均法（このコードの方法）:
   - 3つの成分の算術平均を取る
   - 計算式: グレー値 = (R + G + B) / 3
   - 簡単だが、人間の視覚特性を考慮していない

4. より高度な方法（加重平均法）:
   - 人間の視覚特性を考慮した重み付け
   - 計算式: グレー値 = 0.299×R + 0.587×G + 0.114×B
   - より自然なグレースケール変換が可能

5. 処理の流れ:
   - 各ピクセルのRGB値を読み取り
   - 平均値を計算
   - R、G、Bすべてを同じ平均値に設定
   - 結果として灰色のピクセルになる

使用例:
- 元のピクセル: R=200, G=100, B=50
- 平均値: (200 + 100 + 50) / 3 = 116.67 → round(116.67) = 117
- 変換後: R=117, G=117, B=117（灰色）

注意点:
- round()関数を使用するためには #include <math.h> が必要
- 整数除算（3で割る）だと小数部分が切り捨てられて精度が落ちる
- そのため浮動小数点数除算（3.0で割る）を使用

セピア変換の仕組み:

1. セピア調は古い写真のような色合いを再現するための処理
2. 各ピクセルのRGB値に特定の係数を掛けて新しいRGB値を計算
3. セピアアルゴリズム:
   - 赤成分: 0.393×R + 0.769×G + 0.189×B
   - 緑成分: 0.349×R + 0.686×G + 0.168×B
   - 青成分: 0.272×R + 0.534×G + 0.131×B
4. 計算結果が255を超えないように上限を設定
5. 暖かみのある茶褐色の色調に変換される

反転処理の仕組み:

1. 画像を水平方向に反転（左右反転）する処理
2. 各行で左端と右端のピクセルを交換
3. 幅の半分までループして処理（全部やると元に戻る）
4. 鏡に映したような効果を得ることができる

ぼかし処理の仕組み:

1. 画像にぼかし効果を適用する処理
2. 各ピクセルを周囲3×3の平均値で置換
3. 元画像のコピーを作成してから処理
4. 画像端では存在するピクセルのみで平均計算
5. ソフトフォーカスやノイズ軽減に効果的

共通の学習ポイント:
1. 2次元配列の操作方法
2. 構造体（RGBTRIPLE）の扱い方
3. ピクセル単位での画像処理
4. 数学的な色彩変換
5. 境界条件の処理
6. メモリ管理（一時配列の使用）
*/

/*
typedef struct
{
    WORD   bfType;
    DWORD  bfSize;
    WORD   bfReserved1;
    WORD   bfReserved2;
    DWORD  bfOffBits;
} __attribute__((__packed__))
BITMAPFILEHEADER;

typedef struct
{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
} __attribute__((__packed__))
/