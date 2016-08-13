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
 * main.c : C--コンパイラのメインルーチン
 *
 * 2016.06.04         : eOpen() を使用するように変更
 * 2016.02.05 v3.0.0  : syntax.c から分離して新規作成
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "util.h"                   // その他機能モジュール
#include "syntax.h"                 // 構文解析モジュール
#include "lexical.h"                // 字句解析モジュール

// 使い方表示関数
static void usage(char *name) {
  fprintf(stderr,
          "使用方法 : %s [-h] [-v] [-O0] [-O] [-O1] [-K] [<source file>]\n",
          name);
  fprintf(stderr, "    <source file> は C--言語ソースコード\n");
  fprintf(stderr, "    <source file> 省略時は stdin から入力\n");
  fprintf(stderr, "    コンパイル結果は stdout へ出力\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "    -h, -v  : このメッセージを表示\n");
  fprintf(stderr, "    -O, -O1 : 定数をコンパイル前に計算する(デフォルト)\n");
  fprintf(stderr, "    -O0     : 定数をコンパイル前に計算しない\n");
  fprintf(stderr, "    -K      : カーネルコンパイルモード\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "%s version %s (%s 版)\n", name, VER, ARC);
  fprintf(stderr, "(build date : %s)\n", DATE);
  fprintf(stderr, "\n");
}

// main 関数
int main(int argc, char *argv[]) {
  FILE *fp;
  int i = 1;

  if (argc>i &&
      (strcmp(argv[i],"-v")==0 ||            // "-v", "-h" で、使い方と
       strcmp(argv[i],"-h")==0   ) ) {       //  バージョンを表示
    usage(argv[0]);
    exit(0);
  }

  if (argc>i &&
      (strcmp(argv[i],"-O")==0 ||
       strcmp(argv[i],"-O1")==0)  ) {        // "-O","-O1" で、最適化
    snSetOptFlag(true);
    i = i + 1;
  }

  if (argc>i &&
      strcmp(argv[i],"-O0")==0) {            // "-O0" で、最適化しない
    snSetOptFlag(false);
    i = i + 1;
  }

  if (argc>i &&
      strcmp(argv[i],"-K")==0) {             // "-K" で、カーネルコンパイル
    snSetKrnFlag(true);
    i = i + 1;
  }

  if (argc==i+1) {                           // 引数としてソースファイルがある
    fp = eOpen(argv[i],"r");                 // ソースファイルをオープン
    lxSetFname(argv[i]);                     // error表示用にファイル名を登録
  } else if (argc==i) {
    fp = stdin;
    lxSetFname("STDIN");                     // error表示用にファイル名を登録
  } else {
    usage(argv[0]);
    exit(1);
  }

  lxSetFp(fp);                               // 字句解析に fp を知らせる
  snGetSrc();                                // fp からソースコードを入力して
                                             //   stdout へ出力
  return 0;
}
