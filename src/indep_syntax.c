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
 * syntax.c : C--コンパイラの構文解析ルーチン(意味解析を除く)
 *
 * 2016.08.02         : watch構造体の除去
 * 2016.07.29         : 新規作成
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "util.h"                   // その他機能モジュール
#include "lexical.h"                // 字句解析モジュール
#include "optree.h"                 // 構文木最適化モジュール
#include "code.h"                   // コード生成モジュール
#include "namtbl.h"                 // 名前表モジュール
#include "sytree.h"                 // 構文木モジュール
#include "semantic.h"               // 意味解析モジュール
#include "syntax.h"

/*
 * グローバルデータ
 */
static char   *curName;             // 最近登録した名前のつづり
static int     curScope;            // 現在のスコープ
static int     curType;             // 最近読み込んだ型
static int     curDim;              // 最近読み込んだ型の配列次元数
static int     curCnt;              // 最近読み込んだローカル変数や引数、
                                    //   構造体フィールドの番号
static boolean pubFlag;             // public 修飾子を読んだ
static int     funcIdx;             // 現在読込中の関数

static boolean optFlag = true;      // 最適化を行う
static boolean krnFlag = false;     // カーネルコンパイルモード

//-----------------------------------------------------------------------------
// トークンの読み込みはコンパイラ版とトランスレータ版で処理が異なる。
//-----------------------------------------------------------------------------
#ifndef C
// コンパイラ版はディレクティブに興味がないので
// ディレクティブは getTok() が読み飛ばす。
#define _tok tok                             // _tok と tok の区別はない
static int tok;                              // 次のトークン

static void getTok() {                       // ディレクティブ以外を入力する
  do {                                       //
    tok = lxGetTok();                        // 次のトークンを入力する
  } while (tok==LxFILE);                     // ディレクティブならやり直し
}
//-----------------------------------------------------------------------------
#else
// トランスレータ版ではディレクティブも処理対象になる場合がある。
// ディレクティブを処理したい時は _tok を使用する。
// ディレクティブに興味がない処理は tok を使用する。
// tok を使用すると _getTok() が呼ばれディレクティブを読み飛ばす。
static int _tok;                             // 次のトークン

static void getTok() {                       // 次のトークンを入力する
  _tok = lxGetTok();                         //   ディレクティブも入力する
}

#define tok _getTok()                        // tok 使用は、_getTok() に置換え
static int _getTok() {                       // tok 使用時に
  while (_tok==LxFILE)                       //   ディレクティブを読み飛ばす
    _tok = lxGetTok();                       //   ディレクティブを
  return _tok;
}
//-----------------------------------------------------------------------------
#endif

/*
 * 構文解析
 */
// 次のトークンを調べ予想通りなら読み捨てる、予想との比較結果true/falseを返す
static boolean isTok(int expTok) {
  if (tok==expTok) {
    getTok();                                    // 予想通りなら読み捨てた
    return true;                                 //   上で true を返す
  }
  return false;                                  // 予想と異なるとfalseを返す
}

// 次のトークンを調べ予想通りなら読み捨てる、そうでなければエラーにする
static void chkTok(int expTok, char *msg) {
  if (!isTok(expTok)) error(msg);                // 予想と異なるとエラー
}

// 型か調べ、curType を書換える．型の場合はtrue、型ではない場合はfalseを返す
static boolean isType(void) {
  curType=TyNON;                                 // 取り合えず型ではない
  if      (tok==LxINT ) curType=TyINT;           // int 型
  else if (tok==LxCHAR) curType=TyCHAR;          // char 型
  else if (tok==LxVOID) curType=TyVOID;          // void 型
  else if (tok==LxBOOL) curType=TyBOOL;          // boolean 型
  else if (tok==LxINTR) curType=TyINTR;          // interrupt 型
  else if (tok==LxNAME)                          // 名前なら構造体型かも
    curType=ntSrcStruct(lxGetStr());             //   構造体名を検索する

  return curType!=TyNON;                         // TyNON 以外なら型だった
}

// 型名と配列を表す '[]' を読み込む
static void getType(void) {
  if (!isType()) error("型がない");              // 型かどうか調べる
  getTok();                                      // 型名を読み飛ばす
  for (curDim=0; isTok('['); curDim=curDim+1)    // 配列型なら次元を増やす
    chkTok(']',"配列宣言に ']' が不足");
}

// 名前を読み込み、表に登録する(引数は public 名の意味)
static void getName(boolean pub) {
  chkTok(LxNAME, "名前がない");                          // 名前か調べる
  if (curScope!=ScPROT && curType==TyVOID && curDim==0)  // 関数以外でvoid型は
    error("void型変数/引数は使用できない");              //   ポインタだけOK
  if (curScope!=ScPROT && curType==TyINTR)               // 関数以外でintrrupt
    error("interrupt型変数/引数は使用できない");         //   型は使用できない
  if (curType==TyINTR && !krnFlag)                       // カーネルのみintrrupt
    error("interrupt型はカーネルのみで使用可");          //   型は使用できる
  ntDefName(lxGetStr(), curScope, curType,               // 名前表に登録する
	    curDim, curCnt, pub);
  curName = ntGetName(ntGetSize()-1);                    // 最後に登録した名前
}

/*
 * 構造体の宣言
 */
// 構造体フィールド１行を読み込む
static void getFieldLine(void) {
  getType();                                    // 型を読み込む
  getName(false);                               // 名前を読み込んで表に登録
  curCnt = curCnt + 1;                          // フィールドの番号
  while (isTok(',')) {                          // ','が続く間
    getName(false);                             //    名前を読み込んで表に登録
    curCnt = curCnt + 1;                        // フィールドの番号
  }
  chkTok(';', "構造体フィールドの宣言が ';' で終わっていない");
}

// 構造体宣言を読み込む
static void getStruct(void) {
  chkTok('{', "構造体宣言に '{' がない");
  int structIdx=ntGetSize();                   // 構造体宣言は関数の外にあるの
  curScope = ScLVAR;                           //   でフィールド名の衝突チェッ
  curCnt   = 0;                                //   クは局所変数と同様でよい
  getFieldLine();                              // フィールド宣言1行分を読む
  while (!isTok('}'))                          // '}'以外の間
    getFieldLine();                            //   フィールド宣言1行分を読む
  chkTok(';', "構造体宣言が ';' で終わっていない");
  for (int i=structIdx; i<ntGetSize(); i=i+1)  // 名前の衝突チェックが終わった
    ntSetScope(i, ScVOID);                     //   のでフィールドのscopeに変更
  ntSetCnt(structIdx-1,ntGetSize()-structIdx); // フィールド数を表に記録
#ifdef C
  genStruc(structIdx-1);                       // 構造体宣言を出力
#endif                                         // (トランスレータ版だけで必要)
}

static int getAsExpr(void);        // 再帰呼出の関係で
static int getExpr(void);          //   プロトタイプ宣言が必要

// 後置演算子(添字('[n]'))の処理
static int getIdxOP(int tree) {
  int tree2 = getAsExpr();                     // 添字式はカンマ式不可
  chkTok(']', "']' がない");
  return syNewNode(SyIDXW, tree, tree2);       // とりあえずIDXWにしておく
}                                              //   WとBは意味解析で判定する

// 後置演算子(構造体フィールド('.'))の処理
static int getDotOP(int tree) {
  chkTok(LxNAME, "'.' の次に名前がない");      // '.'の次は必ず名前が必要
  int n = ntSrcFieldName(lxGetStr());          // 構造体をフィールド名でサーチ
  int cnst = syNewNode(SyCNST, n, TyINT);      // フィールドの名前表インデクス
  return syNewNode(SyDOT, tree, cnst);         // (名前のみ合致する別物も有り得る)
}

// 後置演算子(添字('[n]')と構造体フィールド('.'))の処理
static int getPostOP(int tree1) {
  int tree2 = tree1;
  while (true) {                               // 後置演算子が続く間繰り返す
    if (isTok('['))
      tree2 = getIdxOP(tree2);             // '[n]' の処理
    else if (isTok('.'))
      tree2 = getDotOP(tree2);             // '.' の処理
    else break;
  }
  return tree2;
}

// 実引数を読み込む
static int getArgs(int func) {                  // funcは表の現在の関数を指す
  int idx = func + 1;                              // 仮引数を指すインデクス
  int list = SyNULL;                               // 引数リストを作る
  int tree;
  if (tok!=')') {                                  // 引数が存在するなら
    do {                                           //
      tree = getAsExpr();                                // 引数の式(w)(カンマ式不可)
      list = syCatNode(list, tree);             // 引数リストに追加
      idx=idx+1;                                   //     インデクスを進める
    } while (isTok(','));                          // ','が続く間繰り返す
  }
  chkTok(')', "関数呼出に ')' がない");
  return list;                                  // 引数リストを返す
}

// 関数呼出
static int getCall(int n) {   // n は名前表で関数の位置
  chkTok('(',"関数名の次に '(' がない");
  int tree = getArgs(n);                     // 実引数を読み込む
  int s = syNewNode(SyFUNC, n, tree);        // 関数を表現するノード
  return s;
  //setWatch(w,ntGetType(n),ntGetDim(n),s);       // 関数の値は変更不可
}

// sizeof 演算子
static int getSizeof(void) {
  chkTok('(', "sizeof に '(' が続かない");
  int ty=curType, dm=curDim;                  // getType が壊すので保存し、
  getType();                                  // 型を読む
  if (curType<=0&&ntGetType(-curType)==TyREF) // typedef なら
    error("typedefされた型はsizeofで使用できない");
#ifdef C                                      // トランスレータは sizeof を
  int a = syNewNode(SySIZE, curType, curDim); //   C言語ソースに出力する
#else                                         // コンパイラは sizeof を計算する
  int s = NWORD / 8;                          //   INT またはポインタのサイズ
  if (curDim==0) {                            //   配列以外で
    if (curType<=0)                           //     構造体の場合は
      s = s * ntGetCnt(-curType);             //       フィールド数×INTのサイズ
    else if (curType==TyCHAR ||
	     curType==TyBOOL)                 //     char, boolean なら
      s = NBYTE / 8;                          //       バイトのサイズ
  }
  int a = syNewNode(SyCNST, s, TyINT);        //   サイズを格納するノード
#endif
  curType=ty;                                 // 保存したものをもとに戻す
  curDim = dm;
  chkTok(')', "sizeof が ')' で終わらない");
  return a;
}

// addrof 演算子
static int getAddrof(void) {
  chkTok('(', "addrof に '(' が続かない");
  chkTok(LxNAME, "addrof( に名前が続かない");
  int n = ntSrcName(lxGetStr());              // 表から名前を探し
  int a = syNewNode(SyLABL, n, SyNULL);       //   ラベル定数を表現するノード
  chkTok(')', "addrof が ')' で終わらない");
  return a;
}

// ord 演算子
static int getOrd(void) {
  chkTok('(', "ord に '(' が続かない");
  int tree = getExpr();                       // 括弧内の式を読み込む
  chkTok(')', "ord が ')' で終わらない");
  tree = syNewNode(SyORD, tree, SyNULL);
  return tree;
}

// chr 演算子
static int getChr(void) {
  chkTok('(', "chr に '(' が続かない");
  int tree = getExpr();                       // 括弧内の式を読み込む
  chkTok(')', "chr が ')' で終わらない");
  tree = syNewNode(SyCHAR, tree, SyNULL);
  return tree;
}

// bool 演算子
static int getBool(void) {
  chkTok('(', "bool に '(' が続かない");
  int tree = getExpr();                       // 括弧内の式を読み込む
  chkTok(')', "bool が ')' で終わらない");
  tree = syNewNode(SyBOOL, tree, SyNULL);
  return tree;
}

// 関数呼出しか変数（登録済みの名前で始まる）
static int getIdent(void) {
  int tree=0;
  int n = ntSrcName(lxGetStr());              // 表から名前を探し
  int s = ntGetScope(n);                      //   それのスコープを調べ
  //int t = ntGetType(n);                       //   それの型を調べ、
  //int d = ntGetDim(n);                        //   それの配列次元数を調べ、
  int c = ntGetCnt(n);                        //   それの番号を調べる
  if (s==ScPROT || s==ScFUNC) {               // 関数名の場合は、
    tree = getCall(n);                        //   関数呼び出しの処理
  } else if (s==ScCOMM || s==ScGVAR) {        // 大域変数の場合
    tree = syNewNode(SyGLB, n, SyNULL);      //   大域変数を表現するノード
    //setWatch(w, t, d, a);               //   式(w)が大域変数になる
  } else if (s>=ScLVAR) {                     // ScLVAR の場合は
    if (c>0) {                                //   c>0 なら局所変数
      tree = syNewNode(SyLOC, c, SyNULL);    //     局所変数のノード
      //setWatch(w, t, d, a);             //     式(w)が局所変数になる
    } else {                                  //   c<0 なら仮引数
      tree = syNewNode(SyPRM,-c, SyNULL);    //     仮引数のノード
      //setWatch(w, t, d, a);             //     式(w)が仮引数になる
    }
  } else error("バグ...getIdent");            // それ以外の名前はあり得ない
  return tree;
}

// 因子
static int getFactor(void) {
  int tree=0;
  if (isTok('(')) {                           // '(' の場合は
    tree = getExpr();                         // 括弧内の式を読み込む
    chkTok(')', "')' が不足");
  } else if (isTok(LxNAME)) {                 // 名前の場合
    tree = getIdent();                        //   関数呼び出しか変数
  } else if (isTok(LxINTEGER)) {              // 整数定数の場合は
    tree = syNewNode(SyCNST, lxGetVal(), TyINT);  // 定数を格納するノード
  } else if (isTok(LxCHARACTER)) {            // 文字定数の場合は
    tree = syNewNode(SyCNST, lxGetVal(), TyCHAR); // 定数を格納するノード
  } else if (isTok(LxLOGICAL)) {              // 論理値の場合は
    tree = syNewNode(SyCNST, lxGetVal(), TyBOOL); // 定数を格納するノード
  } else if (isTok(LxSTRING)) {               // 文字列の場合は
    int lab = genStr(lxGetStr());             // 文字列を出力しラベルを付ける
    tree = syNewNode(SySTR, lab, SyNULL);     //    ラベル番号を格納するノード
  } else if (isTok(LxNUL)) {                  // null の場合は
    tree = syNewNode(SyCNST, 0, TyREF);       //   NULL を格納するノード
  } else if (isTok(LxSIZEOF)) {               // sizeof 演算子の場合
    tree = getSizeof();
  } else if (isTok(LxADDROF)) {               // addrof 演算子の場合
    tree = getAddrof();
  } else if (isTok(LxORD)) {                  // ord 演算子の場合
    tree = getOrd();
  } else if (isTok(LxCHR)) {                  // chr 演算子の場合
    tree = getChr();
  } else if (isTok(LxBOL)) {                  // bool 演算子の場合
    tree = getBool();
  } else                                      // それ以外は因子にならない
    error("式がない/式が突然終わっている/C--で使用できない演算子");
  return getPostOP(tree);                     // 因子の後半 '.'や'[]'の処理
}

// 単項演算
static int getUniExpr(void) {
  int tree;
  if (isTok('+')) {                           // '+' の場合は
    tree = getUniExpr();                      //   まず、式(w)を読み込み
    tree = syNewNode(SyPLS, tree, SyNULL);
  } else if (isTok('-')) {                    // '-' の場合は
    tree = getUniExpr();                      //   まず、式(w)を読み込み
    tree = syNewNode(SyNEG, tree, SyNULL);
  } else if (isTok('!')) {                    // '!'(論理の否定)の場合は
    tree = getUniExpr();                      //   まず、式(w)を読み込み
    tree = syNewNode(SyNOT, tree, SyNULL);
  } else if (isTok('~')) {                    // '~'(1の補数)の場合は
    tree = getUniExpr();                      //   まず、式(w)を読み込み
    tree = syNewNode(SyBNOT, tree, SyNULL);
  } else tree = getFactor();                  // どれでもなければ因子
  return tree;
}

// 2項演算の木を作る
static int cal2(int op, int tree1, int tree2) {
  int tree = syNewNode(op, tree1, tree2);
  return tree;
}

// 掛算，割算
static int getMulExpr(void) {
  int tree1 = getUniExpr();                    // 左辺(w)を読み込む
  while (true) {
    int op;
    if      (isTok('*')) op=SyMUL;             // '*' か
    else if (isTok('/')) op=SyDIV;             // '/' か
    else if (isTok('%')) op=SyMOD;             // '%' が続く間
    else break;
    int tree2 = getUniExpr();                  // 右辺(tree2)を読み込む
    tree1 = cal2(op, tree1, tree2);            // 2項演算の木を作る(tree2解放)
  }
  return tree1;
}

// 足算，引算
static int getAddExpr(void) {
  int tree1 = getMulExpr();                    // 左辺(w)を読み込む
  while (true) {
    int op;
    if      (isTok('+')) op = SyADD;           // '+' か
    else if (isTok('-')) op = SySUB;           // '-' が続く間
    else break;
    int tree2 = getMulExpr();                  // 右辺(tree2)を読み込む
    tree1 = cal2(op, tree1, tree2);            // 2項演算の木を作る(tree2解放)
  }
  return tree1;
}

// シフト演算
static int getSftExpr(void) {
  int tree1 =  getAddExpr();                   // 左辺(w)を読み込む
  while (true) {
    int op;
    if      (isTok(LxSHL)) op = SySHL;         // '<<' か
    else if (isTok(LxSHR)) op = SySHR;         // '>>' が続く間
    else break;
    int tree2 = getAddExpr();                  // 右辺(tree2)を読み込む
    tree1 = cal2(op, tree1, tree2);            // 2項演算の木を作る(tree2解放)
  }
  return tree1;
}

// 大小比較演算
static int getCmpExpr(void) {
  int tree1 = getSftExpr();                    // 左辺(w)を読み込む
  while (true) {
    int op;
    if      (isTok('<'))   op = SyLT;          // '<'  か
    else if (isTok(LxLTE)) op = SyLE;          // '<=' か
    else if (isTok('>'))   op = SyGT;          // '>'  か
    else if (isTok(LxGTE)) op = SyGE;          // '>=' が続く間
    else break;
    int tree2 = getSftExpr();                  // 右辺(tree2)を読み込む
    tree1 = cal2(op, tree1, tree2);            // 2項演算の木を作る(tree2解放)
  }
  return tree1;
}

// 比較演算
static int getEquExpr(void) {
  int tree1 = getCmpExpr();                    // 左辺(w)を読み込む
  while (true) {
    int op;
    if      (isTok(LxEQU)) op = SyEQU;         // '==' か
    else if (isTok(LxNEQ)) op = SyNEQ;         // '!=' が続く間
    else break;
    int tree2 = getCmpExpr();                  // 右辺(tree2)を読み込む
    tree1 = syNewNode(op, tree1, tree2);       // 新しいノードを作る
  }
  return tree1;
}

// ビット毎の論理積
static int getAndExpr(void) {
  int tree1 = getEquExpr();                    // 左辺(w)を読み込む
  while (isTok('&')) {                         // '&' が続く間
    int tree2 = getEquExpr();                  // 右辺(tree2)を読み込む
    tree1 = cal2(SyBAND, tree1, tree2);        // 2項演算の木を作る(tree2解放)
  }
  return tree1;
}

// ビット毎の排他的論理和
static int getXorExpr(void) {
  int tree1 = getAndExpr();                    // 左辺(w)を読み込む
  while (isTok('^')) {                         // '^' が続く間
    int tree2 = getAndExpr();                  // 右辺(tree2)を読み込む
    tree1 = cal2(SyBXOR, tree1, tree2);        // 2項演算の木を作る(tree2解放)
  }
  return tree1;
}

// ビット毎の論理和
static int getOrExpr(void) {
  int tree1 = getXorExpr();                    // 左辺(w)を読み込む
  while (isTok('|')) {                         // '|' が続く間
    int tree2 = getXorExpr();                  // 右辺を読み込む
    tree1 = cal2(SyBOR, tree1, tree2);         // 2項演算の木を作る(tree2解放)
  }
  return tree1;
}

// 論理積
static int getLogAndExpr(void) {
  int tree1 = getOrExpr();                     // 左辺(w)を読み込む
  while (isTok(LxAND)) {                       // '&&' が続く間
    int tree2 = getOrExpr();                   // 右辺を読み込む
    tree1 = cal2(SyAND, tree1, tree2);         // 2項演算の木を作る(tree2解放)
  }
  return tree1;
}

// 論理和
static int getLogOrExpr(void) {
  int tree1 = getLogAndExpr();                 // 左辺(w)を読み込む
  while (isTok(LxOR)) {                        // '||' が続く間
    int tree2 = getLogAndExpr();               // 右辺を読み込む
    tree1 = cal2(SyOR, tree1, tree2);          // 2項演算の木を作る(tree2解放)
  }
  return tree1;
}

// 代入式
static int getAsExpr(void) {
  int tree1 = getLogOrExpr();                  // 左辺式(w)を読み込む
  if (isTok('=')) {                            // '=' があるなら
    int tree2 = getAsExpr();                   // 右辺(tree2)を読み込む
    tree1 = syNewNode(SyASS, tree1, tree2);
  }
  return tree1;
}

// カンマ式
static int getExpr(void) {
  int tree1 = getAsExpr();                     // 左辺(w)を読み込む
  while (isTok(',')) {                         // ',' があるなら
    int tree2 = getAsExpr();                   // 右辺(w)を読み込む
    tree1 = syNewNode(SyCOMM, tree1, tree2);
  }
  return tree1;
}

/*
 * ローカル変数の処理
 */
// ローカル変数の宣言を読み込む
static int maxCnt;                             // 関数内の最大スタック使用量

static int getLVar(void) {
  curCnt = curCnt + 1;                         // ローカル変数の番号
  getName(false);                              // 変数名を読み込み表に登録
  int dec = syNewNode(SySEMI,curType,curDim);  // 変数の型
  dec = syNewNode(SyVAR, curCnt, dec);         // 変数宣言
  if (isTok('=')) {                            // '='があれば、初期化がある
    int sta = syNewNode(SyLOC,curCnt,SyNULL);  // 初期化の左辺を作る
    int tree = getAsExpr();                    // 初期化式にはカンマ式不可
    sta = syNewNode(SyASS, sta, tree);         // 左辺と右辺を接続
    dec = syCatNode(dec, sta);                 // 宣言と初期化を接続
  }
  return dec;
}

// ローカル変数の宣言並びを読み込む
static int getLVars(void) {
  getType();                                   // 型を読み込む
  int sta = getLVar();                         // 名前 [ = 式 ] を読み込む
  while (isTok(','))                           // ',' が続く間
    sta = syCatNode(sta, getLVar());           //   名前 [ = 式 ] を読み込む
  if (curCnt > maxCnt) maxCnt = curCnt;        // もっとも大きな番号を記録
  return sta;
}

/*
 *  実行文の処理
 */
static int depth = 0;
static int getStat(void);                      // 再帰呼出の関係で
static int getBlock(void);                     //   プロトタイプ宣言が必要

// IF文
static int getIf(void) {
  chkTok('(', "if の次に '(' がない");
  int tree = getExpr();                        // 条件式(w)を読み込む
  chkTok(')', "if 条件式の次に ')' がない");
  int sta = syNewNode(SyIF,tree,getStat());    // 本体を読み込み if 文を作る
  if (isTok(LxELSE)) {                         // 'else'が続けばelse節がある
    sta = syNewNode(SyELS, sta, getStat());    //   else 節を読み込み登録する
  }
  return sta;
}

// WHILE文
static int getWhile(void) {
  chkTok('(', "while の次に '(' がない");
  int tree = getExpr();                        // 条件式(w)を読み込む
  chkTok(')', "while 条件式の次に ')' がない");
  depth = depth + 1;
  int sta = syNewNode(SySEMI,getStat(),SyNULL);// SyNULLは再初期化無しの意味
  depth = depth - 1;
  sta =  syNewNode(SyWHL, tree, sta);          // while を完成
  return sta;
}

// DO - WHILE文
static int getDoWhile(void) {
  depth = depth + 1;
  int sta = getStat();                         // 本体を読み込む
  depth = depth - 1;
  chkTok(LxWHILE, "do - while 文の while がない");
  chkTok('(',   "while の次に '(' がない");
  int tree = getExpr();                                  // 条件式(w)を読み込む
  chkTok(')', "do - while 条件式の次に ')' がない");
  chkTok(';', "do - while 文の次に ';' がない");
  sta = syNewNode(SyDO, sta, tree);        // 登録
  return sta;
}

// FOR文
static int getFor(void) {
  // curScope = curScope + 1;     // C言語と同じスコープルールにするなら
  int tmpIdx  = ntGetSize();                   // for 文はひとつのスコープに
  int tmpCnt  = curCnt;                        //   なるのでローカル変数をもつ
  chkTok('(', "for の次に '(' がない");

  // 初期化式
  int ini = SyNULL;
  if (tok!=';') {                              // 初期化式ありの場合
    if (isType()) {                            //   型が続けば
      ini = getLVars();                        //       変数定義を読み込む
    } else {                                   // それ以外なら
      ini = getExpr();                         //   初期化式(w)を読む
    }
  }
  chkTok(';', "for 初期化の次に ';' がない");

  // 条件式
  int cnd = SyNULL;
  if (tok!=';') {                              // 条件式ありの場合
    cnd = getExpr();                           // 条件式(w)を読み込む
  }
  chkTok(';', "for 条件の次に ';' がない");

  // 再初期化式
  int rini = SyNULL;
  if (tok!=')') {                              // 再初期化式ありの場合
    rini = getExpr();                          // 再初期化式(w)を読み込む
  }
  chkTok(')', "for 再初期化の次に ')' がない");

  // 本文
  depth = depth + 1;
  int sta = getStat();                         // 本体の読み込み
  depth = depth - 1;

  // 木を組み立てる
  // for (int i=1; i<10; i=i+1) sum = sum + a[i];  は
  // {int i=1; for (;i<10; i=i+1) sum = sum + a[i]; にする
  sta = syNewNode(SySEMI, sta, rini);          // 再初期化と本文
  sta = syNewNode(SyWHL, cnd, sta);            // while文相当部分
  sta = syNewNode(SyBLK, ini, sta);            // 初期化とwhile文でブロック

  ntUndefName(tmpIdx);                         // 表からローカル変数を捨てる
  curCnt = tmpCnt;                             // スタックの深さを戻す
  // curScope = curScope - 1;     // C言語と同じスコープルールにするなら
  return sta;
}

// break 文
static int getBreak(void) {
  if (depth <= 0) error("ループ外で break を使用");
  chkTok(';', "break の次に ';' がない");
  return syNewNode(SyBRK, SyNULL, SyNULL);
}

// continue 文
static int getCont(void) {
    if (depth <= 0) error("ループ外で continue を使用");
  chkTok(';', "continue の次に ';' がない");
  return syNewNode(SyCNT, SyNULL, SyNULL);
}

// return 文
static int getReturn(void) {
  int t = ntGetType(funcIdx);                  // 関数の型
  int d = ntGetDim(funcIdx);                   // 関数の型の配列次元数
  int exp = SyNULL;                            // 式
  if (t!=TyINTR && (t!=TyVOID || d!=0)) {      // 現在の関数がvoid型以外なら
    exp = getExpr();                           // 式(w)を読み込み
  }
  chkTok(';', "return の次に ';' がない");
  return syNewNode(SyRET, exp, SyNULL);
}

static int lastStat = LxNONTOK;     // 最後の文はどの種類のトークンで始まったか

// 文
static int getStat(void) {
  int sta = SyNULL;
  int ltok = tok;                              // 最後に読んだ文(returnがあるか)
  if      (isTok(LxIF))       sta=getIf();     // if 文を見つけた場合
  else if (isTok(LxWHILE))    sta=getWhile();  // while 文を見つけた場合
  else if (isTok(LxDO))       sta=getDoWhile();// do - while 文を見つけた場合
  else if (isTok(LxFOR))      sta=getFor();    // for 文を見つけた場合
  else if (isTok(LxRETURN))   sta=getReturn(); // return 文を見つけた場合
  else if (isTok(LxBREAK))    sta=getBreak();  // break 文を見つけた場合
  else if (isTok(LxCONTINUE)) sta=getCont();   // continue 文を見つけた場合
  else if (isTok(';'))    ;                    // 空文を見つけた場合
  else if (isTok('{')) {                       // ブロックを見つけた場合
    // curScope = curScope + 1;                // C言語と同じスコープルールなら
    sta = getBlock();                          //   ブロックを読み込む
    // curScope = curScope - 1;                // C言語と同じスコープルールなら
  } else {                                     // どれでもなければ式文
    sta = getExpr();                           // 式(w)を読み込む
    chkTok(';', "式文の後ろに ';' がない");
  }
  lastStat = ltok;                             // 最後に読んだ文(returnがあるか)
  return sta;
}

// ブロックを読み込む
static int getBlock(void) {
  int lval   = SyNULL;
  int tmpIdx = ntGetSize();                    // ブロック終了時にローカル変数
  int tmpCnt = curCnt;                         //   を捨てるために憶えておくもの
  while (tok!='}' && tok!=EOF) {               // '}' が来るまで処理を続ける
    int rval;
    if (isType()) {                            // 型が来た場合
      rval = getLVars();                       //   ローカル変数の宣言
      chkTok(';', "変数宣言が ';' で終わっていない");
    } else {                                   // 型以外なら
      rval = getStat();                        //   文を読み込む
    }
    lval = syCatNode(lval, rval);              // リストを作る
  }
  chkTok('}', "ブロックが '}' で終了していない");
  ntUndefName(tmpIdx);                         // 表からローカル変数を捨てる
  curCnt = tmpCnt;                             // スタックの深さを戻す
  if (lval!=SyNULL && syGetType(lval)==SySEMI) // 意味のあるブロックなら
    sySetType(lval, SyBLK);                    //   リストを { } で括る
  return lval;
}

// プロトタイプ宣言と引数を比較する
static void chkParam(int idx) {
  if (ntGetType(idx)!=curType || ntGetDim(idx)!=curDim)
    error("引数が以前の宣言と異なる");
}

// 関数宣言の引数リスト( 'f( ... )'の ... 部分)を読み込む
static void getParams(int idx) {
  int lastTok = ',';                           // '...'の前は ',' でなきゃ
  for(curCnt=-1;isType();curCnt=curCnt-1){     // 型名の間,オフセットを変化
    getType();                                 // 型を読む
    if (idx>=0) chkParam(idx-curCnt);          // プロトタイプ宣言があれば比較
    getName(false);                            // 仮引数名を登録
    lastTok = tok;                             // ',' があったか記録する
    if (!isTok(',')) break;                    // ',' の間繰り返す
  }
  if (lastTok==',' && isTok(LxDOTDOTDOT)) {    // '...' があった場合
    curType = TyDOTDOTDOT;                     // 型は便宜的に DOTDOTDOT
    curDim  = 0;
    if (idx>=0) chkParam(idx-curCnt);          // プロトタイプ宣言があれば比較
    ntDefName("", curScope, TyDOTDOTDOT,       // '...'を関数引数として表に登録
	      0, curCnt, false);
  }
  chkTok(')', "引数リストが ')' で終わっていない");
}


// 関数を読み込む(プロトタイプ宣言、定義両方を処理する)
static void getFunc(void) {
  funcIdx = ntGetSize() - 1;       // 処理中の関数(return型のチェックに使用)
  int idx = ntSrcGlob(funcIdx);              // 同じ名前のものはないか
  if (idx>=0 && (ntGetScope(idx)==ScCOMM || ntGetScope(idx)==ScGVAR))
    error("2重定義(以前は変数)");
  if (idx>=0 &&
      (ntGetType(idx)!=curType||ntGetDim(idx)!=curDim||ntGetPub(idx)!=pubFlag))
    error("関数の型が以前の宣言と異なっている");
  if (curType==TyINTR && curDim!=0)
    error("interrupt型の配列は認められない");
  getTok();                                  // '(' を読み飛ばす
  lastStat = LxNONTOK;                       // 最後の文(returnで終わっているか)
  curScope = ScLVAR;                         // 仮引数、ローカル変数のスコープ
  int prmIdx = ntGetSize();                  // 仮引数の先頭を記憶
  getParams(idx);                            // 仮引数リストを読み込む
  int prmCnt = ntGetSize()-prmIdx;           // 仮引数の個数を計算
  if (ntGetType(funcIdx)==TyINTR && prmCnt!=0) // 割込み関数は引数を持てない
    error("interrupt関数は引数を持てない");
  ntSetCnt(funcIdx, prmCnt);                 // 関数に引数の個数を記録
  if (idx>=0 && ntGetCnt(idx)!=ntGetCnt(funcIdx))
    error("引数の数が以前の宣言と異なっている");
  int locIdx = ntGetSize();                  // ローカル変数の先頭を記録
  if (isTok('{')) {                          // 関数本体がある場合
    if (idx>=0 && ntGetScope(idx)!=ScPROT) error("関数の2重定義");
    if (idx>=0) ntSetScope(idx, ScFUNC);     // 以前の宣言を定義に変更
    ntSetScope(funcIdx, ScFUNC);             // 今回の宣言を定義に変更
    maxCnt = 0;                              // 最大の変数番号
    curCnt = 0;                              // 次出現変数の番号
    getBlock();                              // 関数本体を読み込む
    if ((ntGetType(funcIdx)!=TyVOID ||       // void型の関数,
	 ntGetDim(funcIdx)!=0)      &&       //
	ntGetType(funcIdx)!=TyINTR  &&       // interupt型以外の関数が
	lastStat!=LxRETURN)                  // return文で終わっていない
      error("関数が値を返していない");
    //syPrintTree();                         // ### デバッグ用 ###
    semChkFunc(syGetRoot(), funcIdx);       // 意味解析を行う*************************
    if (optFlag) optTree(syGetRoot());       // 木を最適化する
    //syPrintTree();                         // ### デバッグ用 ###
    genFunc(funcIdx, maxCnt, krnFlag);       //   コード生成
    sySetSize(0);                            // コード生成終了で木を消去する
  } else {                                   // プロトタイプ宣言の場合
    chkTok(';', "プロトタイプ宣言が ';' で終わっていない");
#ifdef C
    genProto(funcIdx);                       // プロトタイプ宣言を出力
#endif                                       // (トランスレータ版だけで必要)
  }
  if (idx>=0) {                              // 既に名前表にあれば
    ntUndefName(funcIdx);                    //   全体を削除
  } else {                                   // そうでなければ
    ntUndefName(locIdx);                     // シグネチャーだけ残す
    for (int i=prmIdx; i<locIdx; i=i+1)      // その際、仮引数は検索対象に
      ntSetScope(i, ScVOID);                 // ならないスコープに変更する．
  }
}

/*
 * グローバル変数の処理
 */
static int getGArrayInit(int dim);           // 再帰呼出があるので宣言必要

// 初期化に使用される定数式を読み込む     **********optTreeに移植予定
static int getCnst(int typ) {
  int tree = getAsExpr();                    // 初期化式にはカンマ式不可
  //chkCmpat(w, typ, 0);                     // 初期化(代入)できるかチェック
  optTree(tree);                             // 定数式を計算する
  int ty = syGetType(tree);
  if (ty!=SyCNST && ty!=SyLABL && ty!=SySTR && ty!=SySIZE)
    error("定数式が必要");
  return tree;
}

// array(n1 [, n2] ...) を読み込む */
static int getArray0(int dim) {
  //if (dim<=0) error("array の次元が配列の次元を超える");
  int node = getCnst(TyINT);                 // 整数定数式を読み込む
  if (syGetLVal(node)<=0)
    error("配列のサイズは正であるべき");
  if (isTok(',')) {                          // ',' が続くなら
    int lVal = getArray0(dim - 1);           //   ','の右側を先に読み
    node = syCatNode(lVal, node);            //   リストの左につなぐ
  }
  return node;
}

static int getArray(int dim) {
  chkTok('(', "array の次に '(' がない");
  int node = getArray0(dim);
  chkTok(')', "array の後に ')' がない");
  int d = 0;
  if (curType==TyCHAR || curType==TyBOOL)    // 配列ならバイト単位にする
    d = dim;
  return syNewNode(SyARRY, node, d);         // 構造体と配列と同じ扱い
}

// 構造体初期化('{ ... }'を読み込む
static int getStructInit0() {
  int node = SyNULL;
  int i=-curType+1;                          // i が構造体フィールドを指す
  do {
    //if (i>=ntGetSize()||ntGetScope(i)!=ScVOID)
    //  error("構造体初期化がフィールドより多い");
    int n = 0;
    if (ntGetType(i)<=0) {                   // フィールドが構造体の場合
      chkTok(LxNUL, "入れ子構造体の初期化は null だけ");
      n = syNewNode(SyCNST, 0, TyREF);
    } else if (ntGetDim(i)>0) {              // フィールドが配列の場合
      if (isTok(LxSTRING)) {                 // 入力が文字列なら
	if (ntGetDim(i)!=1||ntGetType(i)!=TyCHAR)
	  error("文字列で初期化できない");
	int lab = genStr(lxGetStr());        // .Lx STRING "..." を出力
	n = syNewNode(SySTR, lab, SyNULL);
      } else if (isTok(LxNUL)) {             // 配列を null で初期化
	n = syNewNode(SyCNST, 0, TyREF);
      } else error("構造体フィールドの配列初期化は文字列か null だけ");
    } else if (ntGetType(i)==TyINT) {        // フィールドが整数型の場合
      n = getCnst(TyINT);                    //   整数定数式を読み込む
    } else if (ntGetType(i)==TyBOOL) {       // フィールドが論理型の場合
      n = getCnst(TyBOOL);                   //   論理型定数式を読み込む
    } else if (ntGetType(i)==TyCHAR) {       // フィールドが文字型の場合
      n = getCnst(TyCHAR);                   //   文字型定数式を読み込む
    } else error("バグ...getStructInit0");
    node = syCatNode(node, n);
    i=i+1;                                   // i を進める
  } while (isTok(','));                      // ',' が続く間繰り返す
  // if (i<ntGetSize() && ntGetScope(i)==ScVOID)
  //   error("構造体の初期化がフィールドより少ない");
  return node;
}

// 構造体の初期化
static int getStructInit() {
  int node = SyNULL;
  if (isTok(LxNUL)) {                        // null による初期化の場合
    node = syNewNode(SyCNST, 0, TyREF);      // NULL を木に登録
  } else {                                   // '{ ... }' による初期化の場合
    chkTok('{', "構造体の初期化が '{' で始まっていない");
    node = getStructInit0();                 // 初期化の内容を読み込む
    node = syNewNode(SyLIST, node, SyNULL);
    chkTok('}', "構造体の初期化が '}' で終わっていない");
  }
  return node;
}

// 配列初期化('{ ... }'の ... を読み込む
static int getGArrayInit0(int dim) {
  int node = SyNULL;                         // 最初はリスト要素が空
  if (dim<=1) {                              // 最後の１次元の場合
    if (curType<=0) {                        //   型が構造体型なら
      do {
        int r = getStructInit();             //     構造体の初期化を読み込む
        node = syCatNode(node, r);           //     リストにつなぐ
      } while (isTok(','));                  //      ',' が続く間、繰り返す
    } else if (curType==TyINT  ||
               curType==TyCHAR ||
               curType==TyBOOL) {            //   型が基本型なら
      do {
        int n = getCnst(curType);            //     定数式を読み込む
        node = syCatNode(node, n);           //     リストにつなぐ
      } while (isTok(','));                  //     ',' が続く間、繰り返す
    } else error("バグ...getGArrayInit0");
  } else {                                   // 2次元以上の配列の場合
    do {
      int r = getGArrayInit(dim-1);          //   1次元低い配列の読み込み
      node = syCatNode(node, r);             //   リストにつなぐ
    } while (isTok(','));                    //   ',' が続く間繰り返す
  }
  int d = 0;
  if (curType==TyCHAR || curType==TyBOOL)    // バイト単位の配列にするもの
    d = dim;
  return syNewNode(SyLIST, node, d);         // リストを返す
}

// 大域配列の初期化
static int getGArrayInit(int dim) {
  int node = 0;
  if (isTok('{')) {                          // '{ ... ' の初期化の場合
    node = getGArrayInit0(dim);              // 初期化の本体(リスト)を読み込む
    chkTok('}', "初期化で '}'が不足");       // '}' をチェック
  } else if (isTok(LxSTRING)) {              // 文字列による初期化の場合
    if (curType!=TyCHAR || dim!=1) error("文字列による初期化の型");
    int l = genStr(lxGetStr());              // .L STRING "..." を出力
    node = syNewNode(SySTR, l, SyNULL);      // 文字列を木に登録
  } else if (isTok(LxARRAY)) {               // 'array( ... ) の場合
    node = getArray(dim);                    // array の括弧の中を読み込む
  } else if (isTok(LxNUL)) {                 // null による初期化の場合
    node = syNewNode(SyCNST, 0, TyREF);      // NULL を木に登録
  } else error("配列の初期化");              // 上のどれでもない入力
  return node;
}

// 大域変数
static void getGVar(void) {
  int curIdx = ntGetSize()-1;                // 処理中の変数
  if (isTok('=')) {                          // '='が続けば初期化部分がある
    ntSetScope(curIdx, ScGVAR);              // 初期化された大域変数
    if (curDim>0) {                          // 配列の初期化なら
      getGArrayInit(curDim);                 //   配列の初期化部分 '{ ... }'
    } else if (curType==TyINT  ||
               curType==TyCHAR ||
               curType==TyBOOL) {            // 基本型の初期化なら
      getCnst(curType);                      //   整数定数式を入力する
    } else if (curType<=0) {                 // 構造体初期化なら
      getStructInit();                       //   構造体の初期化部分 '{ ... }'
    } else error("バグ...getGVar");
  }
  semChkGVar(curType, curDim, pubFlag);      // 意味解析
  genGVar(curIdx);                           // 大域変数の生成
  sySetSize(0);                              // データ生成終了で木を消去する
}

// 大域変数宣言
static void getGVars(void) {
  getGVar();                                 // 大域変数１件
  while (isTok(',')) {                       // ',' が続いている
    getName(pubFlag);                        // 変数名を表に登録
    getGVar();                               // 大域変数１件
  }
  chkTok(';', "大域変数の宣言が ';' で終わっていない");
}

// C-- プログラムを読む
static void getProg(void) {
  pubFlag = false;                           // 'public'をまだ読んでいない
  if (isTok(LxPUBLIC)) {                     // 'public'修飾子があった
    pubFlag = true;                          //   それを記録し次トークンの処理
  }
  if (isTok(LxSTRUCT)) {                     // 'struct'なら構造体宣言
    curScope = ScSTRC;                       // スコープは構造体(-1)にする
    curType=TySTRUC;                         // 型は STRUC にする
    if (pubFlag) error("\'public struct\'は使えない");
    getName(false);                          // 構造体名を表に登録
    getStruct();                             // 構造体宣言の本体を読み込む
  } else if (isTok(LxTYPEDEF)) {             // 'typedef'なら
    curScope = ScSTRC;                       // スコープは構造体(-1)にする
    curType = TyREF;                         // 何か参照型
    if (pubFlag) error("\'public typedef\'は使えない");
    getName(false);                          // 新たな型名を表に登録
    chkTok(';', "typedef宣言が';'で終わっていない");
  } else {
    curScope = ScPROT;                       // プロトタイプ宣言と仮定する
    getType();                               // 型を読み込む
    getName(pubFlag);                        // プロトタイプ宣言として登録
    if (tok=='(') {                          // '('が続けば関数(仮定は正解)
      getFunc();                             // 関数定義かプロトタイプ宣言
    } else {                                 // 大域変数宣言だと判明
      ntSetScope(ntGetSize()-1, ScCOMM);     // 仮定が外れたので訂正
      curScope = ScCOMM;                     // 仮定が外れたので訂正
      if (curType==TyVOID && curDim==0)      // 変数だとすればvoid型はエラー
	error("void型変数は使用できない");
      if (curType==TyINTR)                   // 変数ならinterrupt型はエラー
	error("interrupt型変数は使用できない");
      getGVars();                            // グローバル変数宣言
    }
  }
}

// フラグを操作する
void snSetOptFlag(boolean f) { optFlag = f; };
void snSetKrnFlag(boolean f) { krnFlag = f; };

//-----------------------------------------------------------------------------
// ソースの読み込みはコンパイラ版とトランスレータ版で処理が異なる。
//-----------------------------------------------------------------------------
#ifndef C
// コンパイラ版はディレクティブに興味がないので getProg() を繰り返すだけ
void snGetSrc(void) {
  getTok();                                  // 最初の tok を読み込む
  while (_tok!=EOF)                          // EOF になるまで
    getProg();                               //   C-- プログラムを処理
}
//-----------------------------------------------------------------------------
#else
// トランスレータ版はディレクティブの処理と getProg() を繰り返す
void snGetSrc(void) {
  getTok();                                  // 最初の tok を読み込む
  while (_tok!=EOF) {                        // EOF になるまで
    if (_tok==LxFILE) {                      //  ディレクティブなら
      genDirect(lxGetVal(), lxGetStr());     //   # 行番号 "ファイル名" の処理
      getTok();
    } else {                                 //  ディレクティブ以外なら
      getProg();                             //   C-- プログラムを処理
    }
  }
}
#endif
