/*
 * Programing Language C-- "Compiler"
 *    Tokuyama kousen Educational Computer 16bit Ver.
 *
 * Copyright (C) 2002-2016 by
 *                      Dept. of Computer Science and Electronic Engineering,
 *                      Tokuyama College of Technology, JAPAN
 *
 *   上記著作権者は，Free Software Foundation によって公開されている GNU 一般公
 * 衆利用許諾契約書バージョン２に記述されている条件を満たす場合に限り，本ソース
 * コード(本ソースコードを改変したものを含む．以下同様)を使用・複製・改変・再配
 * 布することを無償で許諾する．
 *
 *   本ソースコードは＊全くの無保証＊で提供されるものである。上記著作権者および
 * 関連機関・個人は本ソースコードに関して，その適用可能性も含めて，いかなる保証
 * も行わない．また，本ソースコードの利用により直接的または間接的に生じたいかな
 * る損害に関しても，その責任を負わない．
 *
 *
 */

/*
 * sytree.h : 構文木関係の外部仕様を定義
 *
 * 2016.07.22         : SySIZE は単項演算子ではなく因子に分類すべき
 * 2016.06.06         : syLn[], syGetLn(), sySetLn() を追加
 * 2016.06.04         : syGetSize() を追加
 *                      syClear() を sySetSize() に名前変更
 * 2016.05.22         : SySIZE を追加
 * 2016.05.04         : SyARG を SyPRM(パラメータ)に変更
 * 2016.02.05 v3.0.0  : トランスレータと統合
 *                      (SyPOST, SyBYTE 削除, SyIDXx, SyDOT, SyVAR, SyBLK 追加)
 * 2010.07.20         : Subversion による管理を開始
 * 2010.04.06         : 初期化データを表現するために SyARRY, SyLIST を追加
 * 2010.03.30 v0.9.11 : ローカル変数と仮引数を区別するために SyARG を追加
 * 2009.11.03 v0.9.9  : addrof 演算子のために SyLABL を追加
 * 2009.07.23 v0.9.0  : 新規に作成
 *
 * $Id$
 *
 */

// 構文木表の本体
#define SyMAX 3000                           // 表の大きさ
int syType[SyMAX];                           // ノードの種類
int syLVal[SyMAX];                           // ノードの値１
int syRVal[SyMAX];                           // ノードの値２
int syLn[SyMAX];                             // 対応するソースの行番号

// 構文木のノード(LVal, Rval)に格納される特殊な値
#define SyNULL        9999                   // 構文木のNULLポインタ

// 構文木のノードの Type

#define SyISFACT(c)   (((c)&0xf00)==0x100)  // 因子かどうか判定
#define SyCNST        0x102                 // 数値定数、文字定数、論理定数
#define SyLOC         0x103                 // ローカル変数
#define SyPRM         0x104                 // 仮引数
#define SyGLB         0x105                 // グローバル変数
#define SySTR         0x106                 // 文字列
#define SyFUNC        0x107                 // 関数コール
#define SyLABL        0x108                 // 大域ラベル(addrof 演算子が使用)
#define SySIZE        0x109                 // sizeof 演算子

#define SyIS1OPR(c)   (((c)&0xf00)==0x200)  // 単項演算子かどうか判定
#define SyNEG         0x200                 // 単項演算 -
#define SyNOT         0x201                 // 単項演算 !
#define SyBNOT        0x202                 // 単項演算 ~(ビット毎のNOT)
#define SyCHAR        0x203                 // chr 演算子(型変換)
#define SyBOOL        0x204                 // bool 演算子(型変換)

#define SyIS2OPR(c)   (((c)&0xf00)==0x300)  // 普通の２項演算子かどうか判定
#define SyADD         0x300                 // ２項演算 +
#define SySUB         0x301                 // ２項演算 -
#define SySHL         0x302                 // ２項演算 <<
#define SySHR         0x303                 // ２項演算 >>
#define SyBAND        0x304                 // ２項演算 &
#define SyBXOR        0x305                 // ２項演算 ^
#define SyBOR         0x306                 // ２項演算 |
#define SyMUL         0x307                 // ２項演算 *
#define SyDIV         0x308                 // ２項演算 /
#define SyMOD         0x309                 // ２項演算 %
#define SyIDXW        0x30a                 // 後置演算子(ワード配列([ ]))
#define SyIDXB        0x30b                 // 後置演算子(バイト配列([ ]))
#define SyDOT         0x30c                 // 後置演算子(構造体(.))

#define SyISCMP(c)    (((c)&0xf00)==0x400)  // 比較演算かどうか判定
#define SyGT          0x404                 // ２項演算 >  (Greater Than)
#define SyGE          0x405                 // ２項演算 >  (Greater or Equal)
#define SyLT          0x406                 // ２項演算 <  (Less Than)
#define SyLE          0x407                 // ２項演算 <= (Less or Equal)
#define SyEQU         0x408                 // ２項演算 == (EQUal)
#define SyNEQ         0x409                 // ２項演算 != (Not EQual)

#define SyASS         0x500                 // ２項演算 =  (ASSign)
#define SyCOMM        0x501                 // ２項演算 ,  (COMMa)

#define SyISLOPR(c)   (((c)&0xf00)==0x600)  // 論理積和かどうか判定
#define SyOR          0x600                 // ２項演算 ||
#define SyAND         0x601                 // ２項演算 &&

#define SyISSTAT(c)   (((c)&0xf00)==0x700)  // 文またはセミコロン
#define SyIF          0x700                 // if 文
#define SyELS         0x701                 // if - else 文
#define SyWHL         0x702                 // while 文
#define SyDO          0x703                 // do - while 文
#define SyBRK         0x704                 // break 文
#define SyCNT         0x705                 // continue 文
#define SyRET         0x706                 // return 文
#define SySEMI        0x707                 // セミコロン
#define SyVAR         0x708                 // ローカル変数宣言
#define SyBLK         0x709                 // ブロック

// 初期化データの表現用
#define SyARRY        0x800                 // 非初期化配列(array)
#define SyLIST        0x801                 // 配列要素の初期化並び

/*
 * 公開関数
 */
int syNewNode(int type, int lVal, int rVal); // 新しいノードを作る
int syCatNode(int lVal, int rVal);           // 二つのノードを "," で接続する
int syGetSize();                             // 表の現在のサイズを返す
void sySetSize(int n);                       // 表の n1 以下を捨てる
int syGetRoot();                             // 構文木のルートを取り出す

#define syGetType(idx) (syType[idx])         // 構文木表からデータを取り出す
#define syGetLVal(idx) (syLVal[idx])
#define syGetRVal(idx) (syRVal[idx])
#define syGetLn(idx)   (syLn[idx])

#define sySetType(idx,v) (syType[idx]=(v))   // 構文木表にデータを書き込む
#define sySetLVal(idx,v) (syLVal[idx]=(v))
#define sySetRVal(idx,v) (syRVal[idx]=(v))
#define sySetLn(idx,v)   (syLn[idx]=(v))

void syDebPrintTree();                        // デバッグ用
void syPrintTree(FILE *fp);                   // 構文木を中間ファイルに出力
