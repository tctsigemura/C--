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
 * sytree.c : 構文木(Syntax Tree)の管理プログラム
 *
 * 2016.09.15         : syGetSize() を削除
 *                      sySetSize() を syClear() に名前変更
 * 2016.06.06         : syNewNode() が行番号も記録するように変更
 * 2016.06.04         : syGetSize() を追加
 *                      syClear() を sySetSize() に名前変更
 * 2016.05.22         : SySIZE を追加
 * 2016.05.05         : syGetRoot() が構文木が存在しない時 SyNULL を返す
 * 2016.05.04         : SyARG を SyPRM(パラメータ)に変更
 * 2016.02.05 v3.0.0  : トランスレータと統合
 *                      (SyPOST, SyBYTE 削除、SyIDXx, SyDOT, SyVAR, SyBLK 追加)
 * 2012.09.12         : 外部非公開の関数、変数に static を付ける
 * 2012.08.14         : sySetXX, syGetXX マクロを使用して操作するように変更
 * 2011.04.24         : syPrintTree, 表の改良
 * 2010.07.20         : Subversion による管理を開始
 * 2010.04.06         : 初期化データを表現するために SyARRY, SyLIST を追加
 * 2010.03.30 v0.9.10 : ローカル変数と仮引数を区別するために SyARG を追加
 * 2009.11.03 v0.9.9  : addrof 演算子(SyLABL)に対応
 * 2009.07.24 v0.9.0  : 新規に作成
 *
 * $Id$
 *
 */

#include <stdio.h>
#include "sytree.h"
#include "lexical.h"
#include "util.h"

// 構文木表
static int syNextIdx = 0;                      // 次に登録する場所

// 構文木表に新しいノードを作る
int syNewNode(int type, int lVal, int rVal) {
  int idx = syNextIdx;
  if (idx>=SyMAX) error("構文木が大き過ぎる");
  sySetType(idx, type);
  sySetLVal(idx, lVal);
  sySetRVal(idx, rVal);
  sySetLn(idx, lxGetLn());                     //  ソースの行番号も記録
  syNextIdx = syNextIdx + 1;
  return idx;
}

// 二つのノードを ',' で接続する
int syCatNode(int lval, int rval) {
  if (lval!=SyNULL && rval!=SyNULL)            // ２つのノードが本当に存在する
    return syNewNode(SySEMI, lval, rval);      //   接続したものを返す
  if (lval!=SyNULL)                            // 左が存在する
    return lval;                               //   左を返す
  return rval;                                 // どちらでもなければ右を返す
}

// 構文木を消す
void syClear() {
  syNextIdx = 0;
}

// 構文木のルートを取り出す
int syGetRoot() {
  int root = syNextIdx - 1;            // 最後に登録されたノードがルート
  if (root<0) root = SyNULL;           // 木が存在しないなら null を返す
  return root;
}

// デバッグ用
/*
struct D {char * a; int b; };
static struct D d[] = {
  // 特別な値
  {"SyNULL",  SyNULL},

  // 構文木のノードの Type
  { "SyCNST", SyCNST},                 // 数値定数、文字定数、論理定数
  { "SyLOC",  SyLOC},                  // ローカル変数、仮引数
  { "SyPRM",  SyPRM},                  // ローカル変数、仮引数
  { "SyGLB",  SyGLB},                  // グローバル変数
  { "SySTR",  SySTR},                  // 文字列
  { "SyFUNC", SyFUNC},                 // 関数コール
  { "SyLABL", SyLABL},                 // ラベル(addrof 演算子が使用)

  { "SyNEG",  SyNEG},                  // 単項演算 -
  { "SyNOT",  SyNOT},                  // 単項演算 !
  { "SyBNOT", SyBNOT},                 // 単項演算 ~(ビット毎のNOT)
  { "SyCHAR", SyCHAR},                 // 文字型へ変換する演算子
  { "SyBOOL", SyBOOL},                 // 文字型へ変換する演算子
  { "SySIZE", SySIZE},                 // データ型のサイズを求める演算子

  { "SyADD",  SyADD},                  // ２項演算 +
  { "SySUB",  SySUB},                  // ２項演算 -
  { "SySHL",  SySHL},                  // ２項演算 <<
  { "SySHR",  SySHR},                  // ２項演算 >>
  { "SyBAND", SyBAND},                 // ２項演算 &
  { "SyBXOR", SyBXOR},                 // ２項演算 ^
  { "SyBOR",  SyBOR},                  // ２項演算 |
  { "SyMUL",  SyMUL},                  // ２項演算 *
  { "SyDIV",  SyDIV},                  // ２項演算 /
  { "SyMOD",  SyMOD},                  // ２項演算 %
  { "SyIDXW", SyIDXW},                 // 後置演算子(ワード配列([ ]))
  { "SyIDXB", SyIDXB},                 // 後置演算子(バイト配列([ ]))

  { "SyGT",   SyGT},                   // ２項演算 >  (Greater Than)
  { "SyGE",   SyGE},                   // ２項演算 >  (Greater or Equal)
  { "SyLT",   SyLT},                   // ２項演算 <  (Less Than)
  { "SyLE",   SyLE},                   // ２項演算 <= (Less or Equal)
  { "SyEQU",  SyEQU},                  // ２項演算 == (EQUal)
  { "SyNEQ",  SyNEQ},                  // ２項演算 != (Not EQual)

  { "SyASS",  SyASS},                  // ２項演算 =  (ASSign)
  { "SyCOMM", SyCOMM},                 // ２項演算 ,  (COMMa)

  { "SyOR",   SyOR},                   // ２項演算 ||
  { "SyAND",  SyAND},                  // ２項演算 &&

  { "SyIF",   SyIF},                   // if 文
  { "SyELS",  SyELS},                  // if - else 文
  { "SyWHL",  SyWHL},                  // while 文
  { "SyDO",   SyDO},                   // do - while 文
  { "SyBRK",  SyBRK},                  // break 文
  { "SyCNT",  SyCNT},                  // continue 文
  { "SyRET",  SyRET},                  // return 文
  { "SySEMI", SySEMI},                 // セミコロン
  { "SyVAR",  SyVAR},                  // セミコロン
  { "SyBLK",  SyBLK},                  // セミコロン

  { "SyARRY", SyARRY},                 // 非初期化配列
  { "SyLIST", SyLIST}                  // 配列要素の初期化並び
};

void syPrintTree() {
  for (int i=0; i<syNextIdx; i=i+1) {
    int n = -1;
    for (int j=0; j<sizeof(d)/sizeof(struct D); j++) {
      if (syGetType(i)==d[j].b) n = j;
    }
    if (n==-1) error("sysPrintTree バグ");
    fprintf(stderr,"%3d: (%-6s,", i, d[n].a);
    if (syGetLVal(i)==SyNULL)
      fprintf(stderr,"   -,");
    else
      fprintf(stderr,"%4d,", syGetLVal(i));
    if (syGetRVal(i)==SyNULL)
      fprintf(stderr,"   -)\n");
    else
      fprintf(stderr,"%4d)\n", syGetRVal(i));
  }
}
*/
