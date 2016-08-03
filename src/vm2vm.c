/*
 * Programing Language C-- "Compiler"
 *    Tokuyama kousen Educational Computer 16bit Ver.
 *
 * Copyright (C) 2002-2015 by
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
 * vm2vm.c : 仮想マシンのコードから実際のコードを生成するプログラムのサンプル
 *           (このプログラムは仮想マシンのコードを出力する)
 *
 * 2016.05.04         : vmLdArg, vmStArg を vmLdPrm, vmStPrm(パラメータ)に変更
 * 2015.08.31 v2.1.0  : vmEntryK 追加
 * 2015.08.30         : vmStWrdのコメント誤り修正（バイト配列=>ワード配列）
 * 2012.09.08         : BOOLOR, BOORAND の位置が分かるようにコメントを出力
 * 2010.09.12 v1.0.0  : 最適化と外部変数の定数式による初期化ができる H8 統合版
 * 2010.09.09         : 初期化データでaddrofを使用できるようにvmDcNameを追加
 * 2010.08.31         : AADD を削除し、SCALE を追加
 * 2010.08.23         : MREG を追加 (RetP, RetFを統合)
 *                    : ラベル欄を印刷したときは必ず改行するようにした
 * 2010.08.16         : アドレス計算用の命令 AADD を追加
 * 2010.07.20         : Subversion による管理を開始
 * 2010.04.10         : データ生成機能を追加
 * 2010.04.01 v0.9.11 : 初期バージョン
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "namtbl.h"
#include "vm.h"

// 名前を表現するラベルを印刷する
void vmName(int idx) {
  if (ntGetPub(idx)) printf("_");                  // public なら '_' を付加
  else printf(".");                                // private なら '.' を付加
  printf("%s\n", ntGetName(idx));                  //   名前本体の出力
}

// 番号で管理されるコンパイラが生成したラベルを印刷する
void vmTmpLab(int lab) {
  printf(".L%d\n", lab);
}

// 番号で管理されるSTRING用ラベルを印刷する
void vmTmpLabStr(int lab) {
  printf(".S%d\n", lab);                           // .Sn
}

// 関数の入口
void vmEntry(int depth, int idx) {
  vmName(idx);
  printf("\tENTRY\t%d\n", depth);
}

// カーネル関数の入口
void vmEntryK(int depth, int idx) {
  vmName(idx);
  printf("\tENTRYK\t%d\n", depth);
}

// 関数の出口
void vmRet() {
  printf("\tRET\n");
}

// 割り込み関数の入口
void vmEntryI(int depth, int idx) {
  vmName(idx);
  printf("\tENTRYI\t%d\n", depth);
}

// 割り込み関数の出口
void vmRetI() {
  printf("\tRETI\n");
}

// 関数の返り値をハードウェアレジスタに移動
void vmMReg() {
  printf("\tMREG\n");
}

// スタックトップの値を引数領域に移動
void vmArg() {
  printf("\tARG\n");
}

// 値を返さない関数を呼び出す
void vmCallP(int n, int idx) {
  printf("\tCALLP\t%d,", n);
  vmName(idx);
}

// 値を返す関数を呼び出す
void vmCallF(int n, int idx) {
  printf("\tCALLF\t%d,", n);
  vmName(idx);
}

// 無条件ジャンプ
void vmJmp(int lab) {
  printf("\tJMP\t");
  vmTmpLab(lab);
}

// スタックから論理値を取り出し true ならジャンプ
void vmJT(int lab) {
  printf("\tJT\t");
  vmTmpLab(lab);
}

// スタックから論理値を取り出し false ならジャンプ
void vmJF(int lab) {
  printf("\tJF\t");
  vmTmpLab(lab);
}

// 定数をスタックにロードする
void vmLdCns(int c) {
  printf("\tLDC\t%d\n", c);
}

// 大域変数の値をスタックに積む
void vmLdGlb(int idx) {
  printf("\tLDG\t");
  vmName(idx);
}

// n番目のローカル変数の値をスタックに積む
void vmLdLoc(int n) {
  printf("\tLDL\t%d\n", n);
}

// n番目の引数の値をスタックに積む
void vmLdPrm(int n) {
  printf("\tLDP\t%d\n", n);
}

// 文字列のアドレスをスタックに積む
void vmLdStr(int lab) {
  printf("\tLDC\t");
  vmTmpLab(lab);
}

// まず、スタックから添字とワード配列の番地を取り出す
// 次にワード配列の要素の内容をスタックに積む
void vmLdWrd() {
  printf("\tLDW\n");
}

// まず、スタックから添字とバイト配列の番地を取り出す
// 次にバイト配列の要素の内容をスタックに積む
void vmLdByt() {
  printf("\tLDB\n");
}

// ラベルの値(アドレス)をスタックに積む
void vmLdLab(int idx) {
  printf("\tLDC\t");
  vmName(idx);
}

// スタックトップの値を大域変数にストアする(POPはしない)
void vmStGlb(int idx) {
  printf("\tSTG\t");
  vmName(idx);
}

// スタックトップの値をn番目のローカル変数にストアする(POPはしない)
void vmStLoc(int n) {
  printf("\tSTL\t%d\n", n);
}

// スタックトップの値をn番目の引数にストアする(POPはしない)
void vmStPrm(int n) {
  printf("\tSTP\t%d\n", n);
}

// まず、スタックから添字とワード配列の番地を取り出す
// 次に、配列の要素に新しいスタックトップの値を
// ストアする(後半でPOPはしない)
void vmStWrd() {
  printf("\tSTW\n");
}

// まず、スタックから添字とバイト配列の番地を取り出す
// 次に、配列の要素に新しいスタックトップの値(バイト)を
// ストアする(後半でPOPはしない)
void vmStByt() {
  printf("\tSTB\n");
}

// まず、スタックから整数を取り出し２の補数を計算する
// 次に、計算結果をスタックに積む
void vmNeg() {
  printf("\tNEG\n");
}

// まず、スタックから論理値を取り出し否定を計算する
// 次に、計算結果をスタックに積む
void vmNot() {
  printf("\tNOT\n");
}

// まず、スタックから整数を取り出し１の補数を計算する
// 次に、計算結果をスタックに積む
void vmBNot() {
  printf("\tBNOT\n");
}

// まず、スタックから整数を取り出し文字型で有効なビット数だけ残しマスクする
// 次に、計算結果をスタックに積む
void vmChar() {
  printf("\tCHAR\n");
}

// まず、スタックから整数を取り出し最下位ビットだけ残しマスクする
// 次に、計算結果をスタックに積む
void vmBool() {
  printf("\tBOOL\n");
}

// 何もしない
void vmBool() {
  ;
}
// まず、スタックから整数を二つ取り出し和を計算する
// 次に、計算結果をスタックに積む
void vmAdd() {
  printf("\tADD\n");
}

// まず、スタックから整数を二つ取り出し差を計算する
// 次に、計算結果をスタックに積む
void vmSub() {
  printf("\tSUB\n");
}

// まず、スタックからシフトするビット数、シフトされるデータの順で取り出す。
// 次に、左シフトした結果をスタックに積む
void vmShl() {
  printf("\tSHL\n");
}

// まず、スタックからシフトするビット数、シフトされるデータの順で取り出す。
// 次に、右シフトした結果をスタックに積む
void vmShr() {
  printf("\tSHR\n");
}

// まず、スタックから整数を二つ取り出しビット毎の論理積を計算する
// 次に、計算結果をスタックに積む
void vmBAnd() {
  printf("\tBAND\n");
}

// まず、スタックから整数を二つ取り出しビット毎の排他的論理和を計算する
// 次に、計算結果をスタックに積む
void vmBXor() {
  printf("\tBXOR\n");
}

// まず、スタックから整数を二つ取り出しビット毎の論理和を計算する
// 次に、計算結果をスタックに積む
void vmBOr() {
  printf("\tBOR\n");
}

// まず、スタックから整数を二つ取り出し積を計算する
// 次に、計算結果をスタックに積む
void vmMul() {
  printf("\tMUL\n");
}

// まず、スタックから整数を二つ取り出し商を計算する
// 次に、計算結果をスタックに積む
void vmDiv() {
  printf("\tDIV\n");
}

// まず、スタックから整数を二つ取り出し剰余を計算する
// 次に、計算結果をスタックに積む
void vmMod() {
  printf("\tMOD\n");
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmGt() {
  printf("\tGT\n");
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmGe() {
  printf("\tGE\n");
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmLt() {
  printf("\tLT\n");
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmLe() {
  printf("\tLE\n");
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmEq() {
  printf("\tEQ\n");
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmNe() {
  printf("\tNE\n");
}

// スタックから値を一つ取り出し捨てる
void vmPop() {
  printf("\tPOP\n");
}

/*
 *  マクロ命令
 *
 */
// BOOLOR .L1,.L2,.L3
void vmBoolOR(int lab1, int lab2, int lab3) {
  printf("; BOOLOR .L%d,.L%d,.L%d\n", lab1, lab2, lab3);
  vmJmp(lab3);
  vmTmpLab(lab1);
  vmLdCns(1);
  if (lab2!=-1) {
    vmJmp(lab3);
    vmTmpLab(lab2);
    vmLdCns(0);
  }
  vmTmpLab(lab3);
  printf("; ----\n");
}

// BOOLAND .L1,.L2,.L3
void vmBoolAND(int lab1, int lab2, int lab3) {
  printf("; BOOLAND .L%d,.L%d,.L%d\n", lab1, lab2, lab3);
  vmJmp(lab3);
  vmTmpLab(lab1);
  vmLdCns(0);
  if (lab2!=-1) {
    vmJmp(lab3);
    vmTmpLab(lab2);
    vmLdCns(1);
  }
  vmTmpLab(lab3);
  printf("; ----\n");
}

/*
 *  データ生成用の疑似命令
 */
// DW name      (ポインタデータの生成)
void vmDwName(int idx) {
  printf("\tDW\t");
  vmName(idx);
}

// DW .Ln       (ポインタデータの生成)
void vmDwLab(int lab) {
  printf("\tDW\t");
  vmTmpLab(lab);
}

// DW .Sn       (STRING用ポインタデータの生成)
void vmDwLabStr(int lab) {
  printf("\tDW\t");
  vmTmpLabStr(lab);
}

// DW N         (整数データの生成)
void vmDwCns(int n) {
  printf("\tDW\t%d\n", n);
}

// DB N         (バイトデータの生成、バイト配列の初期化で使用)
void vmDbCns(int n) {
  printf("\tDB\t%d\n", n);
}

// WS N         (ワード領域の生成)
void vmWs(int n) {
  printf("\tWS\t%d\n", n);
}

// BS N         (バイト配列領域の生成)
void vmBs(int n) {
  printf("\tBS\t%d\n", n);
}

// STRING "..." (文字列データ(バイト配列)の生成)
void vmStr(char *s) {
  printf("\tSTRING\t\"%s\"\n", s);
}
