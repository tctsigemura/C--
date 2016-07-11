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
 * sn.c : C--コンパイラの構文・意味解析ルーチンのドライバ
 *
 * 2016.04.30         : 初期バージョン
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "util.h"
#include "../util.h"
#include "../lexical.h"         // lexical.c と見かけが同じ関数を提供する
#include "../code.h"            // vmCcode.c と見かけが同じ関数を提供する
#include "../syntax.h"          // syntax.c  の関数を使用する
#include "../namtbl.h"          // namtbl.c  の関数を使用する

/*
 * 段階コンパイラ版のために lexical.c の関数を提供する
 */
//static int  nextch = '\n';                         // 次の文字
static int  ch     = 0;                            // 現在の文字
static int  ln     = 1;                            // 現在の行
static int  val;                                   // 数値を返す場合、その値
static char str[StrMAX + 1];                       // 名前を返す場合、その綴
static char fname[StrMAX + 1];                     // 入力ファイル名
static FILE * fp;                                  // 入力ファイル
static FILE * fpout;                               // 出力ファイル


// 一文字を読み込む
static int getCh() {
//  ch = nextch;                                     // 次の文字を現在の文字に
//  nextch = fgetc(fp);                              // 次の文字を読み込む
  ch = fgetc(fp);
  return ch;
}

/* 改行以外の空白を読み飛ばす */
static void skipSpc() {
  while(ch != '\n' && isspace(ch))               // 改行以外の空白文字の間
    getCh();                                     // 1文字読み飛ばす
}

// 10進数を読んで値を返す
static int getDec() {
  int val = 0;                                     // 初期値は 0
  while (isdigit(ch)) {                            // 10進数字の間
    val = val*10 + ch - '0';                       // 値を計算
    getCh();                                       // 次の文字を読む
  }
  return val;                                      // 10進数の値を返す
}

// 名前か文字列を読み込む
static void getStr(){
  int i;
  for (i=0; ch!='\n' && ch!=EOF; i=i+1) {            // 行末まで
    if (i>StrMAX) error("名前が長すぎる");
    str[i] = ch;                                     // strに読み込む
    getCh();                                         // 1文字読み飛ばす
  }
  str[i] = '\0';                                     // 文字列を完成させる
}

//  外部インタフェース用の関数
int lxGetTok(){                                    // トークンを取り出す
  int tok = LxNONTOK;                              // tok の初期値はエラー
  getCh();                                         // 文字を読んでおく
  if(ch == EOF){                                   // ch が EOF なら
    tok = EOF;                                     // tok=EOF
  } else {
    ln = getDec();                                 // 行番号
    skipSpc();
    tok = getDec();                                // トークンの種類
    skipSpc();
    if (tok==LxNAME||tok==LxSTRING||tok==LxFILE)   // 名前か文字列
      getStr();
    else if (tok==LxLOGICAL||tok==LxINTEGER||
        tok==LxCHARACTER)
      val = getDec();
//    getCh();                                       // '\n' を読み飛ばす
  }
  //printf("%d : %d\n", ln, tok);                    // #デバッグ用#
  return tok;                                      // トークン値を返す
}

void lxSetFname(char s[]) {                    // 入力ファイル名をセットする
  int i;
  for (i=0; i<=StrMAX; i=i+1) {
    fname[i] = s[i];
    if (fname[i]=='\0') break;
  }
  if (fname[i]!='\0') error("ファイル名が長すぎる");
}

char *lxGetFname() { return fname; }           // 入力ファイル名を読み出す

int lxGetLn() { return ln; }                   // 行番号を返す

int lxGetVal() { return val; }                 // 数値等を読んだときの値を返す

char *lxGetStr() { return str; }               // 名前、文字列の綴を返す

void lxSetFp(FILE *p) { fp = p; }              // fp をセットする


/*
 * 段階コンパイラ版のために code.c の関数を提供する
 */

// 関数１個分のコード生成
void genFunc(int funcIdx, int depth, boolean krnFlg) {
  printTree(fpout);
  fprintf(fpout, "%d F %d %d %d\n", lxGetLn(), funcIdx, depth, krnFlg);
}
// 初期化データの生成
void genData(int idx) {
  printTree(fpout);
  fprintf(fpout, "%d D %d\n", lxGetLn(), idx);
}

// 非初期化データの生成
void genBss(int idx) {
  printTree(fpout);
  fprintf(fpout, "%d B %d\n", lxGetLn(), idx);
}

// ラベルを割り当てる
static int curLab = 0;
static int newLab() {
  curLab = curLab + 1;
  return curLab;
}

// 文字列を生成しラベル番号を返す
int genStr(char *str) {
  int lab = newLab();                            // ラベルを割り付け
  int i=0;
  fprintf(fpout, "%d S ", lxGetLn());
  while(str[i]){
    fprintf(fpout, "%c", str[i]);
    i =i+1;
  }
  fprintf(fpout, "\n");
  return lab;                                    //   ラベル番号を返す
}

// 段階版のためにダミーのoptTree
void optTree(int node){}

int main(int argc, char *argv[]) {
  // FILE *fp;
  if (argc==2){
    if (!strEndsWith(argv[1], ".lx")) error("入力ファイル形式が違う");
    fp = eOpen(argv[1],"r");                 // 中間ファイルをオープン
    lxSetFname(argv[1]);
  }else if (argc==1){
    fp = stdin;
    lxSetFname("STDIN");
  }else{
    fprintf(stderr, "使用方法 : %s [<srcfile>]\n", argv[0]);
    exit(1);
  }
  char *fn = lxGetFname();
  fpout = openDstWithExt(lxGetFname(), ".sm");// 拡張子を".sm"に変更してOpen
  lxSetFp(fp);                               // 字句解析に fp を知らせる
  snGetSrc();                                // fp からソースコードを入力して
  //   stdout へ出力
  ntPrintTable(fn);                          // 最終的な名前表をファイル出力
  return 0;
}
 
