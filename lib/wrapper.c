/*
 * Programing Language C-- "Compiler"
 *    Tokuyama kousen Advanced educational Computer.
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
 * wrapper.c : C-- 版と C 版で仕様が異なる関数など
 *
 * 2016.08.07 : feof を追加
 * 2016.05.26 : #include <wrapper.h> を追加
 * 2016.03.02 : mAlloc, fOpen を追加
 * 2016.02.26 : 初期バージョン
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wrapper.h>

void *_addrAdd(void *a, int inc) {
  return (char *)a + inc;
}

void *_aToA(void *a) {
  return a;
}

// 以下は 64bit 環境では危険
/*
int _aToI(void *a) {
  return (int)a;
}

void *_iToA(int i) {
  return (void*)i;
}
*/

// TaC 版ではエラーチェックして終了する
void * _mAlloc(int s) {
  void *p = malloc(s);
  if (p==NULL) {
    perror("malloc");
    exit(1);
  }
  return p;
}

/* TaC 版ではエラー原因により null を返すので、
 * 以下では再現しきれていない
FILE *_fOpen(char *n, char *m) {
  FILE *fp = fopen(n, m);
  if (fp==NULL) {
    perror("fopen");
    exit(1);
  }
  return fp;
}
*/

// TaC 版では EOF になるタイミングが早い
int _feof(FILE *fp) {
  int c = fgetc(fp);
  if (c==EOF) return 1;
  ungetc(c, fp);
  return 0;
}

// TaC 版では string.cmm に記述されている関数

// 文字を探す
int strChr(char *s, int c) {
  char *p = strchr(s, c);
  if (p==NULL) return -1;
  return p - s;
}

// 最後の文字を探す
int strRchr(char *s, int c) {
  char *p = strrchr(s, c);
  if (p==NULL) return -1;
  return p - s;
}

// 文字列を探す
int strStr(char *s1, char *s2) {
  char *p = strstr(s1, s2);
  if (p==NULL) return -1;
  return p - s1;
}

// 部分文字列を返す
char *subStr(char *s, int pos) {
  return s + pos;
}
