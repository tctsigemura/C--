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
 * vm.c : C--コンパイラの仮想マシン用コード生成ルーチンのドライバ
 *
 * 2016.07.10         : 初期バージョン
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "util.h"
#include "../util.h"
#include "../vm.h"            // vm2tacと見かけが同じ関数を提供
#include "../code.h"
#include "../namtbl.h"
#include "../sytree.h"

#define StrMAX  128
static char str[StrMAX + 1];

static int ln;
static FILE *fp;                                  // 入力ファイル
static FILE *fpout;                               // 出力ファイル

int lxGetLn(){ return ln; }
char *lxGetFname() { return "ERROR lxGetFname"; } // vmCodeでは使われないはず

// vm出力 0-21:引数1, 22-51:引数0, 52-56:引数2, 57-58:引数3, 59:Str
void vmName(int idx){              fprintf(fpout, "0 %d\n", idx); }
void vmTmpLab(int lab){            fprintf(fpout, "1 %d\n", lab); }
void vmTmpLabStr(int lab){         fprintf(fpout, "2 %d\n", lab); }
void vmEntry(int depth, int idx){  fprintf(fpout, "52 %d %d\n", depth, idx); }
void vmEntryK(int depth, int idx){ fprintf(fpout, "53 %d %d\n", depth, idx); }
void vmRet(void){                  fprintf(fpout, "22\n"); }
void vmEntryI(int depth, int idx){ fprintf(fpout, "54 %d %d\n", depth, idx); }
void vmRetI(void){                 fprintf(fpout, "23\n"); }
void vmMReg(void){                 fprintf(fpout, "24\n"); }
void vmArg(void){                  fprintf(fpout, "25\n"); }
void vmCallP(int n, int idx){      fprintf(fpout, "55 %d %d\n", n, idx); }
void vmCallF(int n, int idx){      fprintf(fpout, "56 %d %d\n", n, idx); }
void vmJmp(int lab){               fprintf(fpout, "3 %d\n", lab); }
void vmJT(int lab){                fprintf(fpout, "4 %d\n", lab); }
void vmJF(int lab){                fprintf(fpout, "5 %d\n", lab); }
void vmLdCns(int c){               fprintf(fpout, "6 %d\n", c); }
void vmLdGlb(int idx){             fprintf(fpout, "7 %d\n", idx); }
void vmLdLoc(int n){               fprintf(fpout, "8 %d\n", n); }
void vmLdPrm(int n){               fprintf(fpout, "9 %d\n", n); }
void vmLdStr(int lab){             fprintf(fpout, "10 %d\n", lab); }
void vmLdLab(int idx){             fprintf(fpout, "11 %d\n", idx); }
void vmLdWrd(void){                fprintf(fpout, "26\n"); }
void vmLdByt(void){                fprintf(fpout, "27\n"); }
void vmStGlb(int idx){             fprintf(fpout, "12 %d\n", idx); }
void vmStLoc(int n){               fprintf(fpout, "13 %d\n", n); }
void vmStPrm(int n){               fprintf(fpout, "14 %d\n", n); }
void vmStWrd(void){                fprintf(fpout, "28\n"); }
void vmStByt(void){                fprintf(fpout, "29\n"); }
void vmNeg(void){                  fprintf(fpout, "30\n"); }
void vmNot(void){                  fprintf(fpout, "31\n"); }
void vmBNot(void){                 fprintf(fpout, "32\n"); }
void vmChar(void){                 fprintf(fpout, "33\n"); }
void vmBool(void){                 fprintf(fpout, "34\n"); }
void vmAdd(void){                  fprintf(fpout, "35\n"); }
void vmSub(void){                  fprintf(fpout, "36\n"); }
void vmShl(void){                  fprintf(fpout, "37\n"); }
void vmShr(void){                  fprintf(fpout, "38\n"); }
void vmBAnd(void){                 fprintf(fpout, "39\n"); }
void vmBXor(void){                 fprintf(fpout, "40\n"); }
void vmBOr(void){                  fprintf(fpout, "41\n"); }
void vmMul(void){                  fprintf(fpout, "42\n"); }
void vmDiv(void){                  fprintf(fpout, "43\n"); }
void vmMod(void){                  fprintf(fpout, "44\n"); }
void vmGt(void){                   fprintf(fpout, "45\n"); }
void vmGe(void){                   fprintf(fpout, "46\n"); }
void vmLt(void){                   fprintf(fpout, "47\n"); }
void vmLe(void){                   fprintf(fpout, "48\n"); }
void vmEq(void){                   fprintf(fpout, "49\n"); }
void vmNe(void){                   fprintf(fpout, "50\n"); }
void vmPop(void){                  fprintf(fpout, "51\n"); }

void vmBoolOR(int L1, int L2, int L3){
  fprintf(fpout, "57 %d %d %d\n", L1, L2, L3); }
void vmBoolAND(int L1, int L2, int L3){
  fprintf(fpout, "58 %d %d %d\n", L1, L2, L3); }

void vmDwName(int idx){   fprintf(fpout, "15 %d\n", idx); }
void vmDwLab(int lab){    fprintf(fpout, "16 %d\n", lab); } 
void vmDwLabStr(int lab){ fprintf(fpout, "17 %d\n", lab); }
void vmDwCns(int n){      fprintf(fpout, "18 %d\n", n); }   
void vmDbCns(int n){      fprintf(fpout, "19 %d\n", n); }   
void vmWs(int n){         fprintf(fpout, "20 %d\n", n); }      
void vmBs(int n){         fprintf(fpout, "21 %d\n", n); }      
void vmStr(char *s){      fprintf(fpout, "59 %s\n", s); }

int main(int argc, char *argv[]){
  int type, lval, rval, idx, depth, krn;
  char op;
  char *fn = "stdin";
  if (argc==2){
    if (!strEndsWith(argv[1], ".op")) error("入力ファイル形式が違う");
    fp = eOpen(argv[1],"r");  // 中間ファイルをオープン
    fn = argv[1];
  }else if (argc==1){
    fp = stdin;
  }else{
    fprintf(stderr, "使用方法 : %s [<srcfile>]\n", argv[0]);
    exit(1);
  }
  ntLoadTable(fn);                   // 名前表ファイルから名前表を作成
  fpout = openDstWithExt(fn, ".vm"); // 拡張子を".vm"に変更してOpen
  while(true){
    ln = getDec(fp);
    if(ln==EOF)
      return 0;
    op = fgetc(fp);
    fgetc(fp);      // 空白読み捨て
    if(op=='N'){
      type = getDec(fp);
      lval = getDec(fp);
      rval = getDec(fp);
      syNewNode(type, lval, rval);
    }else if(op=='F'){
      idx   = getDec(fp);
      depth = getDec(fp);
      krn   = getDec(fp);
      genFunc(idx, depth, krn);
      sySetSize(0);
    }else if(op=='D'){
      idx = getDec(fp);
      genData(idx);
      sySetSize(0);
    }else if(op=='B'){
      idx = getDec(fp);
      genBss(idx);
    }else if(op=='S'){
      int i=0;
      char ch;
      while((ch=fgetc(fp))!='\n'){               // 改行がくるまで文字列
        if(i>StrMAX)
          error("文字列が長すぎる");
        str[i] = ch;
        i = i+1;
      }
      str[i] = '\0';
      genStr(str);
    }else{
      error("bug");
    }
  }
  return 0;
}
