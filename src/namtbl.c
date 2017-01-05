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
 * namtbl.c : 名前表(Name Table)の管理プログラム
 *
 * 2016.07.10         : 中間ファイル用の出力・読込関数を追加(統合版では不使用)
 * 2015.08.31 v2.1.0  : CLang の警告が出ないように変更
 * 2012.12.30         : error2 関数を使用するように変更
 * 2010.07.20         : Subversion による管理を開始
 * 2010.03.30 v0.9.11 : ntOffs を ntCnt に変更
 * 2010.03.12 v0.9.10 : public 修飾に対応
 * 2009.07.22 v0.8.0  : 初期バージョン(syntax.c から分離)
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include "StepByStep/util.h"
#include "namtbl.h"

#define StrMAX 128

int ntPointer(int idx) {
  if(ntType[idx]==TyPNT) return ntCnt[idx];
  return idx;
}

// 定義済みの関数名、変数名、仮引数名を探す(getFactorから呼ばれる)
int ntSrcName(char *str) {
  for (int i=ntNextIdx-1; i>=0; i=i-1)           // 表の最後(深いスコープ)から
    if (ntScope[i]>=ScPROT &&                    // 順に、関数名、大域変数、
	strcmp(ntName[i], str)==0) return i;     // 局所変数、現在の関数の仮引
  error2("未定義の名前", str);                   // 数から同じ名前のものを探す
  return -1;                                     // ここが実行されることはない
}

// 構造体名を探し型を返す(構造体型は非正値, isTypeから呼ばれる)
int ntSrcStruct(char *str) {
  for (int i=0; i<ntNextIdx; i=i+1)              // 表全体について
    if (ntScope[i]==ScSTRC &&                    // 構造体のスコープで
	strcmp(ntName[i], str)==0) return -i;          // 名前の同じものをさがす
  return TyNON;                                  // 見付からない場合は TyNON
}

// 構造体(n)からフィールド名を探す(getPostOPから呼ばれる)
int ntSrcField(int n, char *str) {               // n は構造体型(非正値)
  for (int i=-n+1; i<=-n+ntCnt[-n]; i=i+1)       // ntCnt[-n]はフィールド数
    if (strcmp(ntName[i], str)==0) return i;     // フィールド名と比較
  error2("未定義の構造体フィールド名", str);
  return -1;                                     // ここが実行されることはない
}

// 構造体参照時のフィールド名を探す (名前しか使わないため他は何でもよい)
int ntSrcFieldName(char *str) {
 for (int i=0; i<ntNextIdx; i=i+1)               // 表全体について
	if(strcmp(ntName[i], str)==0) return i;        // 名前が同じものを探す
  error2("未定義の構造体フィールド名.", str);
  return -1;                                     // ここが実行されることはない
}

// 大域名(n)と２重定義になる関数名、大域変数名を探す
int ntSrcGlob(int n) {                           // n は名前表の添字
  for (int i=0; i<n; i=i+1) {                     // n より前にあるはず
    if ((ntScope[i]==ScPROT || ntScope[i]==ScFUNC ||
	 ntScope[i]==ScCOMM || ntScope[i]==ScGVAR) &&
	strcmp(ntName[i], ntName[n])==0)
      return i;                                  // 見つかった
  }
  return -1;                                     // 見つからなかった
}
// ２重定義にならないかチェック(ntDefNameから呼ばれ、関数名と大域変数名を除く)
static int ntChkName(char *str, int scope) {
  if(strcmp("",str)==0) return -1;
  for (int i=0; i<ntNextIdx; i=i+1) {            // 表全体について
    if (ntScope[i]!=ScVOID && strcmp(ntName[i],str)==0){// 衝突の可能性を調べる
      if (scope==ScSTRC||ntScope[i]==ScSTRC)     // 構造体名は他の全てと衝突
        return i;
      if (scope>=ScLVAR)                         // 局所変数、仮引数、構造体
        if (ntScope[i]==scope) return i;         // フィールド名はスコープが
    }                                            // 同じものとだけ衝突
  }
  return -1;
}

//if (curScope!=ScPROT&&curType==VOID&&curDim==0)// 関数以外では void型はポイ
//  error("void型変数/引数は使用できない");      // ンタだけ
//curName         = name[nextIdx];               // 最後に登録した名前

// 名前を登録する(getName, getParams から呼ばれる)
void ntDefName(char *name, int scope, int type, int dim, int cnt, boolean pub){
  if (ntNextIdx>=NtMAX) error("名前が多すぎる"); // 名前表がパンクした
  if (ntChkName(name, scope)>=0) error2("名前の２重定義", name);
  ntName[ntNextIdx]= ealloc(strlen(name)+1);     // 名前のために領域を割り当て
  strcpy(ntName[ntNextIdx], name);               // 名前をコピーする
  ntScope[ntNextIdx] = scope;                    // スコープを登録する
  ntType[ntNextIdx]  = type;                     // 型を登録する
  ntDim[ntNextIdx]   = dim;                      // 次元を登録する
  ntCnt[ntNextIdx]   = cnt;                      // 値を登録する
  ntPub[ntNextIdx]   = pub;                      // 外部名
  ntNextIdx = ntNextIdx + 1;
}

/* 名前表をもとに戻す */
void ntUndefName(int idx) {                      // idx まで戻す
  for (int i=idx; i<ntNextIdx; i=i+1)            // 名前のつづり用の領域を順に
    free(ntName[i]);                            // 解放していく
  ntNextIdx = idx;                               // nextIdx を戻せば完成
}
void ntDebPrintTable(){
  for (int i=0; i<ntNextIdx; i=i+1)              // 表全体について
    printf("%s %d %d %d %d %d\n"
      , ntName[i], ntScope[i], ntType[i], ntDim[i], ntCnt[i], ntPub[i]);
}


//---------------------------------------------------------------------------
// STEP BY STEPの場合は以下の関数が必要
//---------------------------------------------------------------------------

#ifdef STEPBY
// 10進数を読んで値を返す
static int getDecf(FILE *fp) {
  int v = 0;                                     // 初期値は 0
  char ch = fgetc(fp);
  boolean minusflg = false;
  if(ch=='-'){
    minusflg = true;
    ch = fgetc(fp);
  }
  while (isdigit(ch)) {                          // 10進数字の間
    v = v*10 + ch - '0';                         // 値を計算
    ch = fgetc(fp);                              // 次の文字を読む
  }
  if(minusflg) return -v;
  return v;                                      // 10進数の値を返す
}

/* 名前表の出力 */
void ntPrintTable(char *name){
  FILE *fp;
  fp = openDstWithExt(name, ".nt");              // 拡張子を".nt"にしてOpen
  for (int i=0; i<ntNextIdx; i=i+1)              // 表全体について
    fprintf(fp, "%s %d %d %d %d %d\n"
      , ntName[i], ntScope[i], ntType[i], ntDim[i], ntCnt[i], ntPub[i]);
}

/* 名前表の読み込み */
void ntLoadTable(char *name){
  FILE *fp;
  int scp, type, dim, val, pub;
  char c;
  char str[StrMAX + 1];
  char ntfName[StrMAX + 1];
  strcpy(ntfName, name);
  char *lastDot = rindex(ntfName, '.');
  if(lastDot!=NULL) *lastDot='\0';
  if (strlen(ntfName)+strlen(".nt")>StrMAX) error("ファイル名長い");
  strcat(ntfName, ".nt");
  fp = eOpen(ntfName, "r");
  while((c=fgetc(fp))!=EOF){
    int i=0;
    while(c!=' '){
      if(i>StrMAX) error("名前が長すぎる");
      str[i] = c;
      i = i+1;
      c = fgetc(fp);
    }
    str[i] = '\0';
    scp  = getDecf(fp);
    type = getDecf(fp);
    dim  = getDecf(fp);
    val  = getDecf(fp);
    pub  = getDecf(fp);
    ntDefName(str, scp, type, dim, val, pub);
  }
}
#endif    // STEPBY
