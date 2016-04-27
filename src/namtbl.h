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
 * namtbl.h : 名前表の外部仕様を定義
 *
 * 2016.02.05 v3.0.0  : トランスレータと統合(TyREF追加)
 * 2015.08.07         : 名前表サイズを300から400に変更
 * 2010.12.14         : 名前表で型を表現するために使用する定数を追加
 * 2010.07.20         : Subversion による管理を開始
 * 2010.03.31         : 定数の名前付変更('S_' => 'Nt')
 * 2010.03.30 v0.9.11 : ntOffs を ntCnt に変更
 * 2010.03.12 v0.9.10 : public 修飾に対応
 * 2009.07.22 v0.8.0  : 初期バージョン
 *
 * $Id$
 *
 */

/* 名前のスコープ */
#define ScVOID (-2)      // 仮引数やフィールド名等、通常の検索対象外
#define ScSTRC (-1)      // 構造体
#define ScPROT 0         // プロトタイプ宣言された関数
#define ScFUNC 1         // 定義された関数
#define ScCOMM 2         // 初期化されない大域変数
#define ScGVAR 3         // 初期化された大域変数
#define ScLVAR 4         // 仮引数、ローカル変数のスコープ最小値

/* 名前のデータ型 */
// <=0 は構造体を表す
//  >0 は以下の型を表す
#define TyVOID      1     // void
#define TyINT       2     // int
#define TyCHAR      3     // char
#define TyBOOL      4     // boolean
#define TySTRUC     5     // struct
#define TyINTR      6     // interrupt
#define TyDOTDOTDOT 7     // ...
#define TyREF       8     // 何か参照型
#define TyNON       9     // 型ではない

/* 名前表 */
#define NtMAX      400      // 表のサイズ
int     ntNextIdx;          // 次に登録する位置、スコープにより逆戻りする
char   *ntName[NtMAX];      // 名前のつづり
int     ntScope[NtMAX];     // スコープ(ScXXXX を格納する)
int     ntType[NtMAX];      // 型(TyVOID、TyINT、TyCHAR、...、構造体型)
int     ntDim [NtMAX];      // 配列変数の場合、配列の次元
int     ntCnt [NtMAX];      // 局所変数、仮引数の場合は番号
                            //   関数、構造体の場合は、引数やフィールドの数
//int     ntLn[NtMAX];        // 行番号
boolean ntPub [NtMAX];      // 名前が外部公開される

/* 公開関数 */
int ntSrcName(char *str);         // str と同じ名前の関数、変数、仮引数を探索
int ntSrcStruct(char *str);       // str と同じ名前の構造体を探索
int ntSrcField(int n, char *str); // 構造体(n)のフィールドから str を探索
int ntSrcGlob(int n);             // 大域名(n)と２重定義になるものを探索
void ntDefName(char *name,        // 新規登録
	       int scope, int type, int dim, int val, boolean pub);

void ntUndefName(int idx);        // 名前表をもとに戻す
void ntPrintTable();

#define ntGetSize()     (ntNextIdx)       // 名前表のサイズを返す

#define ntGetName(idx)  (ntName[idx])    // 名前表からデータを取り出す。
#define ntGetScope(idx) (ntScope[idx])
#define ntGetType(idx)  (ntType[idx])
#define ntGetDim(idx)   (ntDim[idx])
#define ntGetCnt(idx)   (ntCnt[idx])
#define ntGetPub(idx)   (ntPub[idx])

#define ntSetName(idx,v)  (ntName[idx]=(v)) // 名前表にデータを書き込む
#define ntSetScope(idx,v) (ntScope[idx]=(v))
#define ntSetType(idx,v)  (ntType[idx]=(v))
#define ntSetDim(idx,v)   (ntDim[idx]=(v))
#define ntSetCnt(idx,v)   (ntCnt[idx]=(v))
#define ntSetPub(idx,v)   (ntPub[idx]=(v))

