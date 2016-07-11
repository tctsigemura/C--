/*
 * Programing Language C-- "Compiler"
 *    Tokuyama kousen Educational Computer 16bit Ver.
 *
 * Copyright (C) 2016 by
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
 * util.c : 段階コンパイル版用の良く使う関数
 *
 * 2016.06.04         : 初期バージョン
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <strings.h>

#include "../sytree.h"
#include "../util.h"
#include "util.h"

// 中間ファイルから読み込む

// 構文木をファイルに出力する
void printTree(FILE *fp) {
  for (int i=0; i<syGetSize(); i=i+1) {
    fprintf(fp,"%d N ", syLn[i]);                // ソースコードの行番号
    fprintf(fp,"%d ", syGetType(i));             // ノードの種類
    fprintf(fp,"%d ", syGetLVal(i));             // ノードの値１
    fprintf(fp,"%d\n", syGetRVal(i));            // ノードの値２
  }
}

// 入力ファイル名をもらって、拡張子を変更して書込みオープンする
static void tooLongFname() {
  error("ファイル名が長すぎる");
}

FILE *openDstWithExt(char *srcName, char *ext) {
  char dstName[StrMAX + 1];

  if (strlen(srcName)>StrMAX) tooLongFname();
  strcpy(dstName, srcName);
  
  char *lastDot = rindex(dstName, '.');
  if (lastDot!=NULL) *lastDot='\0';

  if (strlen(dstName)+strlen(ext)>StrMAX) tooLongFname();
  strcat(dstName, ext);

  return eOpen(dstName, "w");
}
