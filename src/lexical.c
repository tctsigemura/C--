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
 * lexical.c : C--コンパイラの字句解析ルーチン
 *
 * 2016.04.11         : 小さなメモリ上で動作させるために分割
 * 2016.02.24         : getSharp が str にもファイル名を記録するように変更
 * 2016.02.05 v3.0.0  : トランスレータと統合(LxFILE, LxTYPEDEF 追加)
 * 2016.01.28         : getCh() の始めて呼ばれた時の処理を改良
 * 2012.09.12         : 非公開関数、変数に static 宣言を追加
 * 2011.01.12         : addr 演算子を削除, refer 演算子を ptr 演算子に変更
 * 2010.12.15         : v2.0 に向けて ord, chr, bool, addr, refer 演算子等を追加
 * 2010.07.20         : Subversion による管理を開始
 * 2010.03.12 v0.9.10 : 'public' 修飾子を追加
 * 2009.11.03 v0.9.9  : 'addrof' 演算子と interrupt 型を追加
 * 2009.08.09 v0.9    : '++', '--' 演算子を検出するように変更
 * 2009.07.23 v0.8    : 公開関数をはっきりさせた(lxXXX()が公開)
 * 2009.04.14 v0.7    : "//" コメントに対応
 *                      クオート内のコメント始まりに関するバグ修正
 * 2008.08.26 v0.6    : #ディレクティブに対応
 * 2007.10.10 v0.5    : 初期バージョン
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexical.h"
#include "util.h"

// 予約語の一覧表
struct RsvWord {
  char *name;                                      // つづりと
  int   tok;                                       // トークン値の組
};

static struct RsvWord rsvWord[] = {
  { "void",      LxVOID     }, { "int",       LxINT      },
  { "char",      LxCHAR     }, { "interrupt", LxINTR     },
  { "array",     LxARRAY    }, { "if",        LxIF       },
  { "else",      LxELSE     }, { "while",     LxWHILE    },
  { "for",       LxFOR      }, { "return",    LxRETURN   },
  { "break",     LxBREAK    }, { "continue",  LxCONTINUE },
  { "struct",    LxSTRUCT   }, { "do",        LxDO       },
  { "null",      LxNUL      }, { "boolean",   LxBOOL     },
  { "true",      LxTRUE     }, { "false",     LxFALSE    },
  { "sizeof",    LxSIZEOF   }, { "addrof",    LxADDROF   },
  { "public",    LxPUBLIC   }, { "ord",       LxORD      },
  { "chr",       LxCHR      }, { "bool",      LxBOL      },
  { "typedef",   LxTYPEDEF  }
};

static int  nextch = '\n';                         // 次の文字
static int  ch     = 0;                            // 現在の文字
static int  ln     = 1;                            // 現在の行
static int  ln2    = 0;                            // 現在の行

#define StrMAX     128                             // 名前の長さの上限

static int  val;                                   // 数値を返す場合、その値
static char str[StrMAX + 1];                       // 名前を返す場合、その綴
static char fname[StrMAX + 1];                     // 入力ファイル名
static FILE * fp;                                  // ソースコードファイル
static char outfname[StrMAX +1] = "stdin";

// 一文字を読み込む
static int getCh() {
  ch = nextch;                                     // 次の文字を現在の文字に
  nextch = fgetc(fp);                              // 次の文字を読み込む
  if (ch=='\n') {                                  // 現在の文字が改行なら
    ln2 = ln2 + 1;                                 // 行数を進める
  }
  //if (ch!=EOF) fputc(ch,stderr);                //!!! デバッグ用
  return ch;
}

// Cタイプのコメントをスキップ
static void skipCom() {
  getCh();                                         // '/'を読み飛ばす
  getCh();                                         // '*'を読み飛ばす
  while(ch != '*' || nextch != '/'){               // '*','/'の連続がない間
    if (nextch == EOF)                             // EOFならエラー
      error("コメントの途中で終わった");
    getCh();                                       // 1文字読み飛ばす
  }
  getCh();                                         // '*'を読み飛ばす
  getCh();                                         // '/'を読み飛ばす
}

/* 改行以外の空白を読み飛ばす */
static void skipSpc() {
    while(ch != '\n' && isspace(ch))               // 改行以外の空白文字の間
      getCh();                                     // 1文字読み飛ばす
}

/* 行末まで読み飛ばす */
static void skipToEol() {
    while(ch != '\n' && ch != EOF)                 // 改行かEOFがくるまで
      getCh();                                     // 1文字読み飛ばす
}

/* C++タイプのコメントをスキップ */
static void skipCppCom() {
  skipToEol();                                     // 行末まで読み飛ばす
}

// 空白文字, コメントを読み飛ばす
static void skipSpace(){
  while(true) {
    skipSpc();                                     // 改行以外の空白をスキップ
    if(ch == '/' && nextch == '*')                 // "/*" ならコメント開始
      skipCom();                                   // コメントを読み飛ばし
    else if(ch == '/' && nextch == '/')            // "//" ならコメント開始
      skipCppCom();                                // コメントを読み飛ばし
    else if(ch == '\n' && nextch != '#')           // 単なる改行だったなら
      getCh();                                     // 読み飛ばす
    else break;                                    // '#', "/*", "//", '\n'の
  }                                                // どれでもなければ
}

// 16進数字の値を返す
static int hex(int ch) {
  if (isdigit(ch)) return ch - '0';                // '0'〜'9'の場合
  ch = toupper(ch);                                // 英字は大文字に統一
  return ch - 'A' + 10;                            // 'A'〜'F'の場合
}

// 16進数を読んで値を返す
static int getHex() {
  if (!isxdigit(ch))                               // 0x の次に16進数がない
    error("16進数の形式");
  int val = 0;                                     // 初期値は 0
  while (isxdigit(ch)) {                           // 16進数字の間
    val = val*16 + hex(ch);                        // 値を計算
    getCh();                                       // 次の文字を読む
  }
  return val;                                      // 16進数の値を返す
}

// 8進数を読んで値を返す
static int getOct() {
  int val = 0;                                     // 初期値は 0
  while (isOdigit(ch)) {                           // 8進数字の間
    val = val*8 + ch - '0';                        // 値を計算
    getCh();                                       // 次の文字を読む
  }
  return val;                                      // 8進数の値を返す
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

// 数値を読み込む
static int getDigit() {
  val = 0;
  if(ch=='0' && (nextch=='x' || nextch=='X')) {    // '0x' で始まれば16進数
    getCh();                                       // '0'を読み飛ばす
    getCh();                                       // 'x'を読み飛ばす
    val = getHex();                                // 16進数の読み込み
  } else if(ch=='0') {                             // '0' で始まれば8進数
    val = getOct();                                // 8進数を読み込む
  } else {                                         // それ以外は10進のはず
    val = getDec();                                // 10進数を読み込む
  }
  return LxINTEGER;                                // tok=LxINTEGER, val=値
}

// #ディレクティブの処理
static int getSharp() {
  getCh();                                         // '\n'を読み飛ばす
  getCh();                                         // '#' を読み飛ばす
  skipSpc();                                       // 改行以外の空白をスキップ
  if (!isdigit(ch)) error("#に行番号がない");      // 行番号があるはず
  ln2 = getDec();                                  // 行番号の読み込み
  skipSpc();                                       // 改行以外の空白をスキップ
  if (ch!='"') error("#にファイル名がない");       // ファイル名は " で始まる
  getCh();                                         // '"'を読み飛ばす
  int i;
  for (i=0; i<=StrMAX; i=i+1) {                    // ファイル名を読み込む
    if (ch=='"' || ch=='\n' || ch==EOF) break;     // 終わったら break
    fname[i] = str[i] = ch;                        // ファイル名を記憶
    getCh();
  }
  fname[i] = str[i] = '\0';                        // ファイル名を完成する
  if (ch!='"') error("#の\"が閉じてないか長すぎる");
  skipToEol();                                     // 行末まで読み飛ばす
  return LxFILE;                                   // ファイル名を読んだ
}

// 名前か予約語を読み込む
static int getWord(){
  int i;
  for (i=0; !isDelim(ch) && i<=StrMAX; i=i+1) {    // 区切り文字以外なら
    str[i] = ch;                                   // strに読み込む
    getCh();                                       // 1文字読み飛ばす
  }
  if (!isDelim(ch)) {                              // 区切り文字以外なのに終
    error("名前が長すぎる");                       // 了していれば名前の長さ
  }                                                // が長すぎる場合
  str[i] = '\0';                                   // 文字列を完成させる
  for (i=0; i<LxRsvNUM; i=i+1) {                   // 予約語の一覧表を捜す
    if (!strcmp(str, rsvWord[i].name))             // 同じつづりが見付かれば
      return rsvWord[i].tok;                       // それのトークン値を返す
  }                                                // 予約語以外なら
  return LxNAME;                                   // tok=LxNAME, str=値
}

// エスケープ文字の読み込み
static int getEsc() {
  int n  = 0;                                      // 文字コードの初期値は 0
  getCh();                                         // '\' を読み飛ばす
  if (ch == 'n'){                                  // '\n' の場合
    n = '\n';                                      // n に LF を格納
    getCh();                                       // 'n' を読み飛ばす
  } else if (ch == 't'){                           // '\t' の場合
    n = '\t';                                      // n に TAB を格納
    getCh();                                       // 't' を読み飛ばす
  } else if (ch == 'r'){                           // '\r' の場合
    n = '\r';                                      // n に CR を格納
    getCh();                                       // 'r' を読み飛ばす
  } else if (ch == 'x' || ch == 'X') {             // '\x' の場合
    getCh();                                       // 'x' を読み飛ばす
    n = getHex();                                  // n に16進数を読み込む
  } else if (isOdigit(ch)) {                       // '\数値' の場合
    n = getOct();                                  // n に8進数を読み込む
  } else if (isprint(ch)) {                        // そのほか印刷可能文字の
    n = ch;                                        // 場合'\c'は'c'と同じ
    getCh();                                       // 文字を読み飛ばす
  } else error("未定義のエスケープ文字");          // それ以外はエラー
  return n;                                        // 文字コードを返す
}

// 文字列 "..." の読み込み
static int getStr(){
  int i = 0;
  while (getCh()!='"' && ch!=EOF && i<=StrMAX) {   // (")以外の間
    if (iscntrl(ch)) {                             // ch が制御文字なら
      break;                                       // 異常終了
    } else if (ch=='\\' && nextch=='"') {          // (\")はそのまま読み込み
      str[i] = '\\';                               // アセンブラのソースに
      i = i + 1;                                   // そのまま出力する
      str[i] = '\"';
      getCh();                                     // 通常より1文字余分に読む
    } else {
      str[i] = ch;                                 // str に格納する
    }
    i = i + 1;
  }
  if (ch!='\"') {                                  // 前のwhileが(")以外で終了
    error("文字列が正しく終わっていないか、長すぎる");
  }
  getCh();                                         // (")を読み飛ばす
  str[i] = '\0';                                   // 文字列を完成する
  return LxSTRING;                                 // tok=LxSTRING, str=値
}

// 文字定数 'x' を読み込む
static int getChar(){
  getCh();                                         // コート(')を読み飛ばす
  if(ch == '\\'){                                  // コートの次が '\\' なら
    val = getEsc();                                // エスケープ文字
  }else{                                           // そうでなければ ch が
    val = ch;                                      // 定数値になる
    getCh();                                       // ch を読み飛ばす
  }
  if(ch != '\'')                                   // コート(')で文字定数が
    error("文字が正しく終わっていない");           // 終了しているか
  getCh();                                         // コート(')を読み飛ばす
  return LxCHARACTER;                              // tok=LxCHARACTER, val=値
}

// 記号を読み込む
static int getSign(){
  int tok = LxNONTOK;                              // tok の初期値はエラー
  if (ch == '=' && nextch == '=') {                // '==' なら
    getCh();                                       // 1文字余計に読み飛ばす
    tok = LxEQU;                                   // tok=EQU
  } else if (ch == '|' && nextch == '|') {         // '||' なら
    getCh();                                       // 1文字余計に読み飛ばす
    tok = LxOR;                                    // tok=OR
  } else if (ch == '&' && nextch == '&') {         // '&&' なら
    getCh();                                       // 1文字余計に読み飛ばす
    tok = LxAND;                                   // tok=AND
  } else if (ch == '!' && nextch == '=') {         // '!=' なら
    getCh();                                       // 1文字余計に読み飛ばす
    tok = LxNEQ;                                   // tok=NEQ
  } else if (ch == '<' && nextch == '=') {         // '<=' なら
    getCh();                                       // 1文字余計に読み飛ばす
    tok = LxLTE;                                   // tok=LTE
  } else if (ch == '>' && nextch == '=') {         // '>=' なら
    getCh();                                       // 1文字余計に読み飛ばす
    tok = LxGTE;                                   // tok=GTE
  } else if (ch == '<' && nextch == '<') {         // '<<' なら
    getCh();                                       // 1文字余計に読み飛ばす
    tok = LxSHL;                                   // tok=SHL
  } else if (ch == '>' && nextch == '>') {         // '>>' なら
    getCh();                                       // 1文字余計に読み飛ばす
    tok = LxSHR;                                   // tok=SHR
  } else if (ch == '+' && nextch == '+') {         // '++' なら
    getCh();                                       // 1文字余計に読み飛ばす
    tok = LxPP;                                    // tok=PP
  } else if (ch == '-' && nextch == '-') {         // '--' なら
    getCh();                                       // 1文字余計に読み飛ばす
    tok = LxMM;                                    // tok=MM
  } else if (ch == '.' && nextch == '.') {         // '..' は '...' の候補
    getCh();                                       // 1文字余計に読み飛ばす
    if (nextch == '.') {                           // 3文字目も '.' なら
      getCh();                                     // もう1文字読み飛ばす
      tok = LxDOTDOTDOT;                           // tok=DOTDOTDOT
    } else tok = LxDOTDOT;                         // 構文解析でエラーになる
  } else {                                         // それ以外の記号は
    tok = ch;                                      // tok=文字コード
  }
  getCh();                                         // 最低1文字は読み飛ばす
  return tok;                                      // tok を返す
}

/*
 *  外部インタフェース用の関数
 */
int lxGetTok(){                                    // トークンを取り出す
  int tok = LxNONTOK;                              // tok の初期値はエラー
  if (ch==0)                                       // はじめて呼び出された
    getCh();                                       // ch と nextch の準備
  skipSpace();                                     // 空白文字をスキップする
  ln = ln2;                                        // 現在の行番号を管理
  if(ch == EOF){                                   // ch が EOF なら
    tok = EOF;                                     // tok=EOF
  } else if(ch == '\n' && nextch == '#') {         // # ディレクティブは
    tok = getSharp();                              // 行頭でなければならない
  } else if(isdigit(ch)) {                         // ch が '0'〜'9'なら
    tok = getDigit();                              // 10進数の読み込み
  } else if(isalpha(ch) || ch == '_') {            // chがアルファベットなら
    tok = getWord();                               // 予約語か名前を読み込む
    if (tok==LxTRUE) { tok=LxLOGICAL; val=1; }     // 予約語 trueとfalse は
    else if (tok==LxFALSE) {tok=LxLOGICAL; val=0;} // tok=LOGICAL, val=0/1
  } else if(ch == '\"') {                          // ch が (") なら
    tok = getStr();                                // 文字列を読み込む
  } else if(ch == '\'') {                          // ch が (') なら
    tok = getChar();                               // 文字定数を読み込む
  } else {                                         // ch がそれ以外なら
    tok = getSign();                               // 記号を読み込む
  }
  return tok;                                      /* トークン値を返す       */
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

int main(int argc, char *argv[]){
  FILE *fpin, *fpout;
  int tok; 
  if (argc==2) {                             // 引数としてソースファイルがある
    if((fpin = fopen(argv[1],"r")) == NULL){ // ソースファイルをオープン
      perror(argv[1]);                       // オープン失敗の場合は、メッ
      exit(1);                               //   セージを出力して終了
    }
    lxSetFname(argv[1]);                     // error表示用にファイル名を登録
    int i;
    for(i=0; i<=StrMAX; i=i+1){
      outfname[i] = argv[1][i];
      if(outfname[i]=='\0') break;
    }
    if (outfname[i]!='\0') error("ファイル名が長すぎる");
    if (strEndsWith(outfname, ".cmm"))
      outfname[strlen(outfname) - 4]='\0';
  } else if (argc==1) {
    fpin = stdin;
    lxSetFname("STDIN");                     // error表示用にファイル名を登録
  } else {
    //usage(argv[0]);
    exit(1);
  }
  sprintf(outfname,"%s.lx",outfname);
  fpout = fopen(outfname, "w");
  lxSetFp(fpin);                               // 字句解析に fp を知らせる
  fprintf(fpout, "%d\t%d\t%s\n", lxGetLn(), LxFILE, lxGetFname());
  while ((tok = lxGetTok())!=EOF){                  // EOF になるまで読む
    if(tok == LxNAME || tok == LxSTRING){
        int i = 0;
        fprintf(fpout, "%d\t%d\t", lxGetLn(), tok);
        while(str[i]){
          if(str[i]=='\n')
            fprintf(fpout, "\\n");
          else
            fprintf(fpout, "%c", str[i]);
          i = i+1;
        }
        fprintf(fpout, "\n");
    }else if(tok == LxINTEGER || tok == LxLOGICAL)
      fprintf(fpout, "%d\t%d\t%d\n", lxGetLn(), tok, lxGetVal());
    else if(tok == LxCHARACTER){
      fprintf(fpout, "%d\t%d\t%d\n", lxGetLn(), tok, lxGetVal());
    }else if(tok == LxFILE){
      fprintf(fpout, "%d\t%d\t%s\n", lxGetLn(), tok, lxGetFname());
    }else
      fprintf(fpout, "%d\t%d\n", lxGetLn(), tok);   // 中間ファイルに出力
  }
  fprintf(fpout, "%d\t%d\n", lxGetLn(), EOF);          // ファイル終わり
  return 0;
}
