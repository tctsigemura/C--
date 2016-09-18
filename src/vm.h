/*
 * Programing Language C-- "Compiler"
 *    Tokuyama kousen Educational Computer 16bit Ver.
 *
 * Copyright (C) 2002-2015 by
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
 * vm.h : 仮想マシンのコードから実際のコードを生成するプログラムで準備
 *        すべき関数のプロトタイプ
 *
 * 2016.09.19         : vmEntry, vmEntryK, vmEntryI変更（ラベルを出力しない）
 * 2016.09.18         : vmLdLabをvmLdNam に変更
 *                    : vmLdStrをvmLdLab に変更
 *                    : vmTmpLabをvmLab に変更
 *                    : vmNameをvmNam に変更
 *                    : vmCharをvmChr に変更
 * 2016.05.04         : vmLdArg, vmStArg を vmLdPrm, vmStPrm(パラメータ)に変更
 * 2015.08.31 v2.1.0  : vmEntryK を追加
 * 2010.12.23         : 仮想マシンのニーモニックを大幅に変更
 *                      変更(JNZ=>JT,JZ=>JF,DC=>DW,DS=>WS)
 *                      追加(LDB,STB,DB,BS)
 * 2010.09.12 v1.0.0  : 最適化と外部変数の定数式による初期化ができる H8 統合版
 * 2010.09.09         : 初期化データでaddrofを使用できるようにvmDcNameを追加
 * 2010.08.31         : AADD を削除し、SCALE を追加
 * 2010.08.23         : RETP, RETF を統合し、
 *                      返り値レジスタに値を移動する命令 MREG を追加
 * 2010.08.16         : アドレス計算用の命令 AADD を追加
 * 2010.07.20         : Subversion による管理を開始
 * 2010.04.10         : データ生成機能を追加
 * 2010.04.01 v0.9.11 : 初期バージョン
 *
 * $Id$
 *
 */

void vmNam(int idx);               // 名前を表現するラベルを印刷する
void vmLab(int lab);               // コンパイラが生成した一時ラベルを印刷する
void vmEntry(int depth);           // 関数の入口
void vmEntryK(int depth);          // カーネル用、関数の入口
void vmRet(void);                  // 関数の出口
void vmEntryI(int depth);          // 割り込み関数の入口
void vmRetI(void);                 // 割り込み関数の出口
void vmMReg(void);                 // スタックトップを返り値レジスタに移動する
void vmArg(void);                  // スタックトップを引数領域に移動する
void vmCallP(int n, int idx);      // 値を返さない関数を呼び出す
void vmCallF(int n, int idx);      // 値を返す関数を呼び出す
void vmJmp(int lab);               // 無条件ジャンプ
void vmJT(int lab);                // 論理値を取り出し true ならジャンプ
void vmJF(int lab);                // 論理値を取り出し false ならジャンプ
void vmLdCns(int c);               // 定数をスタックにロードする
void vmLdGlb(int idx);             // 大域変数の値をスタックに積む
void vmLdLoc(int n);               // n番目のローカル変数の値をスタックに積む
void vmLdPrm(int n);               // n番目の仮引数の値をスタックに積む
void vmLdLab(int lab);             // ラベルの参照(アドレス)をスタックに積む
void vmLdNam(int idx);             // 名前の参照(アドレス)をスタックに積む
void vmLdWrd(void);                // ワード配列からスタックに積む
void vmLdByt(void);                // バイト配列からスタックに積む
void vmStGlb(int idx);             // 大域変数にストアする
void vmStLoc(int n);               // n番目のローカル変数にストアする
void vmStPrm(int n);               // n番目の仮引数にストアする
void vmStWrd(void);                // ワード配列にストアする
void vmStByt(void);                // バイト配列にストアする
void vmNeg(void);                  // ２の補数を計算し結果をスタックに積む
void vmNot(void);                  // 論理の否定を計算し結果をスタックに積む
void vmBNot(void);                 // １の補数を計算し結果をスタックに積む
void vmChr(void);                  // 文字型のビット数だけ残しスタックに積む
void vmBool(void);                 // 最下位１ビットだけ残しスタックに積む
void vmAdd(void);                  // 和を計算し結果をスタックに積む
void vmSub(void);                  // 差を計算し結果をスタックに積む
void vmShl(void);                  // 左シフトし結果をスタックに積む
void vmShr(void);                  // 右シフトし結果をスタックに積む
void vmBAnd(void);                 // ビット毎の論理積をスタックに積む
void vmBXor(void);                 // ビット毎の排他的論理和をスタックに積む
void vmBOr(void);                  // ビット毎の論理和をスタックに積む
void vmMul(void);                  // 積を計算し結果をスタックに積む
void vmDiv(void);                  // 商を計算し結果をスタックに積む
void vmMod(void);                  // 余を計算し結果をスタックに積む
void vmGt(void);                   // 比較(>)結果(論理値)をスタックに積む
void vmGe(void);                   // 比較(>=)結果(論理値)をスタックに積む
void vmLt(void);                   // 比較(<)結果(論理値)をスタックに積む
void vmLe(void);                   // 比較(<=)結果(論理値)をスタックに積む
void vmEq(void);                   // 比較(==)結果(論理値)をスタックに積む
void vmNe(void);                   // 比較(!=)結果(論理値)をスタックに積む
void vmPop(void);                  // スタックから値を一つ取り出し捨てる

/*
 * || 論理式の最後で論理値を求めるマクロ命令
 * (以下のようなニーモニックを生成する)
 *
 *       L2 != -1          |      L2 == -1
 * ------------------------+------------------------
 *          JMP   L3       |         JMP   L3
 *    L1                   |    L1
 *          LDC   1        |         LDC   1
 *          JMP   L3       |    L3
 *    L2                   |
 *          LDC   0        |
 *    L3                   |
 */
void vmBoolOR(int L1, int L2, int L3);

/*
 * && 論理式の最後で論理値を求めるマクロ命令
 * (以下のようなニーモニックを生成する)
 *
 *       L2 != -1          |      L2 == -1
 * ------------------------+------------------------
 *          JMP   L3       |         JMP   L3
 *    L1                   |    L1
 *          LDC   0        |         LDC   0
 *          JMP   L3       |    L3
 *    L2                   |
 *          LDC   1        |
 *    L3                   |
 */
void vmBoolAND(int L1, int L2, int L3);

/*
 *  データ生成用の疑似命令
 */
void vmDwName(int idx);            // 名前へのポインタを生成する
void vmDwLab(int lab);             // Ln ラベルへのポインタを生成する
void vmDwCns(int n);               // 1 ワードの整数データを生成する
void vmDbCns(int n);               // 1 バイトの整数データを生成する
void vmWs(int n);                  // n ワードの領域を生成する
void vmBs(int n);                  // n バイトの領域を生成する
void vmStr(char *s);               // 文字列を生成する
