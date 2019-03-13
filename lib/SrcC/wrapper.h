/*
 * Programing Language C-- "Compiler"
 *    Tokuyama kousen Advanced educational Computer.
 *
 * Copyright (C) 2016 - 2019 by
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
 * wrapper.h : wrapper.c 関数のプロトタイプ宣言
 *
 * 2019.03.12 : <stdio.h>と<stdlib.h>を追加(NULLやabort()のため)
 * 2019.03.11 : string.hmm の関数を削除
 * 2019.02.23 : 実行時エラーチェックに対応
 * 2019.01.27 : htoi を追加
 * 2018.11.17 : lToL を追加
 * 2018.02.26 : fsize を追加
 * 2018.02.20 : fseek を追加
 * 2016.08.07 : feof を追加
 * 2016.05.26 : 初期バージョン
 *
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef _RTC
// int型配列を表現する構造体型
typedef struct { int l; int a[]; } _IA;

// char型を表現する構造体型
typedef struct { int l; char a[]; } _CA;

// 参照の配列を表現する構造体型
typedef struct  { int l; void *a[]; } _RA;

// 実行時エラーチェック用の inline 関数
// int型の配列チェック用関数
inline static int *_ICA(_IA *p, int i, char *file, int line) {
  if (NULL==p) {
    fprintf(stderr, "%s:%d Null Pointer idx=%d \n", file, line, i);
    abort();
  }
  if (i<0 || p->l<=i) {
    fprintf(stderr, "%s:%d Out of Bound idx=%d\n", file, line, i);
    abort();
  }
  return &(p->a[i]);
}

// char型の配列チェック用関数
inline static char *_CCA(_CA *p, int i, char *file, int line) {
  if (NULL==p) {
    fprintf(stderr, "%s:%d Null Pointer(idx=%d)\n", file, line, i);
    abort();
  }
  if (i<0 || p->l<=i) {
    fprintf(stderr, "%s:%d Out of Bound(idx=%d)\n", file, line, i);
    abort();
  }
  return &(p->a[i]);
}

// 参照配列の配列チェック用関数
inline static void **_RCA(_RA *p, int i, char *file, int line) {
  if (NULL==p) {
    fprintf(stderr, "%s:%d Null Pointer(idx=%d)\n", file, line, i);
    abort();
  }
  if (i<0 || p->l<=i) {
    fprintf(stderr, "%s:%d Out of Bound(idx=%d)\n", file, line, i);
    abort();
  }
  return &(p->a[i]);
}

// 参照チェック関数
inline static void *_CP(void *p, char *file, int line) {
  if(p==NULL){
    fprintf(stderr, "%s:%d Null Pointer\n", file, line);
    abort();
  }
  return p;
}
#endif

// 型変換など
inline static void *_addrAdd(void *a, int inc) {
  return (char *)a + inc;
}

inline static void *_aToA(void *a) {
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
void * _mAlloc(int s);

// TaC 版ではエラー原因により null を返すので、以下では再現しきれていない
FILE *_fOpen(char *n, char *m);

// TaC 版では EOF になるタイミングが C 言語より早い
int _feof(FILE *fp);

// TaC 版ではオフセットが上位と下位に分かれている
int _fseek(FILE *stream, int offsh, int offsl);

// ファイルサイズを返す
int fsize(char *path, int *size);

// TaC 版では stdlib.cmm に記述されている関数
int htoi(char *s);

// C-- の long（int[2]）を C の long に変換する
long lToL(unsigned int l[]);

// printf.c に作り直した
int _fPrintf(FILE *fp, char* fmt, ...);
int _printf(char* fmt, ...);

// RTCのため文字列を変換する必要がある関数
#ifdef _RTC
inline static FILE* __fOpen(_CA* path, _CA* mode) {
  return _fOpen(path->a, mode->a);
}
#endif
