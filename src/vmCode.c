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
 * vmCode.c : C--コンパイラの仮想マシン用コード生成ルーチン
 *
 * 2016.05.20         : genProto, genStruc, genOn, genOff 関数廃止
 * 2016.05.05         : genBoolExpr() にバグチェックの error() 追加
 * 2016.05.04         : SyARG を SyPRM(パラメータ)に変更
 *                      vmLdArg, vmStArg を vmLdPrm, vmStPrm に変更
 * 2016.02.05 v3.0.0  : トランスレータと統合
 *                      (genProto, genStruc, genOn, genOff 関数追加)
 *                      (SyPOST,SyBYTE の代わりに SyIDXW,SyIDXB,SyDOT に対応)
 * 2016.02.04         : isCommutativeOp() で SyPOST, SyBYTE の分類間違え
 *                      (スタックの深さ評価を間違え効率の悪いコードを生成)
 * 2016.01.04         : 未使用のローカル変数を2箇所発見、削除
 * 2015.08.31 v2.1.0  : カーネルコンパイル用のフラグを genFunc に追加
 * 2011.04.30         : 配列や構造体の初期化を optTree で処理するように変更
 * 2011.02.07         : jmpBlock が syNULL で呼ばれた場合に対応
 * 2011.01.12         : vmPop 関数の呼び出しにあった余分な引数を削除
 * 2010.09.12 v1.0.0  : 最適化と外部変数の定数式による初期化ができる H8 統合版
 *                    : 外部変数の初期化で定数式が使用できるように変更
 *                    : 必要なもの以外は static にする
 * 2010.09.09         : genFunc に最適化フラグ追加
 * 2010.09.07         : 後置演算で右辺式を最適化ルーチンが削除した場合に対応
 * 2010.09.02         : BOOLOR、BOOLAND マクロ命令を追加
 *                    : AADD を削除し、SCALE を追加
 * 2010.08.31         : スタックが浅くなるように２項演算子の演算順序を変更
 * 2010.08.23         : RETF, RETP を統合し、関数の返り値をハードウェア
 *                      レジスタに移動する命令 MREG を追加
 *                    : ラベル欄を印刷する関数の改行あり/なしの区別をやめた
 * 2010.08.16         : アドレス計算用の命令 AADD を追加
 * 2010.07.20         : Subversion による管理を開始
 * 2010.04.22         : 最右辺に部分式がある条件式のコード生成にバグ
 * 2010.04.10         : データ生成をコード生成に移動
 * 2010.03.31         : TaC用コード生成から枝分かれして新規開発開始
 *
 * 2010.03.30 v0.9.11 : ローカル変数と仮引数を区別するために SyARG を追加
 * 2010.03.12 v0.9.10 : public 修飾に対応
 * 2009.11.03 v0.9.9  : addrof 演算子に対応, interrupt 関数に対応
 * 2009.09.15 v0.9.0  : 初期バージョン完成
 * 2009.08.12         : コーディング開始
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "code.h"
#include "namtbl.h"
#include "sytree.h"
#include "vm.h"

// 大域データ
static int curLab = 0;                            // ラベル用のカウンタ
static int curLabStr = 0;                         // STRINGラベル用のカウンタ
static int retLab;                                // return 時のジャンプ先
static int brkLab;                                // break 時のジャンプ先
static int cntLab;                                // continue 時のジャンプ先

// ラベルを割り当てる
static int newLab() {
  curLab = curLab + 1;
  return curLab;
}

// STRING用ラベルを割り当てる
static int newLabStr() {
  curLabStr = curLabStr + 1;
  return curLabStr;
}

// 値のある場所
#define INVAL 0                                   // 使用していない
#define CNST  1                                   // 定数
#define GVAR  2                                   // 大域変数
#define LVAR  3                                   // ローカル変数
#define PRM   4                                   // 仮引数
#define STR   5                                   // 文字列ラベル
#define STKD  6                                   // スタックに置かれたデータ
#define STKW  7                                   // スタックにワード配列を
                                                  //   参照する添字とアドレス
#define STKB  8                                   // スタックにバイト配列を
                                                  //   参照する添字とアドレス
#define LABL  9                                   // 大域ラベル(定数)

// 式の状態を表現する構造体
struct Expr {
  int place;                                      // 値のある場所
  int value;                                      // 定数値やレジスタ番号等
};

// Expr を割り当てる
static struct Expr *newExpr(void) {
  struct Expr *e=ealloc(sizeof(struct Expr));     // 領域を割り当て
  e->place = INVAL;                               // 未使用の状態に初期化する
  e->value = 0;                                   //
  return e;
}

// 番号で管理されるラベルを出力する
static void printLab(int lab) {
  if (lab!=-1) vmTmpLab(lab);                     // -1 はラベル未割当てを表す
}

// 番号で管理されるSTRING用ラベルを出力する
static void printLabStr(int lab) {
  if (lab!=-1) vmTmpLabStr(lab);                  // -1 はラベル未割当てを表す
}

// 式の値をスタックにロードする
static void load(struct Expr *c) {
  int p = c->place;                               // 値のある場所
  int v = c->value;                               // 値の内容
  if (p != STKD) {                                // ロードされていないなら
    if      (p == CNST) vmLdCns(v);               //   定数値をロード
    else if (p == GVAR) vmLdGlb(v);               //   大域変数の値をロード
    else if (p == LVAR) vmLdLoc(v);               //   ローカル変数の値をロード
    else if (p == PRM)  vmLdPrm(v);               //   仮引数の値をロード
    else if (p == STR)  vmLdStr(v);               //   一時ラベル値をロード
    else if (p == LABL) vmLdLab(v);               //   通常ラベル値をロード
    else if (p == STKW) vmLdWrd();                //   ワード配列からロード
    else if (p == STKB) vmLdByt();                //   バイト配列からロード
    else error("バグ...load");                    //   それ以外ならエラー
    c->place = STKD;                              //   値はスタックにある
  }
}

// スタックトップの値を左辺式へストアする
static void store(struct Expr *c) {
  int p = c->place;                               // 左辺式の示す場所
  int v = c->value;                               // 左辺式の値
  if      (p == GVAR) vmStGlb(v);                 // 大域変数へストア
  else if (p == LVAR) vmStLoc(v);                 // ローカル変数へストア
  else if (p == PRM)  vmStPrm(v);                 // 仮引数へストア
  else if (p == STKW) vmStWrd();                  // ワード配列へストア
  else if (p == STKB) vmStByt();                  // バイト配列へストア
  else error("バグ...store");                     // その他はないはず
}

// スタックに積んだままなら捨てる
static void pop(struct Expr *c) {
  if (c->place==STKD) vmPop();                    // スタック利用なら解放
  if (c->place==STKW || c->place==STKB) {         // 意味のない式を書くと
    vmPop();                                      //   アドレスが残ることがある
    vmPop();
  }
}                                                 

// 再帰の都合でプロトタイプ宣言
static void genBoolExpr(int node, struct Expr* c);
static void genExpr(int node, struct Expr* c);

/// 関数の実引数をスタックに積む
static int genArgs(int node, struct Expr* c) {
  int n = 0;                                      // 引数の個数を数えるカウンタ
  while (node!=SyNULL) {                          // 引数がある間
    int typ  = syGetType(node);
    int exp;
    if (typ == SySEMI) {                          // リストなら
      exp  = syGetRVal(node);                     //   右側に式
      node = syGetLVal(node);                     //   次は左に進む
    } else {                                      // リスト以外(式)なら
      exp  = node;                                //   自身が式
      node = SyNULL;                              //   次は無い
    }
    genExpr(exp, c);                              // 引数を評価しスタックに積む
    vmArg();                                      // 引数値を引数領域に移動
    n = n + 1;                                    // 引数の個数をカウント
  }
  return n;                                       // 引数の個数を返す
}

// 因子
static void genFactor(int node, struct Expr* c) {
  int typ  = syGetType(node);
  int lVal = syGetLVal(node);
  if (typ == SyCNST) {                            // 定数の場合
    c->place = CNST;
    c->value = lVal;
  } else if (typ == SyGLB) {                      // 大域変数の場合
    c->place = GVAR;
    c->value = lVal;
  } else if (typ == SyLABL) {                     // 大域ラベルの場合
    c->place = LABL;
    c->value = lVal;
  } else if (typ == SyLOC) {                      // ローカル変数の場合
    c->place = LVAR;
    c->value = lVal;
  } else if (typ == SyPRM) {                      // 仮引数の場合
    c->place = PRM;
    c->value = lVal;
  } else if (typ == SySTR) {                      // 文字列ラベルの場合
    c->place = STR;
    c->value = lVal;
  } else if (typ == SyFUNC) {                     // 関数呼び出し
    int n = genArgs(syGetRVal(node), c);          //   引数の処理
    if (ntGetType(lVal)!=TyVOID||ntGetDim(lVal)!=0) { //   void 型以外なら
      vmCallF(n, lVal);                           //     関数用の Call
      c->place = STKD;
    } else {                                      //   void 型なら
      vmCallP(n, lVal);                           //     手続き用の Call
      c->place = INVAL;                           //     式は無効
    }
  } else error("バグ...genFactor");               // それ以外はないはず
}

// 単項演算
static void gen1OpExpr(int node, struct Expr* c) {
  genExpr(syGetLVal(node), c);                    // 式を評価しスタックに積む

  int typ = syGetType(node);                      // 演算の種類により命令を出力
  if      (typ == SyNEG)  vmNeg();                //     NEG  (2の補数)
  else if (typ == SyNOT)  vmNot();                //     NOT  (論理の否定)
  else if (typ == SyBNOT) vmBNot();               //     BNOT (1の補数)
  else if (typ == SyCHAR) vmChar();               //     CHR  (文字型への変換)
  else if (typ == SyBOOL) vmBool();               //     BOOL (論理型への変換)
  else error("バグ...gen1OpExpr");
}

// 可換な演算か
static boolean isCommutativeOp(int typ) {
  return                                          // 左辺と右辺を
    typ==SyADD  || typ==SyBAND || typ==SyBXOR ||  //   入れ換え可能な演算なら
    typ==SyBOR  || typ==SyMUL  || typ==SyGT   ||  //     true を返す
    typ==SyGE   || typ==SyLT   || typ==SyLE   ||
    typ==SyEQU  || typ==SyNEQ;
}

// node の演算に必要なスタックの深さ
static int evalDepth(int node) {
  int typ  = syGetType(node);                     // 演算の種類
  int lval = syGetLVal(node);                     // 左辺
  int rval = syGetRVal(node);                     // 右辺

  if (typ==SyFUNC) {                              // 関数コールは 100
    return 100;                                   //   (100 は無限大の代用)
  } else if (SyISFACT(typ)) {                     // 関数コール以外の因子
    return 1;                                     //   (値を１つ積む)
  } else if (SyIS1OPR(typ)) {                     // 単項演算は下位とおなじ
    return evalDepth(lval);                       //   (下位を計算して返す)
  } else if (SyISLOPR(typ)) {                     // && || 演算子なら
    int l = evalDepth(lval);                      //   左右の辺のスタック
    int r = evalDepth(rval);                      //     使用量を計算し
    if (l>r) return l;                            //   lが大きければ l
    return r;                                     //   rが大きか同じならrを返す
  } else if (isCommutativeOp(typ)) {              // 左右の辺が入換え可能なら
    int l = evalDepth(lval);                      //   左右の辺のスタック
    int r = evalDepth(rval);                      //     使用量を計算し
    if (l==r) return l+1;                         //   同じなら l + 1
    if (l>r) return l;                            //   l が大きければ l
    return r;                                     //   r が大きければ r を返す
  }
                                                  // 左右の辺を入換えできない
  int l = evalDepth(lval);                        //   左右の辺のスタック
  int r = evalDepth(rval);                        //     使用量を計算し
  if (l>r) return l;                              //   max(l, r+1) を返す
  return r + 1;                                   //   
}

// 二項演算の基本形 (数値演算に使用)
static void gen2OpExpr(int node, struct Expr* c) {
  int typ  = syGetType(node);                     // 演算の種類
  int lval = syGetLVal(node);                     // 左辺
  int rval = syGetRVal(node);                     // 右辺
  boolean swap = false;

  if (isCommutativeOp(typ) &&                     // 左右を可換な演算で右辺が
      evalDepth(lval) < evalDepth(rval)) {        //   多くのスタックを使うなら
    int tmp = lval;                               // 左右の辺を入れ換える
    lval = rval;
    rval = tmp;
    swap = true;
  }

  // 両辺を処理する
  genExpr(lval, c);                               // 左辺を評価し値をロード
  genExpr(rval, c);                               // 右辺を評価し値をロード

  if (typ==SyADD)       vmAdd();                  //     ADD    (足し算)
  else if (typ==SySUB)  vmSub();                  //     SUB    (引き算)
  else if (typ==SySHL)  vmShl();                  //     SHL    (左シフト)
  else if (typ==SySHR)  vmShr();                  //     SHR    (右シフト)
  else if (typ==SyBAND) vmBAnd();                 //     BAND   (ビット毎AND)
  else if (typ==SyBXOR) vmBXor();                 //     BXOR   (ビット毎XOR)
  else if (typ==SyBOR)  vmBOr();                  //     BOR    (ビット毎OR)
  else if (typ==SyMUL)  vmMul();                  //     MUL    (掛け算)
  else if (typ==SyDIV)  vmDiv();                  //     DIV    (割り算)
  else if (typ==SyMOD)  vmMod();                  //     MOD    (余り)
  else error("バグ...gen2OpExpr");
}

// 後置演算(配列アクセス)
static void genIdxExpr(int node, struct Expr* c, int place) {
  int lval = syGetLVal(node);                     // 左辺
  int rval = syGetRVal(node);                     // 右辺

  genExpr(lval, c);                               // 左辺を評価し値をロード
  genExpr(rval, c);                               // 右辺を評価し値をロード

  c->place=place;                                // ワード配列/バイト配列
}

// 後置演算(構造体アクセス)
static void genDotExpr(int node, struct Expr* c) {
  int lval = syGetLVal(node);                     // 構造体アドレス
  int rval = syGetRVal(node);                     //   定数ノードに名前表の
  int idx  = syGetLVal(rval);                     //     インデクスが入っている

  genExpr(lval, c);                               // 左辺を評価し値をロード

  // ２項演算に分類しているので式の体裁を整える
  c->place = CNST;                                // 定数値として
  c->value = ntGetCnt(idx);                       //   フィールドオフセットを
  load(c);                                        //     ロードする
  c->place = STKW;                                // ワード配列と同様な扱い
}

// 比較演算
static void genCmpExpr(int node, struct Expr* c) {
  int lval = syGetLVal(node);                     // 左辺
  int rval = syGetRVal(node);                     // 右辺
  boolean swap = false;

  if (evalDepth(lval) < evalDepth(rval)) {        // 右辺が左辺より
    int tmp = lval;                               //   多くスタックを使うなら
    lval = rval;                                  //     左右の辺を入れ換える
    rval = tmp;
    swap = true;
  }
    
  // 両辺を処理する
  genExpr(lval, c);                               // 左辺を評価し値をロード
  genExpr(rval, c);                               // 右辺を評価し値をロード

  int typ = syGetType(node);                      // 演算の種類により命令を出力
  if (swap) {                                     //   左右の辺が交換済みなら
    if      (typ==SyGT)  vmLt();                  //     GT( >  ) は LT( <  )
    else if (typ==SyGE)  vmLe();                  //     GE( >= ) は LE( <= )
    else if (typ==SyLT)  vmGt();                  //     LT( <  ) は GT( >  )
    else if (typ==SyLE)  vmGe();                  //     LE( <= ) は GE( >= )
    else if (typ==SyEQU) vmEq();                  //     EQ( == ) はそのまま
    else if (typ==SyNEQ) vmNe();                  //     NE( != ) はそのまま
    else error("バグ...genCmpExpr(1)");
  } else {
    if      (typ==SyGT)  vmGt();                  //     GT       ( >  )
    else if (typ==SyGE)  vmGe();                  //     GE       ( >= )
    else if (typ==SyLT)  vmLt();                  //     LT       ( <  )
    else if (typ==SyLE)  vmLe();                  //     LE       ( <= )
    else if (typ==SyEQU) vmEq();                  //     EQ       ( == )
    else if (typ==SyNEQ) vmNe();                  //     NE       ( != )
    else error("バグ...genCmpExpr(2)");
  }
}

/*
 * 論理式の処理
 */
struct Label {                                    // 論理式中のラベルを表現する
  int no;                                         // ラベルの番号(-1は未使用)
  int left;                                       // ラベルの左向きの深さ
};

// ラベルデータを割り当てる
static struct Label *newLabel(int left) {
  struct Label *l = ealloc(sizeof(struct Label)); //   領域を割り当て
  l->no   = -1;                                   //   未使用
  l->left = left;                                 //   ラベルが対応する左部分木
  return l;
}

// 条件ジャンプ命令の生成
static void boolJmp(boolean tf, struct Expr* c, struct Label *lab) {
  if (lab->no == -1) lab->no = newLab();          // 未割当てなら割当てる
  if (tf) vmJT(lab->no);                          //     JT lab
  else vmJF(lab->no);                             //     JF lab
}

// 論理演算子(&& と ||)の処理
static int genLogExpr(int node, struct Expr *c, int left,
		      struct Label *lab1, struct Label *lab2) {
  int curOp = syGetType(node);                    // 現在の演算子

  // 左辺を処理する
  int lNode = syGetLVal(node);                    // 左に進む場合のノード
  int lOp = syGetType(lNode);                     // 左の演算子
  if (lOp==SyOR || lOp==SyAND) {                  // 左に論理式が続く場合
    struct Label *labN = newLabel(left+1);        //   新しいラベルを準備
    if (lOp == curOp)                             //   同じ演算子なら
      genLogExpr(lNode, c, left+1, lab1, labN);   //     このように ^^;;;
    else                                          //   そうでなければ
      genLogExpr(lNode, c, left+1, labN, lab1);   //     このように ^^;;;
    printLab(labN->no);                           //   no が -1 以外なら印刷
    free(labN);
  } else {                                        // 左辺が論理式以外の場合
    genExpr(lNode, c);                            //   論理値を求め
    boolJmp(curOp==SyOR, c, lab1);                //   条件ジャンプ命令を生成
  }

  // 右辺を処理する
  int rNode = syGetRVal(node);                    // 右に進む場合のノード
  int rrNode = rNode;                             // 最後(最も右)の項
  int rOp = syGetType(rNode);                     // 右の演算子
  if (rOp==SyOR || rOp==SyAND) {                  // 右に論理式が続く場合
    if (rOp == curOp)                             //   同じ演算子なら
      rrNode=genLogExpr(rNode,c,left,lab1,lab2);  //     このように ^^;;;
    else                                          //   そうでなければ
      rrNode=genLogExpr(rNode,c,left,lab2,lab1);  //     このように ^^;;;
  } else if (left>=2) {                           // 式の最後の項以外なら
    genExpr(rNode, c);                            //   論理値を求め
    if (lab1->left == left)                       //   lab1 の直前の項なら
      boolJmp(curOp!=SyOR, c, lab2);              //   逆の条件で lab2 へ飛ぶ
    else
      boolJmp(curOp==SyOR, c, lab1);              //   条件ジャンプ命令を生成
  } // else { }              // 式の最後の項は特別なので呼び出し側で処理する
  return rrNode;
}

// genLogExpr を使用して論理値を求める
static void genLOpExpr(int node, struct Expr* c) {
  int op   = syGetType(node);                     // 論理和か論理積か
  int lab0 = newLab();                            // 論理式の最後
  struct Label *lab1 = newLabel(0);               // 途中で結果が出たとき
  struct Label *lab2 = newLabel(1);               // 途中で結果が出たとき

  int rNode = genLogExpr(node, c, 1, lab2, lab1); // コード生成
  genExpr(rNode, c);                              // 最後の項の論理値を求める

  if (op==SyOR)                                   // || なら
    vmBoolOR(lab2->no, lab1->no, lab0);           //   BOOLOR マクロ命令を生成
  else                                            // && なら
    vmBoolAND(lab2->no, lab1->no, lab0);          //   BOOLAND マクロ命令を生成

  free(lab1);
  free(lab2);
}

// 代入式
static void genAsExpr(int node, struct Expr* c) {
  genExpr(syGetRVal(node), c);                    // 右辺を評価しスタックに積む
  genBoolExpr(syGetLVal(node), c);                // 左辺(代入先)を評価する
  store(c);                                       //     ST  左辺
  c->place = STKD;                                // まだ値はスタックに残る
}

// カンマ演算
static void genCommExpr(int node, struct Expr* c) {
  genBoolExpr(syGetLVal(node), c);                // 左辺(捨てるのでBoolで良い)
  pop(c);                                         // スタックを利用なら解放
  genBoolExpr(syGetRVal(node), c);                // 右辺を評価
}

// 式のコード生成 (結果をロードしていない場合もある)
static void genBoolExpr(int node, struct Expr* c) {
  int op = syGetType(node);                       // 演算子を求める
  if      (op==SyASS)    genAsExpr(node, c);      //   代入演算
  else if (op==SyCOMM)   genCommExpr(node, c);    //   カンマ演算
  else if (SyISFACT(op)) genFactor(node, c);      //   因子
  else if (SyIS1OPR(op)) gen1OpExpr(node, c);     //   単項演算
  else if (op==SyIDXW)   genIdxExpr(node,c,STKW); //   ワード配列アクセス
  else if (op==SyIDXB)   genIdxExpr(node,c,STKB); //   バイト配列アクセス
  else if (op==SyDOT)    genDotExpr(node, c);     //   構造体アクセス'.'
  else if (SyIS2OPR(op)) gen2OpExpr(node, c);     //   普通の二項演算
  else if (SyISCMP(op))  genCmpExpr(node, c);     //   比較演算
  else if (SyISLOPR(op)) genLOpExpr(node, c);     //   論理演算
  else error("バグ...genBoolExpr");
}

// 式のコード生成 (結果を必ずスタックにロードする)
static void genExpr(int node, struct Expr* c) {
  genBoolExpr(node, c);                           // 式を評価する
  load(c);
}

// 再帰のためのプロトタイプ宣言
static void traceTree(int node);

// 条件式の生成(条件式(node)を評価し true/false なら lab へジャンプする)
static void genCnd(int node, boolean tf, struct Label *jmpLab) {
  struct Expr *c = newExpr();                     // Expr を割り当てる
  struct Label *nxtLab = newLabel(2);             //  式の直後用ラベル
  int op = syGetType(node);
  if (op==SyOR) {                                 // 条件式は論理和
    if (tf) {                                     // true で jmpLab へジャンプ
      genLogExpr(node, c, 2, jmpLab, nxtLab);     //  || 式が false 時に直後へ
    } else {                                      // false で jmpLab へジャンプ
      genLogExpr(node, c, 2, nxtLab, jmpLab);     //  || 式が true 時に直後へ
    }
  } else if (op==SyAND) {                         // 条件式は論理積
    if (tf) {                                     // true で jmpLab へジャンプ
      genLogExpr(node, c, 2, nxtLab, jmpLab);     //  && 式が true 時に直後へ
    } else {                                      // true で jmpLabへジャンプ
      genLogExpr(node, c, 2, jmpLab, nxtLab);     //  && 式が false 時に直後へ
    }
  } else {
    genExpr(node, c);                             // 条件式を評価する
    boolJmp(tf, c, jmpLab);                       // 条件ジャンプ命令を生成
  }
  printLab(nxtLab->no);                           // 使用されていればラベル生成
  free(nxtLab);                                   // Label を解放
  free(c);                                        // Expr を解放
}

// if 文
static void genIf(int node) {
  struct Label* ifLab = newLabel(0);              // ラベルを割り当てる
  genCnd(syGetLVal(node), false, ifLab);          // 条件式がfalseならジャンプ
  traceTree(syGetRVal(node));                     // 本文の解析
  printLab(ifLab->no);
  free(ifLab);
}

// 文、ブロックの最後が JMP 命令で終わるか	
static boolean jmpBlock(int node) {
  if (node==SyNULL) return false;                 // 本文が空の場合
  int typ = syGetType(node);
  if (typ==SyCNT || typ==SyRET || typ==SyBRK)     // JMP が生成される命令
    return true;
  if (typ==SyBLK)                                 // ブロックなら
    return jmpBlock(syGetRVal(node));             //   JMP が最後に生成されるか
  return false;
}

// if-else 文
static void genEls(int node) {
  struct Label* elsLab = newLabel(0);             // ラベルを割り当てる
  int ifNode = syGetLVal(node);                   // if 文部分
  genCnd(syGetLVal(ifNode), false, elsLab);       // 条件式がfalseならジャンプ
  traceTree(syGetRVal(ifNode));                   // 本文の解析
  int ifLab = -1;
  if (!jmpBlock(syGetRVal(ifNode))) {
    ifLab = newLab();                             // ラベルを割り当てる
    vmJmp(ifLab);                                 // if 文の後ろにジャンプ
  }
  printLab(elsLab->no);                           // else 節
  traceTree(syGetRVal(node));                     // else 節の本文解析
  printLab(ifLab);
  free(elsLab);
}

// while 文 と for 文
static void genWhl(int node) {
  int tmpLabB = brkLab;                           // ラベルを保存
  int tmpLabC = cntLab;
  int loopLab = newLab();                         // 条件式のラベル
  int sta = syGetRVal(node);                      // 本文 + 再初期化
  boolean jb = jmpBlock(syGetLVal(sta));          // 本文は JMP で終わる
  cntLab = -1;                                    // continue 時のジャンプ先
  brkLab = -1;                                    // break 時のジャンプ先
  if (syGetRVal(sta)==SyNULL) cntLab = loopLab;   // 再初期化がない場合
  printLab(loopLab);                              // ループ先頭のラベル
  if (syGetLVal(node)!=SyNULL) {                  // 条件式がある場合
    struct Label* bLab = newLabel(0);             //   ループ脱出用ラベル
    genCnd(syGetLVal(node), false, bLab);         //   falseならジャンプ
    brkLab = bLab->no;                            //   break 時にも使用する
    free(bLab);
  }
  traceTree(syGetLVal(sta));                      // 本文の解析
  if (syGetRVal(sta)!=SyNULL &&                   // 再初期化がある場合
      (!jb || cntLab!=-1)) {                      //   再初期化に到達するなら
    printLab(cntLab);                             //   continue 時のジャンプ先
    traceTree(syGetRVal(sta));                    //   再初期化を解析
    jb = false;                                   //   再初期化はJMPで終わらない
  } 
  if (!jb) vmJmp(loopLab);                        // 条件式に戻る
  printLab(brkLab);                               // break 時のジャンプ先
  cntLab = tmpLabC;
  brkLab = tmpLabB;
}

// do-while 文
static void genDo(int node) {
  int tmpLabB = brkLab;                           // ラベルを保存
  int tmpLabC = cntLab;
  int loopLab = newLab();                         // 先頭のラベル
  struct Label *lLab = newLabel(0);
  lLab->no = loopLab;
  cntLab  = -1;                                   // 条件式のラベル
  brkLab  = -1;                                   // ループ脱出時のラベル
  printLab(loopLab);
  traceTree(syGetLVal(node));                     // 本文の解析
  printLab(cntLab);
  genCnd(syGetRVal(node), true, lLab);            // 条件式 true ならジャンプ
  printLab(brkLab);
  cntLab = tmpLabC;
  brkLab = tmpLabB;
  free(lLab);
}

// break 文
static void genBrk(int node) {
  if (brkLab==-1) brkLab = newLab();              // 必要ならラベルを割り当てる
  vmJmp(brkLab);                                  //   JMP brkLab
}

// continue 文
static void genCnt(int node) {
  if (cntLab==-1) cntLab = newLab();              // 必要ならラベルを割り当てる
  vmJmp(cntLab);                                  //   JMP cntLab
}

// return 文
static void genRet(int node) {
  if (syGetLVal(node)!=SyNULL) {                  // 返す値があれば
    struct Expr *c = newExpr();                   //   Expr を割り当て
    genExpr(syGetLVal(node), c);                  //   返す式を処理し値をロード
    vmMReg();                                     //     ハードウェアレジスタに
    free(c);                                      //       移動する
  }
  int root = syGetRoot();
  if (root!=node && syGetRVal(root)!=node) {      // 関数の最後の文でないなら
    if (retLab==-1) retLab = newLab();            //   ラベルを割り当て
    vmJmp(retLab);                                //   そこにジャンプする
  }
}

// 構文木をトレースする
static void traceTree(int node) {
  if (node==SyNULL) return;                       // 何も無い
  int ty = syGetType(node);
  if (ty==SyIF) genIf(node);                      // if 文
  else if (ty==SyELS) genEls(node);               // if-else 文
  else if (ty==SyWHL) genWhl(node);               // while 文
  else if (ty==SyDO)  genDo(node);                // do-while 文
  else if (ty==SyBRK) genBrk(node);               // break 文
  else if (ty==SyCNT) genCnt(node);               // continue 文
  else if (ty==SyRET) genRet(node);               // retrun 文
  else if (ty==SyBLK || ty==SySEMI) {             // ブロック
    traceTree(syGetLVal(node));                   //   先に左側をコード生成
    traceTree(syGetRVal(node));                   //   次に右側をコード生成
  } else if (ty==SyVAR) {                         // ローカル変数宣言
    ;                                             //   特にやることがない
  } else {                                        // 式文
    struct Expr *c = newExpr();                   //   式の状態を管理するデータ
    genBoolExpr(node, c);                         //   式を処理する
    pop(c);                                       //   スタックにあれば捨てる
    free(c);                                      //   管理データを解放
  }
}    

// 関数１個分のコード生成
void genFunc(int funcIdx, int depth, boolean krnFlg) {
  if (ntGetType(funcIdx)==TyINTR) {              // 割込み関数の場合は
    vmEntryI(depth, funcIdx);                    //   割込み関数用の Entry
  } else if (krnFlg) {                           // カーネルモードなら
    vmEntryK(depth, funcIdx);                    //   カーネル用の Entry
  } else {                                       // そうでなければ
    vmEntry(depth, funcIdx);                     //   通常の Entry
  }

  retLab = -1;                                   // return 用のラベル

  int root = syGetRoot();                        // 構文木の根を取り出す
  traceTree(root);                               // 根から木をトレースする

  printLab(retLab);                              // return 用のラベルが必要なら

  if (ntGetType(funcIdx)==TyINTR)                // 割込み関数の場合は
    vmRetI();                                    //   RETI
  else
    vmRet();                                     //   RET
}

/*
 * データのコード生成
 */
// 単純なデータの生成
static void genDW(int node) {
  int typ  = syGetType(node);
  int lVal = syGetLVal(node);
  int rVal = syGetRVal(node);

  if (typ==SyLIST || typ==SyARRY) {              // 配列・構造体へのポインタ
    vmDwLab(rVal);                               //   DW .Ln を出力
  } else if (typ==SySTR) {                       // 文字列
    vmDwLabStr(lVal);                            //   DW .Ln を出力
  } else {                                       // 初期化単純変数
    typ  = syGetType(node);                      //   計算後の状態へ変更
    lVal = syGetLVal(node);                      //
    if      (typ==SyLABL) vmDwName(lVal);        //   DW  name を出力
    else if (typ==SyCNST) vmDwCns(lVal);         //   DW  N  を出力
    else error("バグ...genDW");                  // 定数でなければエラー
  }
}

// 非初期化配列[array(n1, n2, ...)] のコードを生成する
static int genArray0(int node, int n, int d) {
  int typ  = syGetType(node);
  int rVal = syGetRVal(node);
  int lVal = syGetLVal(node);
  int lab = -1;                                  // この次元の最初のラベル

  if (typ==SySEMI) {                             // 最後の次元でなければ
    if (syGetType(rVal)!=SyCNST)                 // 定数でなければエラー
      error("バグ...genArray0_1");
    int cnt = syGetLVal(rVal);                   //   現在の配列サイズを調べ
    int ln = genArray0(lVal, n*cnt, d-1);        //   入れ子の配列を生成し
    for (int i=0; i<n; i=i+1) {                  //   前の次元の要素数分の
      int l = newLab();                          //     ラベルを生成し
      if (lab==-1) lab = l;                      //     この次元の最初を記憶し
      printLab(l);                               //     .Ln1 DW .Ly1
      for (int j=0; j<cnt; j=j+1) {              //          DW .Ly1
	vmDwLab(ln);                             //          ...
	ln = ln + 1;                             //     を生成する
      }
    }
  } else if (typ==SyCNST) {                      // 最後の次元なら
    for (int i=0; i<n; i=i+1) {                  //   前の次元の要素数分の
      int l = newLab();                          //     ラベルを生成し
      if (lab==-1) lab = l;                      //     この次元の最初を記憶し
      printLab(l);                               //
      if (d==1)                                  //     １次元バイト配列なら
	vmBs(lVal);                              //       .Ln BS N を生成
      else                                       //     そうでなければ
	vmWs(lVal);                              //       .Ln WS N を生成する
    }
  } else error("バグ...genArray0_2");            // 定数でなければエラー
  return lab;
}

static void genArray(int node) {                 // array(n1, n2, ...) を処理
  int lVal = syGetLVal(node);                    // n1, n2, ...
  int rVal = syGetRVal(node);                    // バイト配列の場合次元
  int lab = genArray0(lVal, 1, rVal);            // コード生成
  sySetRVal(node, lab);
}

// 配列の初期化コード生成
static void genList0(int node, int dim, boolean byt) {
  int typ  = syGetType(node);
  int lVal = syGetLVal(node);
  int rVal = syGetRVal(node);

  if (typ==SySEMI) {                             // リストの途中
    genList0(lVal, dim, byt);                    //   左から
    genList0(rVal, dim, byt);                    //   右の順番で辿る
  } else if (dim==0) {                           // コード生成すべき深さ
    if (byt) {                                   //   バイト配列の末端
      if (syGetType(node)!=SyCNST)
        error("バグ...genList0");             //     定数だけ OK
      vmDbCns(syGetLVal(node));                  //     DB XX を生成
    } else {                                     //   ワード配列(ポインタ含む)
      genDW(node);                               //     DW XX を生成
    }
  } else if (typ==SyLIST) {                      // 初期化配列を発見
    if (dim==1) {                                //   コード生成直前
      int lab = newLab();                        //     配列の先頭に
      sySetRVal(node, lab);                      //     ラベルを生成し
      printLab(lab);                             //     rVal に記録
    }
    genList0(lVal, dim-1, rVal==1);              //   配列の本体を辿る
  } else if (typ==SyARRY) {                      // 非初期化配列が含まれていた
    if (dim==1) genArray(node);                  //   非初期化配列生成
  }
}

static void genList(int node, int dim) {         // 開始位置と配列の次元、型
  dim = dim + 1;                                 // 構造体も配列と同じ扱い
  while (dim>=1) {                               // 深い方(末端)から順に
    genList0(node, dim, false);                  //   コードを生成する
    dim = dim - 1;
  }
}

// 初期化データの生成
void genData(int idx) {
  int root = syGetRoot();
  int typ  = syGetType(root);
  int dim  = ntGetDim(idx);

  if (typ==SyLIST) genList(root,dim);            // 初期化された配列
  else if (typ==SyARRY) genArray(root);          // 非初期化配列[array(n1,...)]

  vmName(idx);                                   // 次のような出力をする
  genDW(root);                                   //   Name  DW xx
}

// 非初期化データの生成
void genBss(int idx) {                           // 次のような出力をする
  vmName(idx);                                   //   Name  WS 1
  vmWs(1);
}

// 文字列を生成しラベル番号を返す
int genStr(char *str) {
  int lab = newLabStr();                            // ラベルを割り付け
  printLabStr(lab);
  vmStr(str);                                    //   .Ln STRING "xxxx" を出力
  return lab;                                    //   ラベル番号を返す
}
