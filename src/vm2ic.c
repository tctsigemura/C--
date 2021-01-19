/*
 * Programing Language C-- "Compiler"
 *    Tokuyama kousen Educational Computer 16bit Ver.
 *
 * Copyright (C) 2002-2021 by
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
 * vm2ic.c : 中間コードをそのまま出力する（仮想マシンのニーモニックにもしない）
 *
 * 2021.01.19         : 構文木の仕様変更に伴いvm*Loc, vm*Prm変更
 * 2016.09.24         : vm2vm をもとに作成
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"

// 名前を表現するラベルを印刷する
void vmNam(int idx) {
  printf("vmNam(%d)\n", idx);
}

// 番号で管理されるコンパイラが生成したラベルを印刷する
void vmLab(int lab) {
  printf("vmLab(%d)\n", lab);
}

// 関数の入口
void vmEntry(int depth) {
  printf("vmEntry(%d)\n", depth);
}

// カーネル関数の入口
void vmEntryK(int depth) {
  printf("vmEntryK(%d)\n", depth);
}

// 関数の出口
void vmRet() {
  printf("vmRet()\n");
}

// 割り込み関数の入口
void vmEntryI(int depth) {
  printf("vmEntryI(%d)\n", depth);
}

// 割り込み関数の出口
void vmRetI() {
  printf("vmRetI()\n");
}

// 関数の返り値をハードウェアレジスタに移動
void vmMReg() {
  printf("vmMReg()\n");
}

// スタックトップの値を引数領域に移動
void vmArg() {
  printf("vmArg()\n");
}

// 値を返さない関数を呼び出す
void vmCallP(int n, int idx) {
  printf("vmCallP(%d, %d)\n", n, idx);
}

// 値を返す関数を呼び出す
void vmCallF(int n, int idx) {
  printf("vmCallF(%d, %d)\n", n, idx);
}


// 無条件ジャンプ
void vmJmp(int lab) {
  printf("vmJmp(%d)\n", lab);
}

// スタックから論理値を取り出し true ならジャンプ
void vmJT(int lab) {
  printf("vmJT(%d)\n", lab);
}

// スタックから論理値を取り出し false ならジャンプ
void vmJF(int lab) {
  printf("vmJF(%d)\n", lab);
}

// 定数をスタックにロードする
void vmLdCns(int c) {
  printf("vmLdCns(%d)\n", c);
}

// 大域変数の値をスタックに積む
void vmLdGlb(int idx) {
  printf("vmLdGlb(%d)\n", idx);
}

// ローカル変数の値をスタックに積む
void vmLdLoc(int idx) {
  int n = ntGetCnt(idx);
  printf("vmLdLoc(%d)\n", n);
}

// 引数の値をスタックに積む
void vmLdPrm(int idx) {
  int n = ntGetCnt(idx);
  printf("vmLdPrm(%d)\n", n);
}

// ラベルの参照(アドレス)をスタックに積む
void vmLdLab(int lab) {
  printf("vmLdLab(%d)\n", lab);
}

// まず、スタックから添字とワード配列の番地を取り出す
// 次にワード配列の要素の内容をスタックに積む
void vmLdWrd() {
  printf("vmLdWrd()\n");
}

// まず、スタックから添字とバイト配列の番地を取り出す
// 次にバイト配列の要素の内容をスタックに積む
void vmLdByt() {
  printf("vmLdByt()\n");
}

// 名前の参照(アドレス)をスタックに積む
void vmLdNam(int idx) {
  printf("vmLdNam(%d)\n", idx);
}

// スタックトップの値を大域変数にストアする(POPはしない)
void vmStGlb(int idx) {
  printf("vmStGlb(%d)\n", idx);
}

// スタックトップの値をローカル変数にストアする(POPはしない)
void vmStLoc(int idx) {
  int n = ntGetCnt(idx);
  printf("vmStLoc(%d)\n", n);
}

// スタックトップの値を引数にストアする(POPはしない)
void vmStPrm(int idx) {
  int n = ntGetCnt(idx);
  printf("vmStPrm(%d)\n", n);
}

// まず、スタックから添字とワード配列の番地を取り出す
// 次に、配列の要素に新しいスタックトップの値を
// ストアする(後半でPOPはしない)
void vmStWrd() {
  printf("vmStWrd()\n");
}

// まず、スタックから添字とバイト配列の番地を取り出す
// 次に、配列の要素に新しいスタックトップの値(バイト)を
// ストアする(後半でPOPはしない)
void vmStByt() {
  printf("vmStByt()\n");
}

// まず、スタックから整数を取り出し２の補数を計算する
// 次に、計算結果をスタックに積む
void vmNeg() {
  printf("vmNeg()\n");
}

// まず、スタックから論理値を取り出し否定を計算する
// 次に、計算結果をスタックに積む
void vmNot() {
  printf("vmNot()\n");
}

// まず、スタックから整数を取り出し１の補数を計算する
// 次に、計算結果をスタックに積む
void vmBNot() {
  printf("vmBNot()\n");
}

// まず、スタックから整数を取り出し文字型で有効なビット数だけ残しマスクする
// 次に、計算結果をスタックに積む
void vmChr() {
  printf("vmChr()\n");
}

// まず、スタックから整数を取り出し最下位ビットだけ残しマスクする
// 次に、計算結果をスタックに積む
void vmBool() {
  printf("vmBool()\n");
}

// まず、スタックから整数を二つ取り出し和を計算する
// 次に、計算結果をスタックに積む
void vmAdd() {
  printf("vmAdd()\n");
}

// まず、スタックから整数を二つ取り出し差を計算する
// 次に、計算結果をスタックに積む
void vmSub() {
  printf("vmSub()\n");
}

// まず、スタックからシフトするビット数、シフトされるデータの順で取り出す。
// 次に、左シフトした結果をスタックに積む
void vmShl() {
  printf("vmShl()\n");
}

// まず、スタックからシフトするビット数、シフトされるデータの順で取り出す。
// 次に、右シフトした結果をスタックに積む
void vmShr() {
  printf("vmShr()\n");
}

// まず、スタックから整数を二つ取り出しビット毎の論理積を計算する
// 次に、計算結果をスタックに積む
void vmBAnd() {
  printf("vmBAnd()\n");
}

// まず、スタックから整数を二つ取り出しビット毎の排他的論理和を計算する
// 次に、計算結果をスタックに積む
void vmBXor() {
  printf("vmBXor()\n");
}

// まず、スタックから整数を二つ取り出しビット毎の論理和を計算する
// 次に、計算結果をスタックに積む
void vmBOr() {
  printf("vmBOr()\n");
}

// まず、スタックから整数を二つ取り出し積を計算する
// 次に、計算結果をスタックに積む
void vmMul() {
  printf("vmMul()\n");
}

// まず、スタックから整数を二つ取り出し商を計算する
// 次に、計算結果をスタックに積む
void vmDiv() {
  printf("vmDiv()\n");
}

// まず、スタックから整数を二つ取り出し剰余を計算する
// 次に、計算結果をスタックに積む
void vmMod() {
  printf("vmMod()\n");
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmGt() {
  printf("vmGt()\n");
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmGe() {
  printf("vmGe()\n");
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmLt() {
  printf("vmLt()\n");
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmLe() {
  printf("vmLe()\n");
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmEq() {
  printf("vmEq()\n");
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmNe() {
  printf("vmNe()\n");
}

// スタックから値を一つ取り出し捨てる
void vmPop() {
  printf("vmPop()\n");
}

/*
 *  マクロ命令
 *
 */
// BOOLOR .L1,.L2,.L3
void vmBoolOR(int lab1, int lab2, int lab3) {
  printf("vmBoolOR(%d, %d, %d)\n", lab1, lab2, lab3);
}

// BOOLAND .L1,.L2,.L3
void vmBoolAND(int lab1, int lab2, int lab3) {
  printf("vmBoolAND(%d, %d, %d)\n", lab1, lab2, lab3);
}

/*
 *  データ生成用の疑似命令
 */
// DW name      (ポインタデータの生成)
void vmDwName(int idx) {
  printf("vmDwName(%d)\n", idx);
}

// DW .Ln       (ポインタデータの生成)
void vmDwLab(int lab) {
  printf("vmDwLab(%d)\n", lab);
}

// DW N         (整数データの生成)
void vmDwCns(int n) {
  printf("vmDwCns(%d)\n", n);
}

// DB N         (バイトデータの生成、バイト配列の初期化で使用)
void vmDbCns(int n) {
  printf("vmDbCns(%d)\n", n);
}

// WS N         (ワード領域の生成)
void vmWs(int n) {
  printf("vmWs(%d)\n", n);
}

// BS N         (バイト配列領域の生成)
void vmBs(int n) {
  printf("vmBs(%d)\n", n);
}

// STRING "..." (文字列データ(バイト配列)の生成)
void vmStr(char *s) {
  printf("vmStr(\"%s\")\n", s);
}
