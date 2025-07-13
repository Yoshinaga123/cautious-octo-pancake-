// DOMContentLoadedイベント: HTMLの読み込みが完了したときに実行される
// これにより、HTML要素が確実に存在してからJavaScriptが動作する
document.addEventListener('DOMContentLoaded', () => {
    
    // ===== DOM要素の取得 =====
    // HTML内の各要素をJavaScriptで操作するために取得
    const boardElement = document.getElementById('board');           // オセロ盤を表示するコンテナ
    const turnElement = document.getElementById('turn');             // 現在の手番を表示する要素
    const blackScoreElement = document.getElementById('black-score'); // 黒石の数を表示する要素
    const whiteScoreElement = document.getElementById('white-score'); // 白石の数を表示する要素
    const resetButton = document.getElementById('reset-button');     // ゲームリセットボタン

    // ===== 定数の定義 =====
    // 定数は値が変更されない変数。ゲーム全体で共通して使用される値
    const BOARD_SIZE = 8;    // オセロ盤のサイズ（8×8マス）
    const EMPTY = 0;         // 空のマスを表す数値（石が置かれていない状態）
    const BLACK = 1;         // 黒石を表す数値
    const WHITE = 2;         // 白石を表す数値

    // ===== ゲームの状態を管理する変数 =====
    // letで宣言された変数は値を変更可能（ゲーム中に状態が変わるため）
    let board = [];          // 8×8の二次元配列。各マスの状態（EMPTY/BLACK/WHITE）を保存
    let currentPlayer;       // 現在の手番のプレイヤー（BLACK または WHITE）
    let gameOver;           // ゲーム終了フラグ（true: 終了, false: 継続中）

    // ===== ゲーム初期化関数 =====
    // 新しいゲームを開始するための初期設定を行う
    function initGame() {
        // 8×8の二次元配列を作成し、全てのマスを空（EMPTY）に設定
        // Array(BOARD_SIZE).fill(null): サイズ8の配列を作成し、nullで埋める
        // .map(() => Array(BOARD_SIZE).fill(EMPTY)): 各要素をサイズ8の配列に変換し、EMPTYで埋める
        // 結果: [[0,0,0,0,0,0,0,0], [0,0,0,0,0,0,0,0], ...]の8×8配列
        board = Array(BOARD_SIZE).fill(null).map(() => Array(BOARD_SIZE).fill(EMPTY));
        
        // オセロの初期配置を設定
        // 中央4マスに白黒の石を交互に配置（オセロの標準的な開始配置）
        const center = BOARD_SIZE / 2;  // 中央位置の計算（8÷2=4）
        // 配列のインデックスは0から始まるため、中央は3,4の位置
        board[center - 1][center - 1] = WHITE;  // (3,3)に白石
        board[center - 1][center] = BLACK;      // (3,4)に黒石
        board[center][center - 1] = BLACK;      // (4,3)に黒石
        board[center][center] = WHITE;          // (4,4)に白石

        // ゲーム状態の初期化
        currentPlayer = BLACK;  // 黒石から開始（オセロのルール）
        gameOver = false;       // ゲーム継続中に設定
        render();              // 画面に盤面を描画
    }

    // ===== 盤面描画関数 =====
    // ゲーム盤面をHTMLに描画する関数
    function render() {
        // 既存の盤面表示をクリア（前回の描画内容を削除）
        boardElement.innerHTML = '';
        
        // 現在のプレイヤーが置ける場所（合法手）のリストを取得
        const validMoves = getValidMoves(currentPlayer);

        // 8×8のマス目を順番に処理（二重ループ）
        // 外側のループ: 行（row）を0から7まで
        // 内側のループ: 列（col）を0から7まで
        for (let row = 0; row < BOARD_SIZE; row++) {
            for (let col = 0; col < BOARD_SIZE; col++) {
                // 各マス目を表すHTML要素（div）を作成
                const cell = document.createElement('div');
                cell.className = 'cell';  // CSSクラスを設定（見た目の設定）
                
                // ===== 石の描画処理 =====
                if (board[row][col] === BLACK) {
                    // このマスに黒石がある場合
                    const stone = document.createElement('div');
                    stone.className = 'stone black';  // 黒石用のCSSクラス
                    cell.appendChild(stone);  // マス目の中に石を配置
                } else if (board[row][col] === WHITE) {
                    // このマスに白石がある場合
                    const stone = document.createElement('div');
                    stone.className = 'stone white';  // 白石用のCSSクラス
                    cell.appendChild(stone);  // マス目の中に石を配置
                } else {
                    // このマスが空の場合
                    // 石が置ける場所（合法手）かどうかをチェック
                    // Array.some(): 配列内に条件を満たす要素が1つでもあればtrueを返す
                    const isMovable = validMoves.some(move => move.row === row && move.col === col);
                    if (isMovable) {
                        // 石が置ける場所には目印（小さな円）を表示
                        const marker = document.createElement('div');
                        marker.className = 'move-marker';  // 目印用のCSSクラス
                        cell.appendChild(marker);  // マス目の中に目印を配置
                    }
                }
                
                // ===== クリックイベントの設定 =====
                // 各マス目がクリックされたときの処理を設定
                // アロー関数を使用して、クリックされたマスの座標（row, col）を渡す
                cell.addEventListener('click', () => handleCellClick(row, col));
                
                // 作成したマス目を盤面コンテナに追加
                boardElement.appendChild(cell);
            }
        }
        
        // 盤面描画完了後、ゲーム状況（手番・スコア）を更新
        updateStatus(validMoves);
    }
    
    // ===== ゲーム状況更新関数 =====
    // 手番表示、スコア表示、ゲーム終了判定を行う
    function updateStatus(validMoves) {
        // ===== 手番表示の更新 =====
        if (gameOver) {
            // ゲーム終了時: 勝敗を判定して結果を表示
            const blackCount = countStones(BLACK);  // 黒石の数をカウント
            const whiteCount = countStones(WHITE);  // 白石の数をカウント
            
            // 石の数を比較して勝敗を決定
            if (blackCount > whiteCount) {
                turnElement.textContent = "ゲーム終了！ 黒の勝ちです！";
            } else if (whiteCount > blackCount) {
                turnElement.textContent = "ゲーム終了！ 白の勝ちです！";
            } else {
                turnElement.textContent = "ゲーム終了！ 引き分けです！";
            }
        } else {
            // ゲーム継続中: 現在の手番を表示
            // 三項演算子を使用: 条件 ? true時の値 : false時の値
             turnElement.textContent = `手番: ${currentPlayer === BLACK ? "黒" : "白"}`;
        }
       
        // ===== スコア（石の数）表示を更新 =====
        blackScoreElement.textContent = countStones(BLACK);  // 黒石の数を表示
        whiteScoreElement.textContent = countStones(WHITE);  // 白石の数を表示

        // ===== ゲーム終了判定とパス処理 =====
        if (!gameOver && validMoves.length === 0) {
            // 現在のプレイヤーが置ける場所がない場合
            
            // 相手プレイヤーを計算
            const opponent = currentPlayer === BLACK ? WHITE : BLACK;
            
            // 相手プレイヤーが置ける場所があるかチェック
            if (getValidMoves(opponent).length > 0) {
                // 相手が置ける場合: パス（手番交代）
                alert(`${currentPlayer === BLACK ? "黒" : "白"} は置ける場所がありません。パスします。`);
                currentPlayer = opponent;  // 手番を相手に変更
                render();  // 画面を再描画
            } else {
                // 両者とも置けない場合: ゲーム終了
                gameOver = true;  // ゲーム終了フラグを立てる
                render();  // 画面を再描画（勝敗表示のため）
            }
        }
    }

    // ===== 石の数カウント関数 =====
    // 指定したプレイヤーの石の数を数える
    function countStones(player) {
        // board.flat(): 二次元配列を一次元配列に変換
        // 例: [[1,0,2], [0,1,0]] → [1,0,2,0,1,0]
        // .filter(): 条件に合う要素のみを抽出
        // .length: 抽出された要素の数を取得
        return board.flat().filter(cell => cell === player).length;
    }

    // ===== セルクリック処理関数 =====
    // プレイヤーがマス目をクリックしたときの処理
    function handleCellClick(row, col) {
        // ゲーム終了時はクリックを無効にする
        if (gameOver) return;

        // そのマスに石を置いた場合にひっくり返せる石のリストを取得
        const flips = getFlipsForMove(row, col, currentPlayer);
        
        // ひっくり返せる石がない場合（無効な手）
        if (flips.length === 0) {
            // 何もせずに関数を終了（無効なクリックは無視）
            return;
        }

        // ===== 有効な手の場合の処理 =====
        
        // 1. クリックされたマスに現在のプレイヤーの石を置く
        board[row][col] = currentPlayer;
        
        // 2. ひっくり返せる石を全てひっくり返す
        // forEach(): 配列の各要素に対して処理を実行
        flips.forEach(flip => {
            board[flip.row][flip.col] = currentPlayer;
        });

        // 3. プレイヤーを交代
        // 三項演算子で相手プレイヤーに切り替え
        currentPlayer = (currentPlayer === BLACK) ? WHITE : BLACK;
        
        // 4. 画面を更新（新しい盤面状況を反映）
        render();
    }

    // ===== 合法手取得関数 =====
    // 指定したプレイヤーが石を置ける全ての場所のリストを取得
    function getValidMoves(player) {
        const moves = [];  // 置ける場所を格納する配列
        
        // 盤面の全てのマス目をチェック（8×8 = 64マス）
        for (let row = 0; row < BOARD_SIZE; row++) {
            for (let col = 0; col < BOARD_SIZE; col++) {
                // そのマスに石を置いた場合にひっくり返せる石があるかチェック
                if (getFlipsForMove(row, col, player).length > 0) {
                    // ひっくり返せる石がある場合、そのマスは合法手
                    moves.push({ row, col });  // オブジェクトとして座標を保存
                }
            }
        }
        return moves;  // 合法手のリストを返す
    }

    // ===== ひっくり返し判定関数 =====
    // 特定のマスに石を置いたときにひっくり返せる石のリストを取得する
    // この関数がオセロの核となるルール判定を行う
    function getFlipsForMove(row, col, player) {
        // そのマスが既に石で埋まっている場合は置けない
        if (board[row][col] !== EMPTY) {
            return [];  // 空配列を返す（ひっくり返せる石なし）
        }

        // 相手プレイヤーを特定
        const opponent = (player === BLACK) ? WHITE : BLACK;
        let allFlips = [];  // ひっくり返せる全ての石を格納する配列

        // ===== 8方向の探索 =====
        // オセロでは石を置いたマスから8方向（上下左右と斜め4方向）に向かって
        // 相手の石を挟めるかどうかをチェックする必要がある
        const directions = [
            { r: -1, c: -1 }, // 左上
            { r: -1, c: 0 },  // 上
            { r: -1, c: 1 },  // 右上
            { r: 0, c: -1 },  // 左
                              // 中央（置く場所）はチェック不要
            { r: 0, c: 1 },   // 右
            { r: 1, c: -1 },  // 左下
            { r: 1, c: 0 },   // 下
            { r: 1, c: 1 }    // 右下
        ];

        // 各方向について石を挟めるかチェック
        for (const dir of directions) {
            let potentialFlips = [];  // この方向でひっくり返せる可能性のある石
            let r = row + dir.r;      // 探索開始位置（行）
            let c = col + dir.c;      // 探索開始位置（列）

            // 盤面の範囲内で探索を続ける
            while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                if (board[r][c] === opponent) {
                    // 相手の石を発見 → ひっくり返せる候補として記録
                    potentialFlips.push({ row: r, col: c });
                } else if (board[r][c] === player) {
                    // 自分の石で挟めた！
                    // この方向の候補石を全て確定させる
                    allFlips = allFlips.concat(potentialFlips);
                    break;  // この方向の探索終了
                } else {
                    // 空白マスに到達 → この方向では挟めない
                    break;  // この方向の探索終了
                }
                // 次のマスに移動
                r += dir.r;
                c += dir.c;
            }
        }
        
        // 全方向の探索結果を返す
        return allFlips;
    }

    // ===== イベントリスナーの設定 =====
    // リセットボタンがクリックされたときにゲームを初期化
    resetButton.addEventListener('click', initGame);

    // ===== ゲーム開始 =====
    // ページ読み込み完了時に最初のゲームを開始
    initGame();
});

// ===== 学習用のJavaScript基礎コード =====
// 以下は JavaScript Primer の学習内容

// 基本的な演算と出力
console.log(1 + 1);                    // 数値の足し算: 2
const total = 42 + 42;                  // 定数に計算結果を代入
console.log(total);                     // => 84
const greeting = "Hello, World!";       // 文字列の定数
console.log(greeting);                  // => "Hello, World!"    
console.log("Console APIで表示");        // 文字列リテラルの直接出力

// 値を評価した場合は最後の結果が表示される
42; 

// ===== エラーの例（コメントアウト済み） =====
// console.log(1; // => SyntaxError: missing ) after argument list
// SyntaxError: missing ) after argument list	
// エラーの種類はSyntaxErrorで、関数呼び出しの)が足りないこと

// cont a = 1; // => SyntaxError: unexpected token: identifier	
// エラーの種類はSyntaxErrorで、予期しない識別子（変数名 a）が指定されている

const value = "値";
// console.log(x); // => ReferenceError: x is not defined
// エラーの種類はReferenceErrorで、変数 x が定義されていないことを示す
// 変数 x は宣言されていないため、参照できません。

const value1 = "値";
console.log(value1); // => "値"

// ===== データ型の説明 =====
// プリミティブ型（基本型）は、真偽値や数値などの基本的な値の型のことです
// 一度作成したらその値自体を変更できないというイミュータブル（immutable）の特性を持ちます。
// JavaScriptでは、文字列も一度作成したら変更できないイミュータブルの特性を持ち、プリミティブ型の一種として扱われます。
const str = "Hello, World!";

// オブジェクト（複合型）と呼び、 オブジェクトは複数のプリミティブ型の値またはオブジェクトからなる集合です。
const obj = {
  name: "Alice",
  age: 30,
  isStudent: false
};

// ===== 7つのプリミティブ型（基本型） =====
// 真偽値（Boolean）: trueまたはfalseのデータ型
// 数値（Number）: 42 や 3.14159 などの数値のデータ型
// 巨大な整数（BigInt）: ES2020から追加された9007199254740992nなどの任意精度の整数のデータ型
// 文字列（String）: "JavaScript" などの文字列のデータ型
// undefined: 値が未定義であることを意味するデータ型
// null: 値が存在しないことを意味するデータ型
// シンボル（Symbol）: ES2015から追加された一意で不変な値のデータ型

// ===== オブジェクト（複合型） =====
// プリミティブ型以外のデータ
// オブジェクト、配列、関数、クラス、正規表現、Dateなど

// ===== typeof演算子の使用例 =====
console.log(typeof true);                    // => "boolean"
console.log(typeof 42);                      // => "number"
console.log(typeof 9007199254740992n);       // => "bigint"
console.log(typeof "JavaScript");            // => "string"
console.log(typeof Symbol("シンボル"));       // => "symbol"
console.log(typeof undefined);               // => "undefined"
console.log(typeof null);                    // => "object" (歴史的な仕様のバグ)
console.log(typeof ["配列"]);                // => "object"
console.log(typeof { "key": "value" });     // => "object"
console.log(typeof function() {});          // => "function"

// プリミティブ型の値は、それぞれtypeof演算子の評価結果として、その値のデータ型を返します。 
// 一方で、オブジェクトに分類される値は"object"となります。

// 配列([])とオブジェクト({})は、どちらも"object"という判定結果になります。 
// そのため、typeof演算子ではオブジェクトの詳細な種類を正しく判定することはできません。
// ただし、関数はオブジェクトの中でも特別扱いされているため、typeof演算子の評価結果は"function"となります。 
// また、typeof nullが"object"となるのは、歴史的経緯のある仕様のバグです。

// このことからもわかるようにtypeof演算子は、プリミティブ型またはオブジェクトかを判別するものです。 
// typeof演算子では、オブジェクトの詳細な種類を判定できないことは、覚えておくとよいでしょう。 
// 各オブジェクトの判定方法については、それぞれのオブジェクトの章で見ていきます。

// ===== リテラルの説明 =====
// プリミティブ型の値や一部のオブジェクトは、リテラルを使うことで簡単に定義できるようになっています。
// リテラルとはプログラム上で数値や文字列など、データ型の値を直接記述できるように構文として定義されたものです。 
// たとえば、"と"で囲んだ範囲が文字列リテラルで、これは文字列型のデータを表現しています。
// 次のコードでは、"こんにちは"という文字列型のデータを初期値に持つ変数strを定義しています。

// "と"で囲んだ範囲が文字列リテラル
const str1 = "こんにちは";

