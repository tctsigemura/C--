/*
 * Programing Language C-- "Compiler"
 *    Tokuyama kousen Advanced educational Computer.
 *
 * Copyright (C) 2019 by
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
 * environ.c : C言語版のためのラッパー
 *
 * 2019.11.13 : include/environ.hmm をもとに作成
 *
 * $Id$
 */

#include <stdlib.h>

inline char* getEnv(char* name) {
  return getenv(name);
}

void putEnv(char* string) {
  if (putenv(string) < 0) {
    perror("putEnv");
    exit(1);
  }
}

void setEnv(char* name, char* value, char overwrite) {
  if (setenv(name, value, overwrite) < 0) {
    perror("setEnv");
    exit(1);
  }
}

void unsetEnv(char* name) {
  if (unsetenv(name) < 0) {
    perror("unsetEnv");
    exit(1);
  }
}
