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
 * op.c : C--コンパイラの構文木最適化ルーチンのドライバ
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
#include "../optree.h"
#include "../namtbl.h"
#include "../sytree.h"

#define StrMAX  128

static char str[StrMAX + 1];
static int ln;
static FILE *fp;    // 入力ファイル
static FILE *fpout; // 出力ファイル

static int curLab = 0;              // STRラベル用のカウンタ

int lxGetLn(){ return ln; }
char *lxGetFname() { return "ERROR lxGetFname"; } // optreeでは使われないはず

// ラベルを割り当てる
static int newLab() {
  curLab = curLab + 1;
  return curLab;
}

// 文字列を生成しラベル番号を返す
static int genStr(char *str) {
  int lab = newLab();                            // ラベルを割り付け
  int i=0;
  fprintf(fpout, "%d S ", lxGetLn());
  while(str[i]){
    fprintf(fpout, "%c", str[i]);
    i =i+1;
  }
  fprintf(fpout, "\n");
  return lab;                                    // ラベル番号を返す
}

// コード生成処理の記録
// 関数１個分のコード生成
static void genFunc(int funcIdx, int depth, boolean krnFlg) {
  optTree(syGetRoot());
  printTree(fpout);
  fprintf(fpout, "%d F %d %d %d\n", lxGetLn(), funcIdx, depth, krnFlg);
}
// 初期化データの生成
static void genData(int idx) {
  optTree(syGetRoot());
  printTree(fpout);
  fprintf(fpout, "%d D %d\n", lxGetLn(), idx);
}

// 非初期化データの生成
static void genBss(int idx) {
  printTree(fpout);
  fprintf(fpout, "%d B %d\n", lxGetLn(), idx);
}

// 10進数を読んで値を返す
static int getDec() {
  int v = 0;                                     // 初期値は 0
  char ch = fgetc(fp);
  boolean minusflg = false;
  if(ch==EOF)
    return EOF;
  else if(ch=='-'){
    minusflg = true;
    ch = fgetc(fp);
  }
  while (isdigit(ch)) {                          // 10進数字の間
    v = v*10 + ch - '0';                         // 値を計算
    ch = fgetc(fp);                              // 次の文字を読む
  }
  if(minusflg) return -v;
  return v;                                      // 10進数の値を返す
}

int main(int argc, char *argv[]){
  int type, lval, rval, idx, depth, krn;
  char op;
  char *fn = "stdin";
  if (argc==2){
    if (!strEndsWith(argv[1], ".sm")) error("入力ファイル形式が違う");
    fp = eOpen(argv[1],"r");   // 中間ファイルをオープン
    fn = argv[1];
  }else if (argc==1){
    fp = stdin;
  }else{
    fprintf(stderr, "使用方法 : %s [<srcfile>]\n", argv[0]);
    exit(1);
  }
  ntLoadTable(fn);                       // 名前表ファイルから名前表を作成
  fpout = openDstWithExt(fn, ".op");     // 拡張子を".sm"に変更してOpen
  while(true){
    ln = getDec();
    if(ln==EOF)
      return 0;
    op = fgetc(fp);
    fgetc(fp);      // 空白読み捨て
    if(op=='N'){
      type = getDec();
      lval = getDec();
      rval = getDec();
      syNewNode(type, lval, rval);
    }else if(op=='F'){
      idx   = getDec();
      depth = getDec();
      krn   = getDec();
      genFunc(idx, depth, krn);
      sySetSize(0);
    }else if(op=='D'){
      idx = getDec();
      genData(idx);
      sySetSize(0);
    }else if(op=='B'){
      idx = getDec();
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
