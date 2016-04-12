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
 * lexical.h : 字句解析部の外部仕様を定義
 *
 * 2016.02.05 v3.0.0  : トランスレータ用と統合(LxTYPEDEF, LxFILE 追加)
 * 2011.01.12         : addr, refer 演算子を削除
 * 2010.12.15         : v2.0 に向けて ord, chr, bool, addr, refer 演算子等を追加
 * 2010.07.20         : Subversion による管理を開始
 * 2010.03.12 v0.9.10 : 'public' 修飾子を追加
 * 2009.11.03 v0.9.9  : 'addrof' 演算子と interrupt 関数を追加
 * 2009.08.09 v0.9    : '++','--' 演算子を追加
 * 2009.07.22 v0.8    : インタフェースの明確化
 *
 * $Id$
 *
 */

/*
 * 字句解析が返す値の定義
 */
#define LxVOID      1          // void
#define LxINT       2          // int
#define LxCHAR      3          // char
#define LxINTR      4          // interrupt
#define LxSTRUCT    5          // struct
#define LxARRAY     6          // array
#define LxIF        7          // if
#define LxELSE      8          // else
#define LxWHILE     9          // while
#define LxFOR       10         // for
#define LxRETURN    11         // return
#define LxBREAK     12         // break
#define LxCONTINUE  13         // continue
#define LxDO        14         // do
#define LxNUL       15         // null
#define LxBOOL      16         // boolean
#define LxTRUE      17         // true    字句解析内部で使用（返すことはない）
#define LxFALSE     18         // false   字句解析内部で使用（返すことはない）
#define LxSIZEOF    19         // sizeof
#define LxADDROF    20         // addrof
#define LxPUBLIC    21         // public
#define LxORD       22         // ord
#define LxCHR       23         // chr
#define LxBOL       24         // bool
#define LxTYPEDEF   25         // typedef
#define LxRsvNUM    25         // 予約語の個数

#define LxOR        101        // '||'
#define LxAND       102        // '&&'
#define LxEQU       103        // '=='
#define LxNEQ       104        // '!='
#define LxLTE       105        // '<='
#define LxGTE       106        // '>='
#define LxSHL       107        // '<<'
#define LxSHR       108        // '>>'
#define LxDOTDOT    109        // '..'
#define LxDOTDOTDOT 110        // '...'
#define LxPP        111        // '++' 使用しないが誤使用発見のため
#define LxMM        112        // '--' 使用しないが誤使用発見のため

#define LxLOGICAL   200        // true の場合 val=1 , false の場合 val=0
#define LxINTEGER   201        // 整数
#define LxNAME      202        // 変数名、関数名
#define LxSTRING    203        // "..."
#define LxCHARACTER 204        // 文字
#define LxFILE      205        // ファイル名を指示するディレクティブ

#define LxNONTOK    256        // トークン値として表れない値
/*
int   lxGetTok(void);          // 字句解析ルーチン
void  lxSetFname(char *s);     // ファイル名を記憶させる
char *lxGetFname(void);        // 入力ファイル名を読み出す
int   lxGetLn(void);           // 行番号を返す
int   lxGetVal(void);          // 数値等を読んだときの値を返す
char *lxGetStr(void);          // 名前、文字列の綴を返す
void  lxSetFp(FILE *p);        // fp をセットする
*/
