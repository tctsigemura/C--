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
 * op.c : C--コンパイラの意味解析ルーチンのドライバ
 * 
 * 2016.11.11         : 初期バージョン
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
#include "../semantic.h"
#include "../namtbl.h"
#include "../sytree.h"

static int ln;
static FILE *fp;    // 入力ファイル
static FILE *fpout; // 出力ファイル

static int curLab = 0;              // STRラベル用のカウンタ

int lxGetLn(){ return ln; }
char *lxGetFname() { return "Fname"; } 

// ラベルを割り当てる
static int newLab() {
  curLab = curLab + 1;
  return curLab;
}

// 文字列を生成しラベル番号を返す
static int genStr(char *str) {
  int lab = newLab();                            // ラベルを割り付け
  fprintf(fpout, "%d S %s\n", lxGetLn(), str);   // 文字列をファイルに出力
  return lab;                                    // ラベル番号を返す
}

// コード生成処理の記録
// 関数１個分のコード生成
static void genFunc(int funcIdx, int depth, boolean krnFlg) {
  syPrintTree(fpout);
  fprintf(fpout, "%d F %d %d %d\n", lxGetLn(), funcIdx, depth, krnFlg);
}
// 初期化データの生成
static void genData(int idx) {
  syPrintTree(fpout);
  fprintf(fpout, "%d D %d\n", lxGetLn(), idx);
}

// 非初期化データの生成
static void genBss(int idx) {
  //syPrintTree(fpout);
  fprintf(fpout, "%d B %d\n", lxGetLn(), idx);
}

// 段階版のためにダミーのoptTree
void optTree(int node){}

int main(int argc, char *argv[]){
  int type, lval, rval, idx, depth, krn;
  char op;
  char *fn = "stdin";
  if (argc==2){
    if (!strEndsWith(argv[1], ".fsm")) error("入力ファイル形式が違う");
    fp = eOpen(argv[1],"r");   // 中間ファイルをオープン
    fn = argv[1];
  }else if (argc==1){
    fp = stdin;
  }else{
    fprintf(stderr, "使用方法 : %s [<srcfile>]\n", argv[0]);
    exit(1);
  }
  ntLoadTable(fn);                       // 名前表ファイルから名前表を作成
  //ntDebPrintTable();
  fpout = openDstWithExt(fn, ".sm");     // 拡張子を".sm"に変更してOpen
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
    }else if(op=='G'){
      //ntDebPrintTable();                           //**********DEBUG
      idx = getDec(fp);
      //printf("idx=%d, root=%d\n",idx,syGetRoot()); //**********DEBUG
      //syDebPrintTree();
      semChkGVar(idx);
      if (syGetRoot()!=SyNULL) genData(idx);
      else genBss(idx);
      sySetSize(0);
    }else if(op=='S'){
      genStr(getStr(fp));
    }else{
      error("bug");
    }
  }
  return 0;
}
