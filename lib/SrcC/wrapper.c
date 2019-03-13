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
 * wrapper.c : C-- 版と C 版で仕様が異なる関数など
 *            (主に stdlib.hmm の関数）
 *
 * 2019.03.11 : 実行時エラーチェック関数inline関数に変更しwrapper.hに移動
 * 2019.03.11 : string.cmm の関数を削除(C--版を使用する）
 * 2019.02.23 : 実行時エラーチェック関数を追加
 * 2019.01.27 : htoi を追加
 * 2018.11.17 : lToL を追加
 * 2018.02.26 : fsize を追加
 * 2018.02.20 : fseek を追加
 * 2016.08.07 : feof を追加
 * 2016.05.26 : #include <wrapper.h> を削除（cc のオプションで指定する）
 * 2016.05.26 : #include <wrapper.h> を追加
 * 2016.03.02 : mAlloc, fOpen を追加
 * 2016.02.26 : 初期バージョン
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _RTC
// C-- で記述した main 関数
int __main(int argc, _RA* args, _RA* envs);

// main 関数
int main(int argc, char *argv[], char *envp[]) {
  // argv を RTC 対応のデータ構造に作り直す
  _RA *args = _mAlloc(sizeof(_RA)+sizeof(void*)*(argc+1));
  args->l = argc + 1;
  args->a[argc] = NULL;
  for (int i=0; i<argc; i=i+1) {
    args->a[i] = _mAlloc(sizeof(_CA)+strlen(argv[i])+1);
    ((_CA*)(args->a[i]))->l = strlen(argv[i])+1;
    strcpy(((_CA*)(args->a[i]))->a, argv[i]);
  }

  // envpの長さを調べる
  int envc=0;
  while (envp[envc]!=NULL) {
    envc = envc + 1;
  }

  // envp を RTC 対応のデータ構造に作り直す
  _RA *envs = _mAlloc(sizeof(_RA)+sizeof(_RA*)*(envc+1));
  envs->l = envc + 1;
  envs->a[envc] = NULL;
  for (int i=0; i<envc; i=i+1) {
    envs->a[i] = _mAlloc(sizeof(_CA)+strlen(envp[i])+1);
    strcpy(((_CA*)(envs->a[i]))->a, envp[i]);
  }

  return __main(argc, args, envs);
}
#endif

// TaC 版ではエラーチェックして終了する
void * _mAlloc(int s) {
  void *p = malloc(s);
  if (p==NULL) {
    perror("malloc");
    exit(1);
  }
  return p;
}

// TaC 版ではエラー原因により null を返すので、以下では再現しきれていない
FILE *_fOpen(char *n, char *m) {
  FILE *fp = fopen(n, m);
  if (fp==NULL) {
    perror("fopen");
    exit(1);
  }
  return fp;
}

// TaC 版では EOF になるタイミングが早い
int _feof(FILE *fp) {
  int c = fgetc(fp);
  if (c==EOF) return 1;
  ungetc(c, fp);
  return 0;
}

// TaC 版ではオフセットが上位と下位に分かれている
int _fseek(FILE *stream, int offsh, int offsl){
  return fseek(stream,((long)offsh<<32)|(long)offsl,SEEK_SET);
}

// ファイルのサイズを返す
int fsize(char *path, int *size) {
  struct stat sbuf;
  if (lstat(path, &sbuf)<0) return 1;
  size[0] = (sbuf.st_size >> 32) & 0xffffffff;
  size[1] = sbuf.st_size & 0xffffffff;
  return 0;
}

// TaC 版では stdlib.cmm に記述されている関数
int htoi(char *s) {
  int v = 0;
  for (int i=0; isxdigit(s[i]); i=i+1) {
    char c = s[i];
    if (isdigit(c)) {                            // 0-9
      v = v * 16 + (c-'0');
    } else {                                     // a-f または A-F
      v = v * 16 + (10+toupper(c)-'A');
    }
  }
  return v;
}

// C-- の long を C の long にする
long lToL(unsigned int l[]) {
  return (((long)l[0])<<32)|l[1];
}
