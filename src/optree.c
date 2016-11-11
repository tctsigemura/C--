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
 * optree.c : C--コンパイラの構文木最適化ルーチン
 * 
 * 2016.07.10         : 初期化配列の最適化に対応
 * 2016.05.22         : SySIZE に対応する
 * 2016.02.05 v3.0.0  : トランスレータと統合(SyBLK, SyIDXW, SyIDXB, SyDOT対応)
 * 2015.08.31 v2.1.0  : copyNode の型を void に変更(int ではバグ)
 * 2012.09.08         : a % 1 は 0 に置き換える
 * 2012.08.18         : 空文を含む制御構造を簡単化する際のバグ訂正
 * 2012.08.15 v2.0.0  : 木を変形し定数を集めて計算する部分を全面的に書き換え
 * 2012.08.10         : x*2, x/2 を x<<1, x>>1 に置換えるのをやめる
 * 2011.04.30         : 配列や構造体の初期化を optTree で処理するように変更
 * 2011.04.24         : for の条件式省略時 SyNULL を参照するバグ訂正
 * 2011.02.04         : SyNULL のとき optExp をすぐ終了するように改良
 * 2010.10.07         : シフトビット数が 0 の場合はシフト演算を抹消する
 * 2010.10.07         : シフトビット数が定数で範囲外の場合をエラーにする
 * 2010.09.12 v1.0.0  : 最適化と外部変数の定数式による初期化ができる H8 統合版
 * 2010.09.04         : コーディング開始
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "sytree.h"
#include "optree.h"

// ノードの内容を上書きする
static void setNode(int node, int typ, int l, int r) {
  if (node==SyNULL) error("BUG : setNode");
  sySetType(node, typ);
  sySetLVal(node, l);
  sySetRVal(node, r);
}

// ノード(src)からノード(dst)に内容をコピーする
static void copyNode(int dst, int src) {
  // 式中のSyNULLはエラー(文中は空文の表現)
  if (src==SyNULL && !SyISSTAT(syGetType(dst))) error("BUG : copyNode");

  if (src!=SyNULL)                                  // 通常のコピー
    setNode(dst, syGetType(src), syGetLVal(src), syGetRVal(src));
  else
    setNode(dst, SySEMI, SyNULL, SyNULL);           // 空文のコピー
}

// 符号なし整数値を取り出す
#define getUInt(node) (syGetLVal(node))             // 取り出すだけで良い

// 符号付き整数値を取り出す
static int getSInt(int node) {
  int v = getUInt(node);
  if ((v & (1<<(NWORD-1)))!=0) v = v | (~0 ^ WMSK); // 符号拡張する
  return v;
}  

// 整数から論理値への型変換
#define bool(v) (v & 1)                             // 最下位ビットで判定

// 論理値を取り出す
static boolean getBool(int node) {
  return bool(getUInt(node));
}

// ノードに整数定数を書き込む
#define setUInt(node, v) (setNode((node),SyCNST,(v),2))
#define setSInt(node, v) (setUInt((node), (v)))

// ノードにブール定数を書き込む
static void setBool(int node, boolean v) {
  if (v) setUInt(node, 1);
  else   setUInt(node, 0);
}  

// node は定数 n か
static boolean isCnstN(int node, int n) {
  return syGetType(node)==SyCNST &&                 // 定数なら
    getUInt(node)==(n & WMSK);                      //   上位ビット消去後、比較
}

// l, r は両方とも定数か
static boolean isCnstCnst(int l, int r) {
  return syGetType(l)==SyCNST && syGetType(r)==SyCNST;
}

// この演算子は左辺式を必要とする
static boolean needLeft(int node) {
  int op = syGetType(node);
  return SyIS1OPR(op)|| SyIS2OPR(op) || SyISCMP(op) || SyISLOPR(op) ||
    op==SyASS || op==SyCOMM || op==SySEMI || op==SyBLK;
}

// この演算子は右辺式を必要とする
static boolean needRight(int node) {
  int op = syGetType(node);
  return (op==SyFUNC && syGetRVal(node)!=SyNULL) ||
    SyIS2OPR(op) || SyISCMP(op) || SyISLOPR(op) ||
    op==SyASS || op==SyCOMM || op==SySEMI || op==SyBLK;
}

/*
 * 定数を含む式を簡単化する
 */
// 定数の '-' を最適化する
static void optNeg(int node, int l) {
  if (syGetType(l)==SyCNST)                       // 対象が定数なら
    setSInt(node, -getSInt(l));                   //   計算結果を格納する
}

// 定数の '!' を最適化する
static void optNot(int node, int l) {
  if (syGetType(l)==SyCNST)                       // 対象が定数なら
    setBool(node, !getBool(l));                   //   計算結果を格納する
}

// 定数の '~' を最適化する
static void optBNot(int node, int l) {
  if (syGetType(l)==SyCNST)                       // 対象が定数なら
    setUInt(node, ~getUInt(l));                   //   計算結果を格納する
}

// 定数の chr 演算を最適化する
static void optChar(int node, int l) {
  if (syGetType(l)==SyCNST)                       // 対象が定数なら、
    setUInt(node, getUInt(l) & BMSK);             //   下位バイトを格納する
}

// 定数の bool 演算を最適化する
static void optBool(int node, int l) {
  if (syGetType(l)==SyCNST)                       // 対象が定数なら
    setBool(node, getBool(l));                    //   計算結果を格納する
}

// sizeof 演算を最適化する
//static void optSIZE(int node, int l) {          // やることがない
//}

// 定数の足算を最適化する
static void optAdd(int node, int l, int r) {
  if (isCnstCnst(l, r))                           // 両辺とも定数なら   
    setSInt(node, getSInt(l) + getSInt(r));       //   両辺の和を格納する
  else if (isCnstN(l, 0))                         // (0 + 右辺) なら
    copyNode(node, r);                            //   右辺と置き換える
  else if (isCnstN(r, 0))                         // (左辺 + 0) なら
    copyNode(node, l);                            //   左辺と置き換える
}

// 定数の引算を最適化する
static void optSub(int node, int l, int r) {
  if (isCnstCnst(l, r))                           // 両辺とも定数なら   
    setSInt(node, getSInt(l) - getSInt(r));       //   両辺の差を格納する
  else if (isCnstN(l, 0))                         // (0 - 右辺) なら
    setNode(node, SyNEG, r, SyNULL);              //   -(右辺)に置き換える
  else if (isCnstN(r, 0))                         // (左辺 - 0) なら
    copyNode(node, l);                            //   左辺と置き換える
}

// 定数の左シフトを最適化する
static void optShl(int node, int l, int r) {
  int n = getUInt(r);
  if (syGetType(r)==SyCNST && (n<0 || NWORD<=n))  // シフト幅が範囲外
    error("<< シフト桁数が範囲外");               //   エラー
  else if (isCnstN(r, 0))                         // シフトビット数が 0 なら
    copyNode(node, l);                            //   左辺と置き換える
  else if (isCnstCnst(l, r))                      // 両辺とも定数なら   
    setSInt(node, getSInt(l) << n);               //   シフト結果を格納する
}

// 定数の右シフトを最適化する
static void optShr(int node, int l, int r) {
  int n = getUInt(r);
  if (syGetType(r)==SyCNST && (n<0 || NWORD<=n))  // シフト幅が範囲外
    error(">> シフト桁数が範囲外");               //   エラー
  else if (isCnstN(r, 0))                         // シフトビット数が 0 なら
    copyNode(node, l);                            //   左辺と置き換える
  else if (isCnstCnst(l, r))                      // 両辺とも定数なら
    setSInt(node, getSInt(l) >> n);               //   シフト結果を格納する
}

// 定数の AND を最適化する
static void optBAnd(int node, int l, int r) {
  if (isCnstCnst(l, r))                           // 両辺とも定数なら
    setUInt(node, getUInt(l) & getUInt(r));       //   両辺のANDを格納する
  else if (isCnstN(l, 0) || isCnstN(r, 0))        // どちらかが定数 0 なら
    setUInt(node, 0);                             //     0 を格納する
  else if (isCnstN(l, WMSK))                      // 左辺が定数 FF..FF なら
    copyNode(node, r);                            //   右辺で置き換える
  else if (isCnstN(r, WMSK))                      // 右辺が定数 FF..FF なら
    copyNode(node, l);                            //   左辺で置き換える
}

// 定数の OR を最適化する
static void optBOr(int node, int l, int r) {
  if (isCnstCnst(l, r))                           // 両辺とも定数なら   
    setUInt(node, getUInt(l) | getUInt(r));       //   OR を格納する
  else if (isCnstN(l, WMSK) || isCnstN(r, WMSK))  // どちらかが FF..FH なら
    setUInt(node , WMSK);                         //   FF..FH を格納する
  else if (isCnstN(l, 0))                         // 左辺が定数 0 なら
    copyNode(node, r);                            //   右辺で置き換える
  else if (isCnstN(r, 0))                         // 右辺が定数 0 なら
    copyNode(node, l);                            //   左辺で置き換える
}

// 定数の XOR を最適化する
static void optBXor(int node, int l, int r) {
  if (isCnstCnst(l, r))                           // 両辺とも定数なら 
    setUInt(node, getUInt(l) ^ getUInt(r));       //   XOR を格納する
  else if (isCnstN(l, WMSK))                      // 左辺が定数FF..FHなら
    setNode(node, SyBNOT, r, SyNULL);             //     単項演算 '~r'に
  else if (isCnstN(l, 0))                         // 左辺が定数 0 なら
    copyNode(node, r);                            //     右辺に置換える
  else if (isCnstN(r, WMSK))                      // 右辺が定数FF..FHなら
    setNode(node, SyBNOT, l, SyNULL);             //     単項演算 '~l' に
  else if (isCnstN(r, 0))                         // 右辺が定数 0 なら
    copyNode(node, l);                            //     左辺と置き換える
}

// 定数のかけ算を最適化する
static void optMul(int node, int l, int r) {
  if (isCnstCnst(l, r))                           // 両辺とも定数なら
    setSInt(node, getSInt(l) * getSInt(r));       //   積を格納する
  else if (isCnstN(l, 0) || isCnstN(r, 0))        // どちらかが定数0なら
    setSInt(node ,0);                             //     0 を格納する
  else if (isCnstN(l, 1))                         // 左辺が 1 なら
    copyNode(node, r);                            //   右辺で置換え
  else if (isCnstN(r, 1))                         // 右辺が 1 なら
    copyNode(node, l);                            //   左辺で置換え
  else if (isCnstN(l, WMSK))                      // 左辺が -1 なら
    setNode(node, SyNEG, r, SyNULL);              //   -(右辺)で置換え
  else if (isCnstN(r, WMSK))                      // 右辺が -1 なら
    setNode(node, SyNEG, l, SyNULL);              //   -(左辺)で置換え
}

// 定数の割算を最適化する
static void optDiv(int node, int l, int r) {
  if (isCnstN(r, 0))                              // 右辺が定数 0 なら
    error("0 での割算");                          //   エラー
  else if (isCnstCnst(l, r))                      // 両辺とも定数なら   
    setSInt(node, getSInt(l) / getSInt(r));       //   商 を格納する
  else if (isCnstN(r, 1))                         // 右辺が定数 1 なら
    copyNode(node, l);                            //   左辺で置き換える
}

// 定数の剰余算を最適化する
static void optMod(int node, int l, int r) {
  if (isCnstN(r, 0))                              // 右辺が定数 0 なら
    error("0 での剰余算");                        //   エラー
  else if (isCnstCnst(l, r))                      // 両辺とも定数なら
    setSInt(node, getSInt(l) % getSInt(r));       //   剰余 を格納する
  else if (isCnstN(r, 1))                         // 右辺が定数 1 なら
    setSInt(node ,0);                             //     0 を格納する
}

// '[]'演算を最適化する
//static void optIdxW(int node, int l, int r) {
//}

// '[]'演算を最適化する
//static void optIdxB(int node, int l, int r) {
//}

// '.'演算を最適化する
//static void optDot(int node, int l, int r) {
//}

// 定数の '>' 演算を最適化する
static void optGt(int node, int l, int r) {
  if (isCnstCnst(l, r))                           // 両辺とも定数なら
    setBool(node, getSInt(l) > getSInt(r));       //   結果を定数にする
}

// 定数の '>=' 演算を最適化する
static void optGe(int node, int l, int r) {
  if (isCnstCnst(l, r))                           // 両辺とも定数なら
    setBool(node, getSInt(l) >= getSInt(r));      //   結果を定数にする
}

// 定数の '<' 演算を最適化する
static void optLt(int node, int l, int r) {
  if (isCnstCnst(l, r))                           // 両辺とも定数なら
    setBool(node, getSInt(l) < getSInt(r));       //   結果を定数にする
}

// 定数の '<=' 演算を最適化する
static void optLe(int node, int l, int r) {
  if (isCnstCnst(l, r))                           // 両辺とも定数なら
    setBool(node, getSInt(l) <= getSInt(r));      //   結果を定数にする
}

// 定数の '==' 演算を最適化する
static void optEqu(int node, int l, int r) {
  if (isCnstCnst(l, r))                           // 両辺とも定数なら
    setBool(node, syGetLVal(l) == syGetLVal(r));  //   結果を定数にする
}

// 定数の '!=' 演算を最適化する
static void optNeq(int node, int l, int r) {
  if (isCnstCnst(l, r))                           // 両辺とも定数なら
    setBool(node, syGetLVal(l) != syGetLVal(r));  //   結果を定数にする
}

// 定数の '||' 演算を最適化する
static void optOr(int node, int l, int r) {
  if (isCnstN(l, true) || isCnstN(r, true))       // どちらかが定数 true なら
    setBool(node, true);                          //   定数 true にする
  else if (isCnstN(l, false))                     // 左辺が定数 false なら
    copyNode(node, r);                            //   右辺で上書き
  else if (isCnstN(r, false))                     // 右辺が定数 false なら
    copyNode(node, l);                            //   左辺で上書き
}

// 定数の '&&' 演算を最適化する
static void optAnd(int node, int l, int r) {
  if (isCnstN(l, false) || isCnstN(r, false))     // どちらかが定数 false なら
    setBool(node, false);                         //   定数 false にする
  else if (isCnstN(l, true))                      // 左辺が定数 true なら
    copyNode(node, r);                            //   右辺で上書き
  else if (isCnstN(r, true))                      // 右辺が定数 true なら
    copyNode(node, l);                            //   左辺で上書き
}

// 式の定数を計算する
static void calExp(int node) {
  int ty = syGetType(node);                       // 演算子
  int l  = syGetLVal(node);                       // 左辺
  int r  = syGetRVal(node);                       // 右辺

  // 先に演算の対象を最適化する
  if (needLeft(node))  calExp(l);                 // 左辺あるので処理する
  if (needRight(node)) calExp(r);                 // 右辺あるので処理する

  // 演算子に応じた最適化を行う
  if      (ty==SyNEG)  optNeg(node, l);           // 単項演算'-'最適化を試みる
  else if (ty==SyNOT)  optNot(node, l);           // 単項演算'!'最適化を試みる
  else if (ty==SyBNOT) optBNot(node, l);          // 単項演算'~'最適化を試みる
  else if (ty==SyCHAR) optChar(node, l);          // 単項演算chr最適化を試みる
  else if (ty==SyBOOL) optBool(node, l);          // 単項演算bool最適化を試みる
  //else if (ty==SySIZE) optSize(node, l);       // 単項演算sizeof最適化を試みる
  else if (ty==SyADD)  optAdd(node, l, r);        // 足算の最適化を試みる
  else if (ty==SySUB)  optSub(node, l, r);        // 引算の最適化を試みる
  else if (ty==SySHL)  optShl(node, l, r);        // 左シフトの最適化を試みる
  else if (ty==SySHR)  optShr(node, l, r);        // 右シフトの最適化を試みる
  else if (ty==SyBAND) optBAnd(node, l, r);       // AND の最適化を試みる
  else if (ty==SyBOR)  optBOr(node, l, r);        // OR  の最適化を試みる
  else if (ty==SyBXOR) optBXor(node, l, r);       // XOR の最適化を試みる
  else if (ty==SyMUL)  optMul(node, l, r);        // 掛算の最適化を試みる
  else if (ty==SyDIV)  optDiv(node, l, r);        // 割算の最適化を試みる
  else if (ty==SyMOD)  optMod(node, l, r);        // 剰余算の最適化を試みる
  //else if (ty==SyIDXW) optIdxW(node, l, r);     // '[]'演算の最適化を試みる
  //else if (ty==SyIDXB) optIdxB(node, l, r);     // '[]'演算の最適化を試みる
  //else if (ty==SyDOT)  optDot(node, l, r);      // '.'演算の最適化を試みる
  else if (ty==SyGT)   optGt(node, l, r);         // '>' 演算の最適化を試みる
  else if (ty==SyGE)   optGe(node, l, r);         // '>=' 演算の最適化を試みる
  else if (ty==SyLT)   optLt(node, l, r);         // '<' 演算の最適化を試みる
  else if (ty==SyLE)   optLe(node, l, r);         // '<=' 演算の最適化を試みる
  else if (ty==SyEQU)  optEqu(node, l, r);        // '==' 演算の最適化を試みる
  else if (ty==SyNEQ)  optNeq(node, l, r);        // '!=' 演算の最適化を試みる
  else if (ty==SyOR)   optOr(node, l, r);         // '||' 演算の最適化を試みる
  else if (ty==SyAND)  optAnd(node, l, r);        // '&&' 演算の最適化を試みる

  // 最適化終了後
  if (syGetType(node)==SyCNST)                    // 定数になっていたら
    sySetLVal(node, getUInt(node) & WMSK);        //   値のオーバーフローを訂正
                                                  //  (全てのノードは計算前に
}                                                 //   ここで訂正を受ける)

/*
 * 式の形を変形し定数を集め、定数を計算する
 */
// 右辺の演算子(右部分式の根っこ)はたぐれる互換性がある(左辺に移動できる)
static boolean canPull(int opL, int opR) {
  if ((opL==SyADD || opL==SySUB)                  // '+','-' は
      && (opR==SyADD || opR==SySUB))              //   右も '+','-' なら
    return true;                                  //     手繰れる
  if ((opL==SyMUL || opL==SyBAND || opL==SyBOR || // '*','&','|',
       opL==SyBXOR || opL==SyAND || opL==SyOR) && // '^','&&','||' は
      (opL==opR))                                 //   両方が同じなら手繰れる
    return true;                                  //  ('/'は含まれないので注意)
                                                  //   '/'を手繰る例:結果が異る
  return false;                                   //     5/(10/10) => 5/10*10
}                                                 // それ以外は手繰れない 

// 再帰のために必要
static void moveLeft(int node);

// 木を「たぐる」(右に広がった木を、なるべく左一直線にする)
static void pullTree(int node) {
  while (needRight(node) &&                       // 右辺が存在する演算子なら
	 canPull(syGetType(node),                 //    演算子を比較し
		 syGetType(syGetRVal(node)))) {   //      たぐれる間たぐる  
    int a = syGetLVal(node);                      //
    int b = syGetRVal(node);                      //    node     =>     node
    int c = syGetLVal(b);                         //    /  |            /  |
    int d = syGetRVal(b);                         //   a   b           b   d
    sySetLVal(node, b);                           //     / |         / |
    sySetRVal(node, d);                           //    c  d        a  c
    sySetLVal(b, a);                              //
    sySetRVal(b, c);                              //
                                                  //    +(-)     =>     -(-)
    /* '+','-' 混在に備えて type を調整する*/     //    /  |            /  |
    int opN = syGetType(node);                    //   a   -(+)      +(-)  d
    int opB = syGetType(b);                       //       /  |      /  |
    if (opN==SyADD || opN==SySUB) {               //      c   d     a   c
      sySetType(b, opN);                          //
      if (opN!=opB) opN = SySUB;                  //    +(-)     =>     +(+)
      else          opN = SyADD;                  //    /  |            /  |
      sySetType(node, opN);                       //   a   +(-)      +(-)  d
    }                                             //       /  |      /  |
                                                  //      c   d     a   c
    // 左を再帰的にたぐる
    pullTree(b);
  }
  // 残った右側について
  if (needRight(node))                            // 右辺が存在するなら
    moveLeft(syGetRVal(node));                    //   右辺を処理する
}

// 木を「左によせる」
static void moveLeft(int node) {
  if (needLeft(node))                             // 左辺が存在する演算子なら
    moveLeft(syGetLVal(node));                    //   左に進み
  pullTree(node);                                 //     戻りながらたぐる
}

// 定数の引き算を負定数の足し算に統一する
static void normSub(int node) {
  int op = syGetType(node);                       // 演算子
  int r  = syGetRVal(node);                       // 右辺
  if (op==SySUB && syGetType(r)==SyCNST) {        // 引き算で右辺が定数なら
    setSInt(r, -getSInt(r));                      //   定数の符号を反転し
    sySetType(node, SyADD);                       //     足し算に変更する
  }
}

// 定数を左部分木のなるべく先に移動する
static void moveCnst(int node) {
  normSub(node);                                  // 定数の引算は負数の足算に

  int op  = syGetType(node);
  if ((op!=SyADD  && op!=SyMUL && op!=SyDIV  &&   // 左右で入れ替え可能な
       op!=SyBAND && op!=SyBOR && op!=SyBXOR &&   //  演算子で
       op!=SyAND  && op!=SyOR) ||                 //   右側が定数の場合
      syGetType(syGetRVal(node))!=SyCNST) return; //
                                                  //  (node)+
  int l, r, lOp;                                  //       / |
  for (;;) {                                      //    ...  c    (c : const)
    l   = syGetLVal(node);                        //
    r   = syGetRVal(node);                        //     以下の処理をする
    lOp = syGetType(l);                           //
    if (!((lOp==op||(op==SyADD && lOp==SySUB)) && // 左辺に同じ演算子が連続し
	syGetType(syGetRVal(l))!=SyCNST))         //   左辺の右辺が定数以外
      break;                                      //     なら木を変形する
    sySetType(l, syGetType(node));                //
    sySetType(node, lOp);                         //  (node)+   =>          -
    sySetRVal(node, syGetRVal(l));                //       / |             / |
    sySetRVal(l, r);                              //   (l)-  5(r)   (node)+  a
    node = l;                                     //     / |             / |
  }                                               //    x  a         (l)x  5(r)
   
  if (lOp==op) { /* 定数の場合SySUBはないはず */  // 同じ演算子連続なら定数発見
    sySetLVal(node, syGetLVal(l));                //  (node)+   => (node)+
    sySetLVal(l, syGetRVal(l));                   //       / |          / |
    sySetRVal(l, r);                              //   (l)+  5(r)      y  +(l)
    sySetRVal(node, l);                           //     / |             / |
                                                  //    y -5           -5  5(r)
    if (op==SyDIV) sySetType(l, SyMUL);           //
  }                                               //  x/c1/c2 = x/(c1*c2)
}

// 式を整理する (定数を末端に集め計算する)
static void arrExp(int node) {
  if (needRight(node)) arrExp(syGetRVal(node));   // 右辺があれば整理

  if (needLeft(node)) {                           // 左辺があれば
    arrExp(syGetLVal(node));                      //   左に進み、戻りながら、
    moveCnst(node);                               //     定数を左端に集める
  }
  calExp(node);                                   // 部分木全体で定数計算をする
}

// 式の最適化
static void optExp(int node) {
  if (node==SyNULL) return;
  moveLeft(node);                                 // 算術式木を左一列に変形する
  arrExp(node);                                   // 算術式の定数を末端に集め
}                                                 //   定数部分を計算する

/*
 * 文の最適化
 */
// if 文を最適化する
static void optIf(int node) {
  int cnd = syGetLVal(node);                      // 条件式を取り出す
  optExp(cnd);                                    // 条件式を最適化
  if (syGetType(cnd)==SyCNST) {                   // 条件式が定数なら
    if (getBool(cnd)) {                           //   true の場合
      copyNode(node, syGetRVal(node));            //     本文で上書き
      optTree(node);                              //     本文を最適化
    } else                                        //   talse の場合
      copyNode(node, cnd);                        //     if 文を定数で上書き
  } else                                          //     コード生成されないはず
    optTree(syGetRVal(node));                     // 通常は本文を最適化
}     

// if-else 文を最適化する
static void optEls(int node) {
  int ifs = syGetLVal(node);                      // if 文を取り出す
  int cnd = syGetLVal(ifs);                       // 条件式を取り出す
  optExp(cnd);                                    // 条件式を最適化
  if (syGetType(cnd)==SyCNST) {                   // 条件式が定数なら
    if (getBool(cnd)) {                           //   ture の場合
      copyNode(node, syGetRVal(ifs));             //     if 文の本体で上書き
    } else {                                      //   false の場合
      copyNode(node, syGetRVal(node));            //     else 節で上書き
    }
    optTree(node);                                //   本文を最適化
  } else {                                        // 通常は
    optTree(syGetRVal(node));                     //   else 節を最適化
    optTree(syGetRVal(ifs));                      //   if の本文を最適化
  }
}

// while 文を最適化する
static void optWhl(int node) {
  int cnd = syGetLVal(node);                      // 条件式を取り出す
  optExp(cnd);                                    // 条件式を最適化
  if (cnd!=SyNULL && syGetType(cnd)==SyCNST) {    // 条件式が存在し定数で
    if (getBool(cnd)) {                           //   true の場合
      sySetLVal(node, SyNULL);                    //     条件式を削除
      optTree(syGetLVal(syGetRVal(node)));        //     本文を最適化
      optExp(syGetRVal(syGetRVal(node)));         //     for の再初期化を最適化
    } else {                                      //   false の場合
      copyNode(node, cnd);                        //     while 文を定数で上書き
    }                                             //     コード生成されないはず
  } else {
    optTree(syGetLVal(syGetRVal(node)));          // 通常は本文の最適化と
    optExp(syGetRVal(syGetRVal(node)));           // 再初期化の最適化をする
  }
}

// do-while 文を最適化する
static void optDo(int node) {
  int cnd = syGetRVal(node);                      // 条件式を取り出す
  int sta = syGetLVal(node);                      // 本文を取り出す
  optExp(cnd);                                    // 条件式を最適化
  optTree(sta);                                   // 本文の最適化
  if (syGetType(cnd)==SyCNST) {                   // 条件式が定数なら
    if (getBool(cnd)) {                           //   true の場合
      sySetType(node, SyWHL);                     //     while 文に書き換える
      sySetType(cnd, SySEMI);                     //     条件式を再利用
      sySetLVal(cnd, sta);                        //     本文を移動
      sySetRVal(cnd, SyNULL);                     //     再初期化は無し
      sySetRVal(node, cnd);                       //     while の本文を登録
      sySetLVal(node, SyNULL);                    //     条件式は削除
    } else                                        //   false の場合
      copyNode(node, sta);                        //     do-whileを
  }                                               //       本文で上書き
}

// break 文を最適化する
static void optBrk(int node) {
}                                                 // 今のところやることがない

// continue 文を最適化する
static void optCnt(int node) {
}                                                 // 今のところやることがない

// return 文を最適化する
static void optRet(int node) {
  optExp(syGetLVal(node));                        // 条件式を最適化
}

// ブロックを最適化する
static void optBlk(int node) {
  optTree(syGetLVal(node));                       //   先に左側を最適化
  optTree(syGetRVal(node));                       //   次に右側を最適化
}

// 初期化配列を最適化する
static void optArr(int node){                     // !!分割版でのみ使用される!!
  optBlk(syGetLVal(node));
}

// 構文木を最適化する
void optTree(int node) {
  if (node==SyNULL) return;                       // 何も無い
  int ty = syGetType(node);
  if      (ty==SyVAR)  return;                    // ローカル変数宣言は無視
  else if (ty==SyIF)   optIf(node);               // if 文
  else if (ty==SyELS)  optEls(node);              // if-else 文
  else if (ty==SyWHL)  optWhl(node);              // while 文
  else if (ty==SyDO)   optDo(node);               // do-while 文
  else if (ty==SyBRK)  optBrk(node);              // break 文
  else if (ty==SyCNT)  optCnt(node);              // continue 文
  else if (ty==SyRET)  optRet(node);              // retrun 文
  else if (ty==SySEMI ||
	         ty==SyBLK)  optBlk(node);              // ブロック
  else if (ty==SyARRY ||                          // 分割版では初期化配列
           ty==SyLIST) optArr(node);              // 　の最適化が必要
  else                 optExp(node);              // 式文
}    
