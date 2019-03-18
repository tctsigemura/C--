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
 * printf.c : C言語版のためのラッパー
 *
 * 2019.03.13 : TaC版の printf.cmm をもとに作成
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

// C-- と C の差を可能な限り解消する
typedef unsigned int  boolean ;
#define true  1
#define false 0
#define ord(c) (c)
#define strLen(s) strlen(s)
#define isDigit(c) isdigit(c)

// カウンター付きの出力ルーチン
static FILE *out;                               // 出力ストリーム
static int   cnt;                               // 出力文字数

static void putC(char c) {                      // 文字数カウント付き
  if (!fputc(c, out)) cnt = cnt + 1;            //  出力ルーチン
}

static void putN(char c, int n) {               //  c を n 個出力する
  for (int i=0; i<n; i=i+1)
    putC(c);
}

// 幅指定のありの文字出力
static void putCW(char c, int w, boolean lf) {
  int l = w - 1;
  if (!lf) putN(' ', l);                        // 右詰めなら前に空白
  putC(c);
  if (lf) putN(' ', l);                         // 左詰めなら後で空白
}

// 幅指定のありの文字列出力
static void putS(char* s, int w, boolean lf) {
  int l = w - strLen(s);                        // 必要な空白の文字数
  if (!lf) putN(' ', l);                        // 右詰めなら前に空白
  for (int i=0; s[i]!='\0'; i=i+1)              // 文字列本体の出力
    putC(s[i]);
  if (lf) putN(' ', l);                         // 左詰めなら後で空白
}

// 数値 x を b 進数で出力
static void putNum(int x, int b) {              // 数値を上の桁から順に出力する
  if (x!=0) {
    putNum(x/b, b);                             //   逆順のために再帰使用
    putC("0123456789abcdef"[x%b]);              //   文字列は文字配列
  }
}

// 数値 x が b 進数のとき何桁で表示されるか
static int numLen(int x, int b) {                      
  int l;
  for (l=0; x!=0; l=l+1)
    x = x / b;
  if (l==0) l=1;                                // 最低でも 1 桁
  return l;
}

// 符号なしの数値出力
static void uPutNum(int x, int w, boolean lf, char p, int b) {
  int  l = w - numLen(x, b);                    // 空白がいくつ必要か
  if (!lf) putN(p, l);                          // 右詰めなら前に空白
  if (x!=0) putNum(x, b);                       // 数値を出力
  else putC('0');                               // putNumでは0桁になるので
  if (lf) putN(' ', l);                         // 左詰めなら後で空白
}

// 符号付きの数値出力
static void sPutNum(int x, int w, boolean lf, char p, int b) {
  boolean minus = false;
  if (x<0) {                                    // 値が負なら
    minus = true;                               //  フラグを立てる
    x = -x;                                     //   値は正にする
    w = w - 1;                                  // '-'のために幅1文字減
  }
  int  l = w - numLen(x, b);                    // 空白がいくつできるか
  if (!lf) putN(p, l);                          // 右詰めなら前に空白
  if (minus) putC('-');                         // 負なら符号を表示
  if (x!=0) putNum(x, b);                       // 数値を出力
  else putC('0');                               // putNumでは0桁になるので
  if (lf) putN(' ', l);                         // 左詰めなら後で空白
}

// 可変個引数
va_list va;

// 書式文字列を解析して int を出力する
static void conv16(char c, int w, boolean lf, char p) {
  if (c=='o') {                                 // 8進数として出力
    uPutNum(va_arg(va,int),w,lf,p,8);
  } else if (c=='d') {                          // 10進数として出力
    sPutNum(va_arg(va,int),w,lf,p,10);
  } else if (c=='x') {                          // 16進数として出力
    uPutNum(va_arg(va,int),w,lf,p,16);
  } else if (c=='c') {                          // 文字として出力
    putCW(va_arg(va,int),w,lf);                 //   char は int 扱い
  } else if (c=='s') {                          // 文字列として出力
#ifdef _RTC
    putS(va_arg(va,_CA*)->a,w,lf);
#else
    putS(va_arg(va,char*),w,lf);
#endif
  } else if (c=='%') {                          // '%' を出力
    putC('%');
  } else {
    fputs("Illegal format string\n", stderr);
    exit(1);                                    // どれでもなければエラー
  }
}

// 書式文字列を解析して long（int[]）を出力する
typedef unsigned long ul;
static void conv32(char c, int w, boolean lf, char p) {
#ifdef _RTC
  int* arg = va_arg(va, _IA*)->a;
#else
  int* arg = va_arg(va, int*);
#endif
  if (c=='d') {                                 // 符号なし10進数だけ利用可
    ul n = lToL(arg);                           // C-- の long を long に変換
    int l = n % 100000000;
    n = n / 100000000;
    int m = n % 100000000;
    n = n / 100000000;
    int h = n % 100000000;
    if (h!=0) {
      uPutNum(h, w-16, lf, p,   10);
      uPutNum(m, 8,    lf, '0', 10);
      uPutNum(l, 8,    lf, '0', 10);
    } else if (m!=0) {
      uPutNum(m, w-8,  lf, p,   10);
      uPutNum(l, 8,    lf, '0', 10);
    } else {
      uPutNum(l, w,    lf, p,   10);
    }
  } else {
    fputs("Illegal format string\n", stderr);
    exit(1);                                    // どれでもなければエラー
  }
}

static int print(char *fmt) {
  cnt = 0;                                      // 出力した文字数
  for (int i=0; fmt[i]!='\0'; i=i+1) {          // '\0' が来るまで fmt をたぐる
    char c = fmt[i];                            // fmt から1文字取得する
    if (c=='%') {                               // '%' ならば
      c = fmt[i=i+1];                           //  次の1文字を取得する
      boolean lf = false;                       //  左詰め
      if (c=='-') {
        lf=true;                                //  左詰め
        c = fmt[i=i+1];                         //  次の1文字を取得する
      }
      char  p = ' ';
      if (c=='0') p='0';                        //  先行の0詰め用
      int  w = 0;                               //  幅指定
      while (isDigit(c)) {                      //  数字が来れば
        w = w * 10 + (ord(c) - ord('0'));       //   幅指定がある
        c = fmt[i=i+1];
      }                                         //  cが'\0'の可能性がある！注意
      if (c=='l') {                             //  もしも long なら
        c = fmt[i=i+1];                         //    次の文字に進む
        conv32(c, w, lf, p);                    //  int[] を変換して出力
      } else {
        conv16(c, w, lf, p);                    //  int を変換して出力
      }
    } else                                      // '%' 以外なら
      putC(c);                                  //   そのまま出力 
  }
  return cnt;
}

// 以下が外部に公開される関数
#ifdef _RTC
int _fPrintf(FILE *fp, _CA* fmt, ...) {
  va_start(va, fmt);
  out = fp;                                     // 出力先は fp
  int r = print(fmt->a);
  va_end(va);
  return r;
}

int _printf(_CA* fmt, ...) {
  va_start(va, fmt);
  out = stdout;                                 // 出力先は stdout
  int r = print(fmt->a);
  va_end(va);
  return r;
}
#else
int _fPrintf(FILE *fp, char* fmt, ...) {
  va_start(va, fmt);
  out = fp;                                     // 出力先は fp
  int r = print(fmt);
  va_end(va);
  return r;
}

int _printf(char* fmt, ...) {
  va_start(va, fmt);
  out = stdout;                                 // 出力先は stdout
  int r = print(fmt);
  va_end(va);
  return r;
}
#endif
