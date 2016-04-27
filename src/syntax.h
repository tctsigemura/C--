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
 * syntax.h : C--コンパイラの構文解析ルーチン
 *
 * 2016.02.03         : 新規作成
 *
 * $Id$
 *
 */

void snGetSrc(void);                       // C-- プログラムを読む
void snSetOptFlag(boolean f);              // 最適化フラグを操作する
void snSetKrnFlag(boolean f);              // カーネルフラグを操作する

FILE * fpout;                // 出力ファイル

int   lxGetTok(void);          // 字句解析ルーチン
void  lxSetFname(char *s);     // ファイル名を記憶させる
int   lxGetVal(void);          // 数値等を読んだときの値を返す
char *lxGetStr(void);          // 名前、文字列の綴を返す
void  lxSetFp(FILE *p);        // fp をセットする
