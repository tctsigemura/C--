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
 * wrapper.h : wrapper.c 関数のプロトタイプ宣言
 *
 * 2016.08.07 : feof を追加
 * 2016.05.26 : 初期バージョン
 *
 */

void *_addrAdd(void *a, int inc);
void *_aToA(void *a);

// 以下は 64bit 環境では危険
/*
int _aToI(void *a);
void *_iToA(int i);
*/

// TaC 版ではエラーチェックして終了する
void * _mAlloc(int s);

/* TaC 版ではエラー原因により null を返すので、
 * 以下では再現しきれていない
FILE *_fOpen(char *n, char *m);
*/

// TaC 版では EOF になるタイミングが C 言語より早い
int _feof(FILE *fp);

// TaC 版では string.cmm に記述されている関数
int strChr(char *s, int c);
int strRchr(char *s, int c);
int strStr(char *s1, char *s2);
char *subStr(char *s, int pos);
