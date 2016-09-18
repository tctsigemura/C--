/*
 * Programing Language C-- "Compiler"
 *    Tokuyama kousen Educational Computer 16bit Ver.
 *
 * Copyright (C) 2010-2012 by
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
 * vm2h8300hn.c : 仮想マシンのコードから H8/300H Tiny の機械語を生成する
 *
 * 2016.09.18         : vmLdLabをvmLdNam に変更
 *                    : vmLdStrをvmLdLab に変更
 *                    : vmTmpLabをvmLab に変更
 *                    : vmNameをvmNam に変更
 *                    : vmCharをvmChr に変更
 * 2012.09.12         : FLAG に論理値を残している場合の処理にバグがある=>未解決
 * 2012.09.09         : vmJmp, vmJT, vmJF でスタックの深さ制限をしていたのはバグ
 * 2012.08.15 v2.0.0  : *2, /2 をシフトに置換える
 * 2010.09.12 v1.0.0  : 最適化と外部変数の定数式による初期化ができる
 * 2010.09.09         : 初期化データでaddrofを使用できるようにvmDcNameを追加
 *                    : cmp.w #0,Rd を mov.w Rd,Rd に置き換える
 *                    : 関数から帰ったとき sp(r7) を可能なら adds 命令で戻す
 *                    : 定数の演算を取り止め(最適化ルーチンに任せる)
 * 2010.09.07         : add, and, or, xor, cmp 等で Src と Dst を入換え可能に
 * 2010.09.02         : マクロ命令を導入しステートマシンを除去
 *                    : ラベル欄を印刷したときは必ず改行するようにした
 * 2010.08.17         : 作成開始
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include "namtbl.h"
#include "vm.h"

/*
 * レジスタ割り付け
 */
static char *regs[]={"r0", "r1", "r2",            // Acc として使用する
		     "r3", "r4", "r5" };          // レジスタ変数として使用
		     //"e3", "e4", "e5" };        // 8ビット操作に不向き
static char *er[] = {"e0", "e1", "e2" };          // Acc の上位16ビット

#define AccSIZ  3                                 // Acc(アキュムレータ)の数
#define RVarSIZ 3                                 // RVar(レジスタ変数)の数

/*
 * Acc の管理ルーチン
 */
static boolean accBsy[AccSIZ];                    // 使用中 Acc
static int     accCnt = 0;                        // Acc の使用数

// Acc を解放する
static void accRel(int acc) {
  if (acc<0||AccSIZ<=acc||                        // Acc が範囲外か
      accBsy[acc]==false) error("バグ...accRel"); //  使用中ではない場合エラー
  accBsy[acc] = false;                            // Acc を空きに変更する
  accCnt = accCnt - 1;                            // 使用数を変更
}

// 空き Acc を探し割り付ける
static int accAlloc() {
  for (int i=0; i<AccSIZ; i=i+1) {                // 全ての Acc について   
    if (accBsy[i]) continue;                      //   使用中は飛ばす
    accBsy[i] = true;                             //   空きが見付かったら
    accCnt = accCnt + 1;                          //   状態を変更して
    return i;                                     //   それを返す
  }
  error("バグ...accAlloc");                       // ここは実行しないはず
  return -1;
}

/*
 * トランスレータ内部のスタックで仮想マシンのスタックを追跡する
 * (なるべくロード待ちを保ち、必要になるまでロードしない)
 */
// ロード待ちが FLAG あるとき true でジャンプする条件ジャンプ
//                  0      1      2     3      4      5   (n+3)%6 で逆の条件
char*   bcc[] = { "blt", "ble", "beq", "bge", "bgt", "bne"};
#define BLT   0                                   // Branch on Less Than
#define BLE   1                                   // Branch on Less or Equal
#define BEQ   2                                   // Branch on EQual
#define BGE   3                                   // Branch on Greater or Equal
#define BGT   4                                   // Branch on Greater Than
#define BNE   5                                   // Branch on Not Equal

// ロード待ちの状況
#define CNST  1                                   // 定数            (未ロード)
#define GVAR  2                                   // グローバル変数  (未ロード)
#define LVAR  3                                   // ローカル変数    (未ロード)
#define RVAR  4                                   // レジスタ変数    (未ロード)
#define ARG   5                                   // 引数            (未ロード)
#define STR   6                                   // 文字列のラベル  (未ロード)
#define ADDR  7                                   // アドレス(ラベル)(未ロード)
#define FLAG  8                                   // 比較結果がフラグにある
#define WINDR 9                                   // ワードデータの間接アドレス
#define BINDR 10                                  // バイトデータの間接アドレス
#define ACC   11                                  // Acc にある      (既ロード)
#define STK   12                                  // 実スタックに追い出された

// 仮想スタック
#define StkSIZ 20                                 // 仮想スタックのサイズ
static int stkSta[StkSIZ];                        // スタック。。。状態
static int stkAux[StkSIZ];                        // スタック。。。追加情報
static int stkPtr = -1;                           // スタックポインタ

// フラグの値に反映されている仮想スタックの位置
static int flgPtr = -1;


// 仮想スタックのスタックトップを覗き見する
#define topSta (stkSta[stkPtr])
#define topAux (stkAux[stkPtr])
#define secSta (stkSta[stkPtr-1])
#define secAux (stkAux[stkPtr-1])

// 仮想スタックに値を積む
static void stkPush(int sta, int val) {
  stkPtr = stkPtr + 1;
  //fprintf(stdout, "push=>%d\n",stkPtr);
  if (stkPtr >= StkSIZ) error("スタック不足...stkPush");
  stkSta[stkPtr] = sta;
  stkAux[stkPtr] = val;
}

// 仮想スタックの値を n 捨てる
static void stkPop(int n) {
  int newPtr = stkPtr - n;
  if (newPtr<flgPtr && flgPtr<=stkPtr) flgPtr=-1;
  stkPtr = newPtr;
  //fprintf(stdout, "pop=>%d\n",stkPtr);
  if (stkPtr < -1) error("バグ...stkPop");
}

// 仮想スタックにある Acc の中で一番奥のものを返す
static int stkSrc() {
  for (int i=0; i<=stkPtr; i=i+1) {             // 仮想スタックの奥から順に
    int s = stkSta[i];                          //   Acc を使用している
    int a = stkAux[i];                          //    ものを捜す
    if (s==ACC ||                               //   Acc にデータがある
	s==WINDR && a<AccSIZ ||                 //    間接アドレスがレジスタ
	s==BINDR && a<AccSIZ    )               //     変数ではなく Acc にある
      return i;                                 //     見付かったら場所を返す
  }
  error("バグ...stkSrc");                       // 必要時にしか呼ばれないはず
  return -1;
}

// 仮想スタックの一番奥の Acc を実スタックに移す
static void stkOut() {
  int idx = stkSrc();                           // 一番奥の Acc を探す
  int sta = stkSta[idx];                        // Acc か ?INDR のはず
  int acc = stkAux[idx];                        //   レジスタ番号
  char* reg = regs[acc];                        //   レジスタ名
  if (sta==WINDR)                               // ワードデータに変換
    printf("\tmov.w\t@%s,%s\n", reg, reg);      //     mov.w  @Rs,Rs
  else if (sta==BINDR) {                        // バイトデータに変換
    printf("\tmov.b\t@%s,%sl\n", reg, reg);     //     mov.b  @Rs,RsL
    printf("\txor.b\t%sh,%sh\n", reg, reg);     //     xor.b  RsH,RsH
  }
  printf("\tpush.w\t%s\n", reg);                //     push.w Rs
  accRel(acc);                                  // Acc を解放する
  stkSta[idx] = STK;                            // データが実スタックにある
  flgPtr = idx;                                 // フラグが変化した
}

// 全ての Acc を実スタックに移す
static void stkOutAll() {
  while (accCnt > 0) {                          // 使用中の Acc 全てについて
    stkOut();                                   //   奥から順に実スタックに移す
  }
}

/*
 * トランスレータ用のラベル管理
 */
static int tlab = 0;                             // ラベル番号
static int newLab() {                            // 新しいラベルを割り振る
  int t = tlab;
  tlab = tlab + 1;
  return t;
}

// ローカルなラベルの印刷
static void printLab(int lab) {
  printf(".L%d:\n", lab);                        // .Ln:
  flgPtr = -1;
}

// 定数のロード
static void loadCns(int rd, int n) {
  char * r = regs[rd];
  if (n==0)
    printf("\txor.w\t%s,%s\n", r, r);            //      xor.w Rd,Rd
  else 
    printf("\tmov.w\t#%d,%s\n", n, r);           //      mov.w #n,Rd
}

// ロード待ちの値を r にロードする(Acc の他、レジスタ変数にも使用する)
static void loadReg(int p, int r) {
  int sta = stkSta[p];                                 // 仮想スタックの状態
  int aux = stkAux[p];                                 // 仮想スタックの付加情報
  char *reg = regs[r];                                 // レジスタの名称
  if (sta==CNST) {                                     // 定数なら
    loadCns(r, aux);                                   //   mov.w #n,Rd
    flgPtr = p;                                        //   フラグが変化した
  } else if (sta==GVAR) {                              // グローバル変数なら
    printf("\tmov.w\t@_%s,%s\n", ntGetName(aux), reg); //   mov.w @_name,Rd
    flgPtr = p;                                        //   フラグが変化した
  } else if (sta==LVAR) {                              // ローカル変数なら
    printf("\tmov.w\t@(-%d,r6),%s\n", aux, reg);       //   mov.w @(-n,r6),Rd
    flgPtr = p;                                        //   フラグが変化した
  } else if (sta==RVAR) {                              // レジスタ変数なら
    if (aux!=r) {                                      //   必要なら
      printf("\tmov.w\t%s,%s\n", regs[aux], reg);      //   mov.w Rs,Rd
      flgPtr = p;                                      //   フラグが変化した
    }
  } else if (sta==ARG) {                               // 関数引数なら
    printf("\tmov.w\t@(%d,r6),%s\n", aux, reg);        //   mov.w @(n,r6),Rd
    flgPtr = p;                                        //   フラグが変化した
  } else if (sta==STR) {                               // 文字列のラベルなら
    printf("\tmov.w\t#.L%d,%s\n", aux, reg);           //   mov.w #.Ln,Rd
    flgPtr = p;                                        //   フラグが変化した
  } else if (sta==ADDR) {                              // グローバルラベルなら
    printf("\tmov.w\t#_%s,%s\n", ntGetName(aux), reg); //   mov.w #_name,Rd
    flgPtr = p;                                        //   フラグが変化した
  } else if (sta==WINDR) {                             // 間接ワードなら
    printf("\tmov.w\t@%s,%s\n", regs[aux], reg);       //   mov.w @Rs,Rd
    flgPtr = p;                                        //   フラグが変化した
  } else if (sta==BINDR) {                             // 間接バイトなら
    printf("\tmov.b\t@%s,%sl\n", regs[aux], reg);      //   mov.b @Rs,RdL
    printf("\txor.b\t%sh,%sh\n", reg, reg);            //   xor.b RdH,RdH
    flgPtr = -1;                                       //   フラグが無効になった
  } else if (sta==STK) {                               // スタックにある
    printf("\tpop.w\t%s\n", reg);                      //   pop.w Rd
    flgPtr = p;                                        //   フラグが変化した
  } else if (sta==ACC) {                               // スタックにある
    if (aux!=r) {                                      //   必要なら
      printf("\tmov.w\t%s,%s\n", regs[aux], reg);      //     mov.w Rs,Rd
      flgPtr = p;                                      //   フラグが変化した
    }
  } else if (sta==FLAG) {                              // フラグに論理値がある
    int lab0 = newLab();                               //   T0 を作る
    int lab1 = newLab();                               //   T1 を作る
    printf("\t%s\t.T%d\n", bcc[aux], lab0);            //     bcc .T0
    printf("\txor.w\t%s,%s\n", reg, reg);              //     xor.w Rd,Rd
    printf("\tbra\t.T%d\n", lab1);                     //     bra .T1
    printf(".T%d:\n", lab0);                           // .T0:
    printf("\tmov.w\t#1,%s\n", reg);                   //     mov.w #1,Rd
    printf(".T%d:\n", lab1);                           // .T1:
    flgPtr = p;                                        //   フラグが変化した
  } else error("バグ...loadReg");
}

// 仮想スタックの任意の位置のロード待ちを Acc にロードする
//   通常 offs は 0 か 1
//   関数呼び出しでは引数の個数までの offs を使用
static void loadStk(int offs) {                 // offs はスタックトップ
  int p = stkPtr - offs;                        //   からの距離
  if (p<0 || stkPtr<p) error("バグ...loadStk");
  int sta = stkSta[p];
  if (sta!=ACC) {                               // ロード待ちなら
    int reg = stkAux[p];
    if ((sta!=WINDR&&sta!=BINDR)||reg>=AccSIZ) {//   Acc に間接アドレス以外なら
      if (accCnt>=AccSIZ) stkOut();             //     空きがなければ作り
      reg = accAlloc();                         //     Acc を割り当てる
    }
    loadReg(p, reg);                            //   ロード待ちをAccにロード
    stkSta[p] = ACC;                            //   スタックの状態を
    stkAux[p] = reg;                            //     (ACC, reg)  に変更
  }
}  

/*
 * セグメントの切替命令の次にラベルが出力されるように現在のセグメントや
 * 出力待ちラベルを記憶しておき、命令を見てセグメントが判明した時点で出力する
 */
// セグメントの種類
#define SEG_NULL  0                             // どのセグメントか不明
#define SEG_TEXT  1                             // テキストセグメント
#define SEG_BSS   2                             // 非初期化データセグメント
#define SEG_STR   3                             // ストリング用のセグメント
#define SEG_DATA  4                             // 初期化データセグメント
#define SEG_BDATA 5                             // 初期化データバイト

static int curSeg = SEG_NULL;                   // 現在のセグメント
static char *segName[] = {"error", ".text",     // セグメントの名称
			  "error", ".rodata",
			  ".data", ".data" };
static int curLab = -1;                         // 出力待ちラベルの番号
static int curNam = -1;                         // 出力待ちの名前表添字
static boolean dupLab = false;                  // ラベルが連続した
static boolean first  = true;                   // 初めての出力

// 命令出力に先立ち必要ならセグメントの切替やラベルを出力する
static void antecedent(int newSeg) {
  if (first) {                                  // 初めての出力なら
    printf("\t.h8300hn\n");                     //   CPU の種類を出力
    first = false;
  }
  if (newSeg==SEG_BSS) return;                  // .commは特別扱い
  if (curSeg!=newSeg) {                         // セグメントが変更された
    if (dupLab) error("バグ...antecedent");     //   ラベルの連続はまずい
    if (curSeg<SEG_DATA || newSeg<SEG_DATA)     // .data 間の変化ではない
      printf("\t.section %s\n",segName[newSeg]);//   .section .data等
    if (newSeg!=SEG_STR && newSeg!=SEG_BDATA)   // アライメントが必要
      //if (newSeg!=curSeg)
      printf("\t.align 1\n");                   //   .align 1
    curSeg = newSeg;                            //   現在のセグメントを記録
  }
  if (curNam!=-1) {                             // 名前ラベルがあり
    if (ntGetPub(curNam))                       //   public なら
      printf("\t.global\t_%s\n", ntGetName(curNam));//   ` .global _name' と出力
    printf("_%s:\n", ntGetName(curNam));        //   ラベル欄に `_name: を出力
    curNam = -1;                                //   ラベルを忘れる
  }
  if (curLab!=-1) {                             // ローカルラベルがあれば
    printLab(curLab);                           //   ラベル欄に `.Ln:'
    curLab = -1;                                //   ラベルを忘れる
  }
  dupLab = false;                               // ラベルの連続をクリア
}

// グローバルな名前をラベル欄に出力(実際は出力待ちに記録)
void vmNam(int idx) {
  if (curNam!=-1) error("バグ...vmNam");        // 名前ラベルは連続しないはず
  curNam = idx;                                 // 次に出力するラベル
}

// 番号で管理されるラベルを印刷する(実際は出力待ちに記録)
void vmLab(int lab) {
  if (curLab!=-1) {                             // ローカルラベルが連続した
    printLab(curLab);                           //  .Ln
    dupLab = true;                              // 用心のために記録
  }
  curLab = lab;                                 // 次に出力するラベル
}

/*
 * 機械語命令を生成する
 */
static int rvCnt;                               // レジスタ変数の個数
static int frSize;                              // スタックの
                                                //   ローカル変数領域サイズ
// スタックフレームを作る
static void makeFrame(int depth) {
  frSize = 0;                                   // ローカル変数を
  if (depth > RVarSIZ) {                        //   レジスタ変数と
    frSize = (depth - RVarSIZ) * 2;             //     スタック上の変数に
    depth = RVarSIZ;                            //       分ける
  }
  rvCnt = depth;
  printf("\tpush.w\tr6\n");                     // フレームポインタ保存
  printf("\tmov.w\tr7,r6\n");                   // フレームポインタ設定
  if (frSize > 0) {                             // もしもレジスタ変数で不足
    if (frSize==2 || frSize==4)                 //   ならスタックに領域を作る
      printf("\tsubs\t#%d,er7\n", frSize);      //       subs #n,er7
    else                                        //     2,4 以外では sub
      printf("\tsub.w\t#%d,r7\n", frSize);      //       sub.w #n,r7
  }
  for (int i=0; i<depth; i=i+1)                 // 各レジスタ変数について
    printf("\tpush.w\t%s\n", regs[i+AccSIZ]);   //   push.w Rv
}

// スタックフレームの削除
static void cancelFrame() {
  if (stkPtr!=-1) error("バグ...cancelFrame(1)"); // スタックが元に戻ってない
  if (flgPtr!=-1) error("バグ...cancelFrame(2)");
  if (accCnt!=0)  error("バグ...cancelFrame(3)"); // 解放されていない Acc あり
  for (int i=rvCnt - 1; i>=0; i=i-1)            // 各レジスタ変数について
    printf("\tpop.w\t%s\n", regs[i+AccSIZ]);    //   pop.w Rv
  if (frSize > 0)                               // ローカル変数領域があれば
    printf("\tmov.w\tr6,r7\n");                 //   ローカル変数領域を削除し
  printf("\tpop.w\tr6\n");                      // フレームポインタを復元
}

// 関数の入口
void vmEntry(int depth, int idx) {
  vmNam(idx);                                   // 関数名ラベルを記憶させる
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  makeFrame(depth);                             // スタックフレームを作る
}

// 関数の出口
void vmRet() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  cancelFrame();                                // スタックフレームを削除し
  printf("\trts\n");                            //   rts
}

// 割り込み関数の入口
void vmEntryI(int depth, int idx) {
  vmNam(idx);                                   // 関数名ラベルを記憶させる
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  printf("\tpush.l\ter0\n");                    //   push.l er0
  printf("\tpush.l\ter1\n");                    //   push.l er1
  printf("\tpush.l\ter2\n");                    //   push.l er2
  printf("\tpush.l\ter3\n");                    //   push.l er3
  printf("\tpush.l\ter4\n");                    //   push.l er4
  printf("\tpush.l\ter5\n");                    //   push.l er5
  makeFrame(depth);                             // スタックフレームを作る
}

// 割り込み関数の出口
void vmRetI() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  cancelFrame();                                // スタックフレームを削除し
  printf("\tpop.l\ter5\n");                     //   pop.l er5
  printf("\tpop.l\ter4\n");                     //   pop.l er4
  printf("\tpop.l\ter3\n");                     //   pop.l er3
  printf("\tpop.l\ter2\n");                     //   pop.l er2
  printf("\tpop.l\ter1\n");                     //   pop.l er1
  printf("\tpop.l\ter0\n");                     //   pop.l er0
  printf("\trte\n");                            //   rte
}

// 関数の返り値をハードウェアレジスタ(r0)に移動
void vmMReg() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  loadReg(stkPtr, 0);                           //   r0 にロードする
  if ((topSta==WINDR||topSta==BINDR)&&topAux<AccSIZ ||
      topSta==ACC)                             // Acc を使用中だったなら
    accRel(topAux);                             //   Acc を解放
  stkPop(1);                                    // スタックが 1 縮小
}

// 関数の引数を引数領域(実スタック)に移動する
void vmArg() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  if (topSta!=RVAR) {                           // レジスタ変数以外なら
    loadStk(0);                                 //   仮引数を Acc にロード
    stkOutAll();                                //   実スタックに移す
  } else {                                      // レジスタ変数なら
    stkOutAll();                                //   Acc を実スタックに移す
    printf("\tpush.w\t%s\n", regs[topAux]);     //   push.w Rs
  }
  stkPop(1);                                    // 仮想スタックが 1 縮小
}

// 関数を呼ぶ
static void call(int n, int idx) {
  // 関数コール前に Acc に残っているものを実スタックに移す
    stkOutAll();                                  // Acc を全て実スタックに移動
  // 関数を呼び出す。
  printf("\tjsr\t@_%s\n", ntGetName(idx));      //  jsr @_name
  // 引数領域として使用した実スタックを解放
  if (n==1 || n==2)                             // 引数が 1 個か 2 個なら
    printf("\tadds\t#%d,er7\n", n*2);           //  adds #n,er7
  else if (n>=3)                                // 引数が 3 個以上なら
    printf("\tadd.w\t#%d,r7\n", n*2);           //  add.w #n,r7
  flgPtr = -1;                                  // フラグは無効になる
}

// 値を返さない関数を呼び出す
void vmCallP(int n, int idx) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  call(n, idx);                                 // 関数を呼び出す
}

// 値を返す関数を呼び出す
void vmCallF(int n, int idx) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  call(n, idx);                                 // 関数を呼び出す
  int a = accAlloc();                           // 戻り値用 Acc を割り当てる
  if (a!=0) error("バグ...vmCallF");            //   必ず r0 が選択されるはず
  stkPush(ACC, a);                              // 仮想スタックに r0 を記録
}

// 無条件ジャンプ
void vmJmp(int lab) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  printf("\tbra\t.L%d:16\n", lab);              //  bra .Ln:16
}

// 論理値によりジャンプするプログラムを生成する
static void boolJmp(boolean tf, int lab) {
  if (topSta==FLAG) {                           // フラグに論理値がある
    char *op;                                   // 使用する bcc 命令
    if (tf) op = bcc[topAux];                   // Trueでジャンプする bcc命令
    else    op = bcc[(topAux+3)%6];             // Falseでジャンプするbcc命令
    printf("\t%s\t.L%d:16\n", op, lab);         //     bcc .Ln:16
  } else {                                      // フラグ以外に論理値がある
    if (topSta!=ACC && topSta!=RVAR) loadStk(0);//  レジスタ以外ならロードする
    if (flgPtr!=stkPtr) {                       //   フラグが無効なら
      char *reg = regs[topAux];                 //    フラグを変化させる
      printf("\tand.w\t%s,%s\n", reg, reg);     //     and.w Rs,Rs
      flgPtr = stkPtr;                          //    スタックトップを反映
    }
    if (tf) printf("\tbne\t.L%d:16\n", lab);    //     bne .Ln
    else    printf("\tbeq\t.L%d:16\n", lab);    //     beq .Ln
    if (topSta==ACC) accRel(topAux);            //  レジスタ変数以外ならAcc解放
  }
  stkPop(1);                                    // スタックが 1 縮小
}

// スタックから論理値を取り出し true ならジャンプ
void vmJT(int lab) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  boolJmp(true, lab);                           //  true のとき lab へジャンプ
}

// スタックから論理値を取り出し false ならジャンプ
void vmJF(int lab) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  boolJmp(false, lab);                          //  false のとき lab へジャンプ
}

// 定数をスタックにロードする
void vmLdCns(int c) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  stkPush(CNST, c);                             // 仮想スタックに (CNST,c)
}

// 大域変数の値をスタックに積む
void vmLdGlb(int idx) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  stkPush(GVAR, idx);                           // 仮想スタックに (GVAR,idx)
}

// n番目のローカル変数の値をスタックに積む(レジスタ変数と分離する)
void vmLdLoc(int n) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  if (n<=RVarSIZ)                               // レジスタ変数なら
    stkPush(RVAR, n-1+AccSIZ);                  //   仮想スタックに (RVAR,reg)
  else                                          // ローカル変数
    stkPush(LVAR, (n-RVarSIZ)*2);               //   仮想スタックに (LVAR,offs)
}

// n番目の引数の値をスタックに積む
void vmLdArg(int n) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  stkPush(ARG, (n+1)*2);                        // 仮想スタックに (ARG,offs)
}

// ラベルの参照(アドレス)をスタックに積む
void vmLdLab(int lab) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  stkPush(STR, lab);                            // 仮想スタックに (STR,lab)
}

// 名前の参照(アドレス)をスタックに積む
void vmLdNam(int idx) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  stkPush(ADDR, idx);                           // 仮想スタックに (ADDR,idx)
}

// スタックトップの値を大域変数にストアする(POPはしない)
void vmStGlb(int idx) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  if (topSta!=RVAR) loadStk(0);                 // レジスタにロードする
  printf("\tmov.w\t%s,@_%s\n",                  // グローバル変数へストア
	 regs[topAux], ntGetName(idx));         //   mov.w Rs,@_name
  flgPtr = stkPtr;                              // フラグ変化あり
}

// スタックトップの値をn番目のローカル変数にストアする(POPはしない)
void vmStLoc(int n) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  if (n <= RVarSIZ) {                           // レジスタ変数なら
    int reg = n-1+AccSIZ;                       //   レジスタの番号
    loadReg(stkPtr, reg);                       //   レジスタ変数にロード
    if ((topSta==WINDR||topSta==BINDR) &&       //   Acc に間接アドレスなら
	topAux<AccSIZ) accRel(topAux);          //     Acc を解放
    if (topSta!=ACC) {                          //   Acc に値がある場合以外は
      topSta = RVAR;                            //     今後、レジスタ変数の
      topAux = reg;                             //       値を使用する
    }
  } else {                                      // ローカル変数なら
    if (topSta!=RVAR) loadStk(0);               //   レジスタにロードし
    printf("\tmov.w\t%s,@(-%d,r6)\n",           //   ローカル変数へストア
	   regs[topAux], (n-RVarSIZ)*2);        //   mov.w Rs,@(-n,r6)
    flgPtr = stkPtr;                            // フラグ変化あり
  }
}

// スタックトップの値をn番目の引数にストアする(POPはしない)
void vmStArg(int n) {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  if (topSta!=RVAR) loadStk(0);                 //   レジスタにロードし
  printf("\tmov.w\t%s,@(%d,r6)\n",              // 引数へストアする
	 regs[topAux], (n+1)*2);                //   mov.w Rs,@(n,r6)
  flgPtr = stkPtr;                              // フラグ変化あり
}

// 配列要素のアドレスを計算する(s は要素のサイズ)
static void calAddr(int s) {
  if (!((topSta==CNST && topAux==0) &&          // 添字が0でアドレスがレジスタ
	secSta==RVAR)) loadStk(1);              //   変数にあるならロードしない
  char *aReg = regs[secAux];                    // アドレスを求めるレジスタ(Ra)
  if (topSta==CNST) {                           // 添字が定数で
    int n = topAux * s;
    if (n==1 || n==2) {                         //   1,2 なら inc が使える
      printf("\tinc.w\t#%d,%s\n", n, aReg);     //  inc.w #n,Ra
      flgPtr = stkPtr - 1;                      // フラグ変化あり
    } else if (n>=3) {                          //   3 以上は add を使える
      printf("\tadd.w\t#%d,%s\n", n, aReg);     //  add.w #n,Ra
      flgPtr = stkPtr - 1;                      // フラグ変化あり
    }                                           //   0 なら何もしない
  } else {                                      // 添字が定数以外なら添字計算
    if (s==2) {                                 //  添字のスケーリング必要なら
      loadStk(0);                               //    添字を Ri にロードし
      printf("\tshal.w\t%s\n", regs[topAux]);   //      shal.w Ri
    } else {                                    //  スケーリング不要なら
      if (topSta!=RVAR) loadStk(0);             //    レジスタ変数以外はロード
    }
    char *iReg = regs[topAux];                  //  添字を格納したレジスタ(Ri)
    printf("\tadd.w\t%s,%s\n", iReg, aReg);     //    add.w Ri,Ra
    flgPtr = stkPtr - 1;                        // フラグ変化あり
    if (topSta!=RVAR) accRel(topAux);           //  Ri が Acc なら解放する
  }
  stkPop(1);                                    // アドレス計算が終わった
}

// まず、スタックから添字とワード配列の番地を取り出す
// 次にワード配列の要素の内容をスタックに積む
void vmLdWrd() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  calAddr(2);                                   // レジスタにアドレスを計算する
  topSta = WINDR;
}

// まず、スタックから添字とバイト配列の番地を取り出す
// 次にバイト配列の要素の内容をスタックに積む
void vmLdByt() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  calAddr(1);                                   // レジスタにアドレスを計算する
  topSta = BINDR;
}

// まず、スタックから添字とバイト配列の番地を取り出す
// 次に、配列の要素に新しいスタックトップの値を
// ストアする(後半でPOPはしない)
void vmStWrd() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  calAddr(2);                                   // レジスタにアドレスを計算する
  char* dstReg = regs[topAux];                  // アドレスのあるレジスタの名前
  if (secSta!=RVAR) loadStk(1);                 // データを Rs にロード
  char* srcReg = regs[secAux];                  // Acc レジスタの名前
  printf("\tmov.w\t%s,@%s\n", srcReg, dstReg);  //   mov.w Rs,@Rd
  if (topSta!=RVAR) accRel(topAux);             // Rd を解放
  stkPop(1);                                    // アドレスは捨てる
  flgPtr = stkPtr;                              // フラグ変化あり
}

// まず、スタックトップからバイトデータの番地を取り出す
// 次に、その番地に新しいスタックトップの値をストアする(後半でPOPはしない)
void vmStByt() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  calAddr(1);                                   // レジスタにアドレスを計算する
  char* dstReg = regs[topAux];                  // アドレスのあるレジスタの名前
  if (secSta!=RVAR) loadStk(1);                 // データを Rs にロード
  char* srcReg = regs[secAux];                  // Acc レジスタの名前
  printf("\tmov.b\t%sl,@%s\n", srcReg, dstReg); //   mov.b RsL,@Rd
  if (topSta!=RVAR) accRel(topAux);             // Rd を解放
  stkPop(1);                                    // アドレスは捨てる
  flgPtr = -1;                                  // フラグはバイト値に基づくので
}                                               //   使用できない

// まず、スタックから整数を取り出し２の補数を計算する
// 次に、計算結果をスタックに積む
void vmNeg() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  loadStk(0);                                   // スタックトップを Rd に移動
  printf("\tneg.w\t%s\n", regs[topAux]);        //   neg.w Rd
  flgPtr = stkPtr;                              // フラグ変化あり
}

// まず、スタックから論理値を取り出し否定を計算する
// 次に、計算結果をスタックに積む
void vmNot() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  loadStk(0);                                   // スタックトップを Rd に移動
  printf("\tbnot\t#0,%sl\n", regs[topAux]);     //   bnot rx #0,Rdl
  flgPtr = stkPtr;                              // フラグ変化あり
}

// まず、スタックから整数を取り出し１の補数を計算する
// 次に、計算結果をスタックに積む
void vmBNot() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  loadStk(0);                                   // スタックトップを Rd に移動
  printf("\tnot.w\t%s\n", regs[topAux]);        //   not.w Rd
  flgPtr = stkPtr;                              // フラグ変化あり
}

// まず、スタックから整数を取り出し文字型で有効なビット数だけ残しマスクする
// 次に、計算結果をスタックに積む
void vmChr() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  loadStk(0);                                   // スタックトップを Rd に移動
  char* reg = regs[topAux];
  printf("\txor.b\t%sh,%sh\n", reg, reg);       //   xor.b RdH,RdH
  flgPtr = -1;                                  // 計算結果は必ず０
}

// まず、スタックから整数を取り出し最下位ビットだけ残しマスクする
// 次に、計算結果をスタックに積む
void vmBool() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  loadStk(0);                                   // スタックトップを Rd に移動
  printf("\tand.w\t#1,%s\n", regs[topAux]);     //   and.w #1,Rd
  flgPtr = stkPtr;                              // フラグ変化あり
}

// 多くの演算命令の共通処理(イミディエイトが使用できる)
static void cal(char *op) {
  loadStk(1);                                   // Dst を Rd にロード
  if (topSta==CNST) {                           // 定数がロード待ちなら
    printf("\t%s.w\t#%d,%s\n",                  //     op.w #c,Rd
	   op, topAux, regs[secAux]);           //   
  } else if (topSta==RVAR) {                    // レジスタ変数がロード待ちなら
    printf("\t%s.w\t%s,%s\n",                   //     op.w Rs,Rd
	   op, regs[topAux], regs[secAux]);     //   
  } else {                                      // その他は
    loadStk(0);                                 //   Src を Rs にロード
    printf("\t%s.w\t%s,%s\n",                   //     op.w Rs,Rd
	   op, regs[topAux], regs[secAux]);     //
    accRel(topAux);                             //   Rs を解放
  }
  stkPop(1);
  flgPtr = stkPtr;                              // フラグ変化あり
}

// 左右逆順になっても良い演算で少し効率アップ
static void rcal(char *op) {
  if (secSta==CNST && topSta!=CNST) {           // スタックの奥(Dst)が定数なら
    loadStk(0);                                 //   Src を Rd にロード
    printf("\t%s.w\t#%d,%s\n",                  //    Src が右辺の演算をする
           op, secAux, regs[topAux]);           //     op.w #Dst,Rd
    secSta = topSta;                            //   Rd は新しいトップが
    secAux = topAux;                            //     使用中にする
    stkPop(1);
    flgPtr = stkPtr;                            // フラグ変化あり
  } else {                                      // それ以外は
    cal(op);                                    //   通常の演算手順
  }
}

// まず、スタックから整数を二つ取り出し和を計算する
// 次に、計算結果をスタックに積む
void vmAdd() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  topAux = topAux & 0xffff;
  secAux = secAux & 0xffff;
  if ((topSta==CNST && (topAux==1 || topAux==2)) || // 定数 1, 2 なら
      (secSta==CNST && (secAux==1 || secAux==2))) {
    rcal("inc");                                    //   inc 命令を使用する
  } else if(topSta==CNST &&
	    (topAux==0xffff || topAux==0xfffe)) {   // 定数 -1,-2
    topAux = (~topAux + 1) & 0xffff;
    cal("dec");                                     //   dec 命令を使用する
  } else if(secSta==CNST &&
	    (secAux==0xffff || secAux==0xfffe)) {   // 定数 -1,-2
    secAux = (~secAux + 1) & 0xffff;
    rcal("dec");                                    //   dec 命令を使用する
  } else                                            // 通常の足算
    rcal("add");                                    // 逆順ありのコード生成
}

// まず、スタックから整数を二つ取り出し差を計算する
// 次に、計算結果をスタックに積む
void vmSub() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  topAux = topAux & 0xffff;
  secAux = secAux & 0xffff;
  if (topSta==CNST && (topAux==1 || topAux==2)) {   // 定数 1, 2 なら
    cal("dec");                                     //   dec 命令を使用する
  } else if(topSta==CNST &&
	    (topAux==0xffff || topAux==0xfffe)) {   // 定数 -1,-2
    topAux = (~topAux + 1) & 0xffff;
    cal("inc");                                     //   inc 命令を使用する
  } else                                            // 通常の引算
    cal("sub");                                     // 逆順ありのコード生成
}

// 定数回シフトループのコード生成
static void shCnst(char *op) {
  loadStk(1);                                   // Dst を Rd にロード
  if (topAux<0||15<topAux) {                    // 変なシフト回数なら
    loadCns(secAux, 0);                         //   xor.w Rd,Rd
  } else {
    for (int i=0; i<topAux; i=i+1)              // 1〜15 なら必要数の命令生成
      printf("\t%s.w\t%s\n",op,regs[secAux]);   //   sha?.w Rd
  }
 stkPop(1);
 flgPtr = stkPtr;                               // フラグ変化あり
}

// 変数回シフトループのコード生成
static void shLoop(char *op) {
  loadStk(1);                                   // Dst(データ)を Rd にロード
  loadStk(0);                                   // Src(回数)  を Rs にロード
  int lab0 = newLab();                          // ループ用ラベル
  int lab1 = newLab();                          // ループ脱出用ラベル
  printf("\tbra\t.T%d\n", lab1);                //     bra .T1
  printf(".T%d:\n", lab0);                      // .T0:
  printf("\t%s.w\t%s\n", op, regs[secAux]);     //     sha?.w Rd
  printf(".T%d:\n", lab1);                      // .T1:
  printf("\tadd.b\t#-1,%sl\n", regs[topAux]);   //     add.b  #-1,Rs
  printf("\tbge\t.T%d\n", lab0);                //     bge    .T0
  accRel(topAux);                               // Rs を解放
  stkPop(1);
  flgPtr = -1;                                  // フラグが無効になった
}

// まず、スタックからシフトするビット数、シフトされるデータの順で取り出す。
// 次に、左シフトした結果をスタックに積む
void vmShl() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  if (topSta==CNST) shCnst("shal");             // 定数回シフトのコード生成
  else shLoop("shal");                          // 不定回シフトのコード生成
}

// まず、スタックからシフトするビット数、シフトされるデータの順で取り出す。
// 次に、右シフトした結果をスタックに積む
void vmShr() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  if (topSta==CNST) shCnst("shar");             // 定数回シフトのコード生成
  else shLoop("shar");                          // 不定回シフトのコード生成
}

// まず、スタックから整数を二つ取り出しビット毎の論理積を計算する
// 次に、計算結果をスタックに積む
void vmBAnd() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  rcal("and");                                  // 逆順ありのコード生成
}

// まず、スタックから整数を二つ取り出しビット毎の排他的論理和を計算する
// 次に、計算結果をスタックに積む
void vmBXor() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  rcal("xor");                                  // 逆順ありのコード生成
}

// まず、スタックから整数を二つ取り出しビット毎の論理和を計算する
// 次に、計算結果をスタックに積む
void vmBOr() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  rcal("or");                                   // 逆順ありのコード生成
}

static void mul() {
  loadStk(1);                                   // Dst をロード
  if (topSta==RVAR) {                           // Src がレジスタ変数なら
    printf("\tmulxs.w\t%s,e%s\n",               //    mulxs.w ry,erx
	   regs[topAux], regs[secAux]);         //   
  } else {                                      // Src がレジスタ変数以外なら
    loadStk(0);                                 // Src をロード
    printf("\tmulxs.w\t%s,e%s\n",               //   mulxs.w rx,ery
	   regs[topAux], regs[secAux]);         //
    accRel(topAux);                             // Acc を解放
  }
  stkPop(1);
  flgPtr = -1;                                  // フラグが無効になった
}

// まず、スタックから整数を二つ取り出し積を計算する
// 次に、計算結果をスタックに積む
void vmMul() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  if (topSta==CNST && topAux==2) {              //  x*2 なら
    topAux = 1;                                 //    x<<1 のコードを
    shCnst("shal");                             //      生成する
  } else if  (secSta==CNST && secAux==2) {      //
    secSta = topSta;                            //  2*x なら 
    secAux = topAux;                            // 
    topSta = CNST;                              //
    topAux = 1;                                 //    x<<1 のコードを 
    shCnst("shal");                             //      生成する
  } else  mul();                                //  普通の掛け算のコード生成
}

// 割算のコード生成
static void divMod(boolean mod) {
  loadStk(1);                                   // Dst をロード
  if (topSta==RVAR) {                           // Src がレジスタ変数なら
    printf("\texts.l\te%s\n", regs[secAux]);    //    exts.l eRd
    printf("\tdivxs.w\t%s,e%s\n",               //    divxs.w Rs,eRd
	   regs[topAux], regs[secAux]);         //   
    if (mod) printf("\tmov.w\t%s,%s\n",         //   MOD なら
		    er[secAux], regs[secAux]);  //    mov.w ERd,Rd
  } else {                                      // Src がレジスタ変数以外なら
    loadStk(0);                                 //    Src を Acc に移動
    printf("\texts.l\te%s\n", regs[secAux]);    //    exts.l eRd
    printf("\tdivxs.w\t%s,e%s\n",               //    divxs.w Rs,eRd
	   regs[topAux], regs[secAux]);         //
    if (mod) printf("\tmov.w\t%s,%s\n",         //   MOD なら
		    er[secAux], regs[secAux]);  //    mov.w ERd,Rd
    accRel(topAux);                             //    Acc を解放
  }
  stkPop(1);
  if (mod) flgPtr = stkPtr;                      // MOD ならフラグ有効
  else flgPtr = -1;                              // フラグが無効になった
}

// まず、スタックから整数を二つ取り出し商を計算する
// 次に、計算結果をスタックに積む
void vmDiv() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  if (topSta==CNST && topAux==2) {              //  /2 なら
    topAux = 1;                                 //    >>1 のコードを
    shCnst("shar");                             //      生成する
  } else divMod(false);                         // div のコード生成
}

// まず、スタックから整数を二つ取り出し剰余を計算する
// 次に、計算結果をスタックに積む
void vmMod() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  divMod(true);                                 // mod のコード生成
}

// 比較命令を出力(cns:定数,reg:レジスタ,whc:レジスタ位置)
static void printCmpCns(int cns, int reg, int whc) {
  char *r = regs[reg];
  if (cns==0) {                                 // 0 との比較は
    if (flgPtr != stkPtr - whc)                 //   省略できることがある
      printf("\tmov.w\t%s,%s\n", r, r);         //   mov.w Rd,Rd
  } else                                        // 0 以外との比較は
    printf("\tcmp.w\t#%d,%s\n", cns, r);        //   cmp.w #c,Rd
}

// 比較のコード生成
static void cmp(int bcc) {
  if (secSta==CNST && topSta!=CNST) {             // Dst が定数なら左右逆にする
    if      (bcc==BGT) bcc = BLT;                 //     GT( >  ) は LT( <  )
    else if (bcc==BGE) bcc = BLE;                 //     GE( >= ) は LE( <= )
    else if (bcc==BLT) bcc = BGT;                 //     LT( <  ) は GT( >  )
    else if (bcc==BLE) bcc = BGE;                 //     LE( <= ) は GE( >= )
    if (topSta!=RVAR) loadStk(0);                 //   レジスタ変数以外はロード
    printCmpCns(secAux, topAux, 0);               //     cmp.w #n,Rs
    if (topSta==ACC) accRel(topAux);              //     Rs を解放
    stkPop(2);
  } else if (secSta==RVAR) {                      // Dst がレジスタ変数で
    char * r = regs[secAux];                      //
    if (topSta==CNST) {                           //   Src が定数なら
      printCmpCns(topAux, secAux, 1);             //     cmp.w #n,Rd
    } else if (topSta==RVAR) {                    //   Src がレジスタ変数なら
      printf("\tcmp.w\t%s,%s\n",regs[topAux],r);  //     cmp.w Rs,Rd
    } else if (topSta==STR) {                     //   Src が文字列(ラベル)なら
      printf("\tcmp.w\t#.L%d,%s\n",topAux,r);     //     cmp.w #.Ln,Rd
    } else if (topSta==ADDR) {                    //   Src が名前(ラベル)なら
      printf("\tcmp.w\t#_%s,%s\n",                //     cmp.w #_name,Rd
	     ntGetName(topAux), r);               //
    } else {                                      //   Src がこれら以外なら
      loadStk(0);                                 //     Acc にロードし
      printf("\tcmp.w\t%s,%s\n",regs[topAux],r);  //     cmp.w Rs,Rd
      accRel(topAux);                             //     Rs を解放
    }
    stkPop(2);                                    //   Src, Dst を解放
  } else if (topSta==CNST) {                      // Src が定数(DstはRVAR以外)
    int cns = topAux;                             //   定数をスタックから取出す 
    stkPop(1);                                    //   Src を解放
    loadStk(0);                                   //   Dst を Acc にロード
    printCmpCns(cns, topAux, 0);                  //     cmp.w #n,Rd
    accRel(topAux);                               //     Rd を解放
    stkPop(1);                                    //   Dst を解放
  } else {                                        // レジスタ変数以外なら
    cal("cmp");                                   //   通常の計算でコード生成
    accRel(topAux);                               //   結果はフラグなので解放
    stkPop(1);                                    //   Dst を解放
  }
  stkPush(FLAG, bcc);                             // 結果は (FLAG, bcc)
  flgPtr = -1;                                    // フラグは異なる使い方
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmGt() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  cmp(BGT);                                     // 比較命令を生成
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmGe() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  cmp(BGE);                                     // 比較命令を生成
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmLt() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  cmp(BLT);                                     // 比較命令を生成
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmLe() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  cmp(BLE);                                     // 比較命令を生成
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmEq() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  cmp(BEQ);                                     // 比較命令を生成
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmNe() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  cmp(BNE);                                     // 比較命令を生成
}

// スタックから値を一つ取り出し捨てる
void vmPop() {
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  if (topSta==ACC) {                            // スタックトップがレジスタなら
    accRel(topAux);                             //   レジスタを解放
  } else if ((topSta==WINDR || topSta==BINDR)   // スタックトップが間接アドレス
	     && topAux<AccSIZ) {                //   で Acc にあるなら
    accRel(topAux);                             //   レジスタを解放
  } else if (topSta==STK) {                     // スタックトップが実スタック
    //printf("\tadds\t#2,er7\n");               //    adds #2,er7
    error("バグ...vmPop");                      //   ここは実行されないはず
  }
  stkPop(1);
}

/*
 *  マクロ命令
 */
// フラグ以外にある場合
static void boolOrAnd(int lab1, int lab2, int lab3, int x, int nx) {
  loadStk(0);                                   //   Acc にロードする
  printf("\tbra\t.L%d\n", lab3);                //       bra .L3
  printLab(lab1);                               //  .L1:
  loadCns(topAux, x);                           //       mov.w #x,Rd
  if (lab2!=-1) {
    printf("\tbra\t.L%d\n", lab3);              //       bra .L3
    printLab(lab2);                             //  .L2:
    loadCns(topAux, nx);                        //       mov.w #nx,Rd
  }
  printLab(lab3);                               //  .L3:
}

// フラグにある場合
static void flagOrAnd(int lab1, int lab2, int lab3, char *bcc, int x, int nx) {
  char lab2P = 'L';
  if (lab2==-1) {                               //   lab2 が未割り当て
    lab2 = newLab();                            //     割り当てる
    lab2P = 'T';                                //     トランスレータのラベル
  }
  if (accCnt>=AccSIZ) stkOut();                 //   Acc に空きを作って
  int reg = accAlloc();                         //   論理値を求める Acc を用意
  printf("\t%s\t.%c%d\n", bcc, lab2P, lab2);    //       bcc   .L2
  printLab(lab1);                               //  .L1:
  loadCns(reg, x);                              //       mov.w #x,Rd
  printf("\tbra\t.L%d\n", lab3);                //       bra   .L3
  printf(".%c%d:\n", lab2P, lab2);              //  .L2:
  loadCns(reg, nx);                             //       mov.w #nx,Rd
  printLab(lab3);                               //  .L3:
  topSta = ACC;                                 //  状態は (ACC, Rd) になる
  topAux = reg;
  flgPtr = stkPtr;                              // フラグが変化する
}

// BOOLOR .L1,.L2,.L3 マクロ命令
void vmBoolOR(int lab1, int lab2, int lab3) {
  if (stkPtr<0) error("バグ...vmBoolOR");
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  if (topSta==FLAG)                             // フラグに結果があるなら
    flagOrAnd(lab1, lab2, lab3, bcc[(topAux+3)%6], 1, 0);
  else                                          // フラグ以外にあるなら
    boolOrAnd(lab1, lab2, lab3, 1, 0);
}

// BOOLAND .L1,.L2,.L3 マクロ命令
void vmBoolAND(int lab1, int lab2, int lab3) {
  if (stkPtr<0) error("バグ...vmBoolOR");
  antecedent(SEG_TEXT);                         // .section 等を必要なら出力
  if (topSta==FLAG)                             // フラグに結果があるなら
    flagOrAnd(lab1, lab2, lab3, bcc[topAux], 0, 1);
  else                                          // フラグ以外にあるなら
    boolOrAnd(lab1, lab2, lab3, 0, 1);
}

/*
 *  データ生成用の疑似命令
 */
// DW name      (ポインタデータの生成)
void vmDwName(int idx) {
  antecedent(SEG_DATA);                         // .section 等を必要なら出力
  printf("\t.word\t_%s\n", ntGetName(topAux));  //    .word _name
}

// DW .Ln       (ポインタデータの生成)
void vmDwLab(int lab) {
  antecedent(SEG_DATA);                         // .section 等を必要なら出力
  printf("\t.word\t.L%d\n", lab);               //    .word .Ln
}

// DW N         (整数データの生成)
void vmDwCns(int n) {
  antecedent(SEG_DATA);                         // .section 等を必要なら出力
  printf("\t.word\t%d\n", n);                   //    .word N
}

// DB N         (整数データの生成)
void vmDbCns(int n) {
  antecedent(SEG_BDATA);                        // .section 等を必要なら出力
  printf("\t.byte\t%d\n", n);                   //    .byte N
}

// WS N         (ワード領域の生成)
void vmWs(int n) {
  antecedent(SEG_BSS);                          // .section 等を必要なら出力
  if (curLab!=-1) {                             // ローカルラベルがあれば
    printf("\t.comm\t.L%d,%d,2\n",              //   .comm .Ln,n*2,2
	   curLab, n*2);
    curLab = -1;                                //   ラベルを忘れる
  } else if (curNam!=-1) {                      // 名前ラベルがあれば
    printf("\t.comm\t_%s,%d,2\n",               //   .comm _name,n*2,2
	   ntGetName(curNam), n*2);
    curNam = -1;                                //   ラベルを忘れる
  } else error("バグ...vmWs(1)");               // ラベルがない場合はエラー

  if (curNam!=-1) error("バグ...vmWs(2)");      // ラベルが複数あったらエラー
  dupLab = false;                               // 重複チェックをリセット
}

// BS N         (バイト領域の生成)
void vmBs(int n) {
  antecedent(SEG_BSS);                          // .section 等を必要なら出力
  if (curLab!=-1) {                             // ローカルラベルがあれば
    printf("\t.comm\t.L%d,%d\n", curLab, n);    //   .comm .Ln,n
    curLab = -1;                                //   ラベルを忘れる
  } else if (curNam!=-1) {                      // 名前ラベルがあれば
    printf("\t.comm\t_%s,%d\n",                 //   .comm _name,n
	   ntGetName(curNam), n);
    curNam = -1;                                //   ラベルを忘れる
  } else error("バグ...vmBs(1)");               // ラベルがない場合はエラー

  if (curNam!=-1) error("バグ...vmBs(2)");      // ラベルが複数あったらエラー
  dupLab = false;                               // 重複チェックをリセット
}

// STRING "..." (文字列データの生成)
void vmStr(char *s) {
  antecedent(SEG_STR);                          // .section 等を必要なら出力
  printf("\t.string\t\"%s\"\n", s);             // .string "str"
}
