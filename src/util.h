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
 * util.h : よく使う関数や定数の定義
 *
 * 2016.03.01         : null の値を 0 から NULL に変更
 * 2016.02.05 v3.0.0  : トランスレータと統合(strEndsWith 関数を追加)
 * 2015.08.07         : WMSK, BMSK に unsigned を追加
 * 2012.12.30         : error2 を追加
 * 2012.08.14         : WMSK, BMSK の定義を追加
 * 2010.07.20         : Subversion による管理を開始
 * 2010.03.30 v0.9.11 : NOTYPE を nametbl.h からここに移動
 *
 * $Id$
 *
 */

// バッファサイズ等
#define StrMAX      128        // 文字列、名前の最大長

// C-- 風に C 言語で記述するための定義
#define boolean  int
#define true     1
#define false    0
#define null     NULL

// ターゲットの有効ビット
#define WMSK (((unsigned)1<<(NWORD-1)|(((unsigned)1<<(NWORD-1))-1))) // ワード
#define BMSK (((unsigned)1<<(NBYTE-1)|(((unsigned)1<<(NBYTE-1))-1))) // バイト

// 良く使う関数
void error(char s[]);                     // エラーメッセージを表示後終了
void error2(char s1[], char s2[]);        // エラーメッセージを表示後終了
void *ealloc(int s);                      // エラーチェックつきの malloc
int isDelim(int ch);                      // 区切り記号かテスト
int isOdigit(int ch);                     // 8進数字かテスト
boolean strEndsWith(char *s, char *sfx);  // 文字列最後の一致をテスト
