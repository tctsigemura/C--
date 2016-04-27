/*
 * Programing Language C-- "Compiler"
 *    Tokuyama kousen Educational Computer 16bit Ver.
 *
 * Copyright (C) 2010-2016 by
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
 * vm2tac.c : 仮想スタックマシンのコードから TaC-CPU V2 の機械語を生成する
 *            (仮想スタックマシンをシミュレーションする機械語を生成する)
 *
 * 2016.01.18 v2.1.2  : vmPop() で BUG の警告を止める
 *                      ("a[3];"のような意味の無い式で警告が出てしまう。)
 * 2015.08.31 v2.1.0  : vmEntryK 追加
 *                      Clang の警告対策
 * 2012.09.28         : 出力の "op Rd,%Rx" を "op Rd,0,Rx" へ置換える
 *     (as-- が自動的にオフセット 0 のインデクスドをインダイレクトに置換える)
 * 2012.09.12         : 初期バージョン完成
 * 2012.08.20         : vm2h8300hn.c から枝分かれし vm2tac.c になる
 * 2010.08.17         : vm2h8300hn.c 作成開始
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

#define StrMAX  128
#define BUG(c,msg) {if(c) {fprintf(stderr,"BUG..."); error(msg);}}
//#define BUG(c,msg) {}

// 大域データ
static char str[StrMAX + 1];
static FILE *fp;                                  // 入力ファイル
int lxGetLn(){ return 0; }                        // vm2tacでは使われないはず
char *lxGetFname() { return "ERROR lxGetFname"; } // vm2tacでは使われないはず

/*
 * レジスタの一覧と使用目的
 */
static char *regs[]={"G0",  "G1",  "G2",        // Acc として使用する
		     "G3",  "G4",  "G5",        // レジスタ変数として使用
		     "G6",  "G7",  "G8",        // レジスタ変数として使用
		     "G9",  "G10", "G11"};      // レジスタ変数として使用

#define AccSIZ  3                               // Acc(アキュムレータ)の数(>=2)
#define RVarSIZ 9                               // RVar(レジスタ変数)の数

/*
 * Acc の管理
 */
static boolean accBsy[AccSIZ];                  // 使用中 Acc
static int     accCnt = 0;                      // Acc の使用数

// Acc を解放する
static void relAcc(int acc) {
  BUG(acc<0 || AccSIZ<=acc || accBsy[acc]==false, "relAcc")
  accBsy[acc] = false;                          // Acc を空きに変更する
  accCnt = accCnt - 1;                          // 使用数を変更
}

// 空き Acc を探し割り付ける
static int allocAcc() {
  for (int i=0; i<AccSIZ; i=i+1) {              // 全ての Acc について   
    if (accBsy[i]) continue;                    //   使用中は飛ばす
    accBsy[i] = true;                           //   空きが見付かったら
    accCnt = accCnt + 1;                        //   状態を変更して
    return i;                                   //   それを返す
  }
  BUG(true, "allocAcc")                         // ここは実行しないはず
  return -1;
}

/*
 * 大域名が外部公開なら '_'、そうでなければ '.' を返す。
 * (TaCアセンブラの仕様 : '.' で始まる名前は外部非公開)
 */
static char getPref(int n) {
  if (ntGetPub(n)) return '_';                  // public なら '_' を付加
  return '.';                                   // private なら '.' を付加
}

/*
 * トランスレータ内部のスタックで仮想マシンのスタックを追跡する
 * (なるべくロード待ちを保ち、必要になるまでロードしない)
 */
// 論理値のロード待ちがフラグにあるとき true でジャンプする条件ジャンプ
//                  0      1      2     3      4      5   (n+3)%6 で逆の条件
char*   jcc[] = { "JLT", "JLE", "JZ", "JGE", "JGT", "JNZ"};
#define JLT   0                                 // Jump on Less Than
#define JLE   1                                 // Jump on Less or Equal
#define JEQ   2                                 // Jump on EQual
#define JGE   3                                 // Jump on Greater or Equal
#define JGT   4                                 // Jump on Greater Than
#define JNZ   5                                 // Jump on Not Equal

// ロード待ちの状況
#define CNST  1                                 // 定数            (未ロード)
#define GVAR  2                                 // グローバル変数  (未ロード)
#define LVAR  3                                 // ローカル変数    (未ロード)
#define RVAR  4                                 // レジスタ変数    (未ロード)
#define ARG   5                                 // 引数            (未ロード)
#define STR   6                                 // 文字列のラベル  (未ロード)
#define ADDR  7                                 // アドレス(ラベル)(未ロード)
#define WINDR 8                                 // ワードデータの間接アドレス
#define BINDR 9                                 // バイトデータの間接アドレス
#define ACC   10                                // Acc にある      (既ロード)
#define FLAG  11                                // 比較結果がフラグにある
#define STK   12                                // 実スタックに追い出された

// 仮想スタック
#define StkSIZ 20                               // 仮想スタックのサイズ
static int stkSta[StkSIZ];                      // スタック。。。状態
static int stkAux[StkSIZ];                      // スタック。。。追加情報
static int stkPtr = -1;                         // スタックポインタ

// フラグの値に反映されている仮想スタックの位置
static int flgPtr = -1;

// 論理値のロード待ちがフラグにある場合、論理値に変換するための条件ジャンプ
static int flgJcc;

// 仮想スタックのスタックトップを覗き見する
#define topSta (stkSta[stkPtr])
#define topAux (stkAux[stkPtr])
#define secSta (stkSta[stkPtr-1])
#define secAux (stkAux[stkPtr-1])

// フラグにロード待ち(p)があるとき論理値をレジスタ(r)に移す
static int tlab = 0;                            // 生成するラベル(.Tn)の番号
static void loadFlg(int r, int p) {
  BUG(flgPtr==-1 || flgPtr!=p || stkSta[p]!=FLAG, "loadFlg");
  char *reg = regs[r];                          // レジスタの名称
  printf("\tLD\t%s,#1\n", reg);                 //     LD Reg,#1 (true)
  printf("\t%s\t.T%d\n", jcc[flgJcc], tlab);    //     Jcc .Tn
  printf("\tLD\t%s,#0\n", reg);                 //     LD Reg,#0 (flase)
  printf(".T%d\n", tlab);                       // .Tn
  tlab = tlab + 1;                              // ラベル番号を進める
}
    
// フラグ変化にそなえ、フラグにある論理値を Acc に移動する
static void clearFlg() {
  if (flgPtr!=-1 && stkSta[flgPtr]==FLAG) {     // ロード待ちがフラグにあれば
    loadFlg(stkAux[flgPtr], flgPtr);            //   Acc にロードする
    stkSta[flgPtr] = ACC;                       //   論理値は Acc に格納された
  }
  flgPtr = -1;                                  // この後フラグは破壊される
}

// 仮想スタック(p)とレジスタ(r)で演算(op)する
static void calReg(char *op, int r, int p) {
  int sta = stkSta[p];                          // 仮想スタックの状態
  int aux = stkAux[p];                          // 仮想スタックの付加情報
  char *reg = regs[r];                          // レジスタの名称
  if (sta==CNST) {                              // 定数なら
    printf("\t%s\t%s,#%d\n", op, reg, aux);     //   op Reg,#n
  } else if (sta==GVAR) {                       // グローバル変数なら
    printf("\t%s\t%s,%c%s\n", op, reg,          //   op Reg,_name
	   getPref(aux), ntGetName(aux));       //
  } else if (sta==LVAR) {                       // ローカル変数なら
    printf("\t%s\t%s,-%d,FP\n", op, reg, aux);  //   op Reg,-n,FP
  } else if (sta==RVAR) {                       // レジスタ変数なら
    printf("\t%s\t%s,%s\n", op,reg,regs[aux]);  //   op Reg,RVar
  } else if (sta==ARG) {                        // 関数引数なら
    printf("\t%s\t%s,%d,FP\n", op, reg, aux);   //   op Reg,n,FP
  } else if (sta==STR) {                        // 文字列のラベルなら
    printf("\t%s\t%s,#.S%d\n", op, reg, aux);   //   op Reg,#.Sn
  } else if (sta==ADDR) {                       // グローバルラベルなら
    printf("\t%s\t%s,#%c%s\n", op, reg,         //   op Reg,#_name
	   getPref(aux), ntGetName(aux));       //
  } else if (sta==WINDR) {                      // 間接ワードなら
 // printf("\t%s\t%s,%%%s\n",op,reg,regs[aux]); //   op Reg,%Acc
    printf("\t%s\t%s,0,%s\n",op,reg,regs[aux]); //   op Reg,0,Acc
  } else if (sta==BINDR) {                      // 間接バイトなら
    printf("\t%s\t%s,@%s\n",op,reg,regs[aux]);  //   op Reg,@Acc
  } else if (sta==ACC) {                        // Acc に値があるなら
    printf("\t%s\t%s,%s\n",op,reg,regs[aux]);   //   op reg,acc
  } else BUG(true, "calReg");                   // それ以外は直接演算できない
}

// 仮想スタック(p)からレジスタ(r)にロードする
static void loadReg(int r, int p) {
  int sta = stkSta[p];
  if (sta==STK) {                               // 値は実スタックにある
    printf("\tPOP\t%s\n",regs[r]);              //   POP Reg
  } else if (sta==FLAG) {                       // 値はフラグにある論理値
    loadFlg(r, p);                              //   論理値を r に移す
  } else {                                      // その他なら
    BUG((sta==ACC||sta==RVAR) && stkAux[p]==r,  //   必要ないのに呼ばれていれる
	"loadReg");
    calReg("LD", r, p);                         //   演算ルーチンを流用して
  }                                             //     ソースをロード
}

// 仮想スタックに値を積む
static void pushStk(int sta, int val) {
  stkPtr = stkPtr + 1;
  if (stkPtr >= StkSIZ) error("stack overflow...pushStk");
  stkSta[stkPtr] = sta;
  stkAux[stkPtr] = val;
}

// 仮想スタックの値を n 個捨てる
static void popStk(int n) {
  int newPtr = stkPtr - n;                      // スタックポインタの新しい値
  BUG(newPtr<-1, "popStk");                     // スタックの底を通り過ぎる
  if (newPtr<flgPtr) flgPtr = -1;               // フラグ値が無効になる場合
  stkPtr = newPtr;                              // スタックポインタを更新
}

// 仮想スタックの ptr 番目は 定数 0 か
static boolean isCnst0(int ptr) {
  BUG(ptr<0 || stkPtr<ptr, "isCnst");           // ptr の値が不正
  return stkSta[ptr]==CNST && stkAux[ptr]==0;   // 定数 0 なら true
}

// 仮想スタックの ptr 番目は Acc を使用しているか
static boolean isAcc(int ptr) {
  BUG(ptr<0 || stkPtr<ptr, "isAcc");            // ptr の値が不正
  int sta = stkSta[ptr];
  int aux = stkAux[ptr];
  return sta==ACC || sta==FLAG ||               // Acc か Flag か
    ((sta==WINDR || sta==BINDR) && aux<AccSIZ); //  Acc 上の間接アドレス
}

// 仮想スタックにある Acc の中で一番奥のものを返す
static int srchAcc() {
  for (int i=0; i<=stkPtr; i=i+1)               // 仮想スタックの奥から順に
    if (isAcc(i)) return i;                     //   Acc を探し,場所を返す
  BUG(true, "srchAcc");                         // 必要時にしか呼ばれないはず
  return -1;
}

// 仮想スタックの一番奥の Acc を実スタックに移す
static void outStk() {
  int idx = srchAcc();                          // 一番奥の Acc を探す
  int sta = stkSta[idx];                        // Acc, FLAG か間接アドレス
  int acc = stkAux[idx];                        // レジスタ番号
  if (sta!=ACC)                                 // FLAG か間接アドレスなら
    loadReg(acc, idx);                          //   値を Acc にロードし
  printf("\tPUSH\t%s\n", regs[acc]);            //     PUSH Acc
  relAcc(acc);                                  // Acc を解放する
  stkSta[idx] = STK;                            // データが実スタックにある
}

// 全ての Acc を実スタックに移す
static void outStkAll() {
  while (accCnt > 0) {                          // 使用中の Acc 全てについて
    outStk();                                   //   奥から順に実スタックに移す
  }
}

// 仮想スタックの任意の位置のロード待ちを Acc にロードする
//   通常 offs は 0 か 1
static void loadStk(int offs) {                 // offs はスタックトップ
  int p = stkPtr - offs;                        //   からの距離
  BUG(p<0 || stkPtr<p, "loadStk");
  if (stkSta[p]!=ACC) {                         // スタックがロード待ちなら
    int acc = stkAux[p];                        //  ロードする Acc を決める
    if (!isAcc(p)) {                            //    もし Acc が未割り当てなら
      if (accCnt>=AccSIZ) outStk();             //      空きがなければつくり
      acc = allocAcc();                         //       新しく割り当てる
    }
    loadReg(acc, p);                            // Acc にロード待ちをロードする
    stkSta[p] = ACC;                            // スタックの状態を
    stkAux[p] = acc;                            //   (ACC, acc)  に変更
  }
}

/*
 * 仮想マシンの命令に対応する関数(vm.h に宣言、vmCode.c から呼ばれる)
 */
static boolean inFunc = false;                  // 関数内部のコード生成中
static int rvCnt;                               // レジスタ変数の個数
static int frSize;                              // スタックフレームの
                                                //   ローカル変数領域サイズ
// グローバルな名前をラベル欄に出力
void vmName(int idx) {
  printf("%c%s",getPref(idx),ntGetName(idx));   // '_' or '.' と 名前の出力
}

// 番号で管理されるラベルを印刷する
void vmTmpLab(int lab) {
  printf(".L%d", lab);                          // .Ln
  if (inFunc) printf("\n");                     // 関数内部ではラベルが
}                                               //   連続することがある

// 番号で管理されるSTRING用ラベルを印刷する
void vmTmpLabStr(int lab) {
  printf(".S%d", lab);                          // .Sn
  BUG(inFunc, "関数内でSTRING出力");            // 関数内部では呼ばれない
}

// スタックフレームを作る
static void makeFrame(int depth) {
  inFunc = true;                                // 関数内部のコード生成開始
  frSize = 0;                                   // ローカル変数を
  if (depth > RVarSIZ) {                        //   レジスタ変数と
    frSize = (depth - RVarSIZ) * 2;             //     スタック上の変数に
    depth = RVarSIZ;                            //       分ける
  }
  rvCnt = depth;                                // レジスタ変数の個数決定
  printf("\tPUSH\tFP\n");                       //   PUSH FP
  printf("\tLD\tFP,SP\n");                      //   LD   FP,SP
  if (frSize > 0)                               // レジスタ変数だけでは不足なら
    printf("\tSUB\tSP,#%d\n", frSize);          //   SUB  SP,#n
  for (int i=0; i<depth; i=i+1)                 // 各レジスタ変数について
    printf("\tPUSH\t%s\n", regs[i+AccSIZ]);     //   PUSH Rv
}

// スタックフレームの削除
static void cancelFrame() {
  BUG(stkPtr!=-1, "cancelFrame(1)");            // 仮想スタックが元に戻ってない
  BUG(accCnt!=0,  "cancelFrame(2)");            // 解放されていない Acc あり
  for (int i=rvCnt-1; i>=0; i=i-1)              // 各レジスタ変数について
    printf("\tPOP\t%s\n", regs[i+AccSIZ]);      //   POP Rv
  if (frSize > 0)                               // ローカル変数領域があれば
    printf("\tLD\tSP,FP\n");                    //   LD  SP,FP
  printf("\tPOP\tFP\n");                        //   POP FP
  inFunc = false;                               // 関数内部のコード生成終了
}

// 関数の入口
void vmEntry(int depth, int idx) {
  vmName(idx);                                  // 関数名ラベルを印刷
  makeFrame(depth);                             // スタックフレームを作る
  printf("\tCALL\t__stkChk\n");                 // スタックオーバーフローを
}                                               //   チェックする

// カーネル関数の入口
void vmEntryK(int depth, int idx) {
  vmName(idx);                                  // 関数名ラベルを印刷
  makeFrame(depth);                             // スタックフレームを作る
}

// 関数の出口
void vmRet() {
  cancelFrame();                                // スタックフレームを削除し
  printf("\tRET\n");                            //   RET
}

// 割り込み関数の入口
void vmEntryI(int depth, int idx) {
  vmName(idx);                                  // 関数名ラベルを印刷
  for (int gr = 0; gr < AccSIZ; gr = gr + 1)    // アキュムレータを全て
    printf("\tPUSH\t%s\n", regs[gr]);           //   保存する
  makeFrame(depth);                             // スタックフレームを作る
}

// 割り込み関数の出口
void vmRetI() {
  cancelFrame();                                // スタックフレームを削除し
  for (int gr=AccSIZ-1; gr>=0; gr=gr-1)         // アキュムレータを全て
    printf("\tPOP\t%s\n", regs[gr]);            //   復元する
  printf("\tRETI\n");                           //   RETI
}

// 関数の返り値をハードウェアレジスタ(G0)に移動
void vmMReg() {
  if (topSta!=ACC || topAux!=0)                 // 返す値が G0 以外にあるなら
    loadReg(0, stkPtr);                         //   G0 にロードする
  if (isAcc(stkPtr)) relAcc(topAux);            // Acc を使用中なら解放する
  popStk(1);                                    // 仮想スタックが 1 縮小
}

// 関数の引数を引数領域(実スタック)に移動する
void vmArg() {
  if (topSta!=RVAR) {                           // レジスタ変数以外なら
    loadStk(0);                                 //   引数を Acc にロード
    outStkAll();                                //   実スタックに移す
  } else {                                      // レジスタ変数なら
    outStkAll();                                //   Acc を実スタックに移す
    printf("\tPUSH\t%s\n", regs[topAux]);       //   PUSH RVar
  }
  popStk(1);                                    // 仮想スタックが 1 縮小
}

// 関数を呼ぶ
static void call(int n, int idx) {
  outStkAll();                                  // Acc を全て実スタックに移動
  printf("\tCALL\t%c%s\n",                      // 関数を呼び出す
	 getPref(idx), ntGetName(idx));         //     CALL _name
  if (n!=0)                                     // 引数がスタックにあれば捨てる
    printf("\tADD\tSP,#%d\n", n*2);             //     ADD SP,#(n*2)
}

// 値を返さない関数を呼び出す
void vmCallP(int n, int idx) {
  call(n, idx);                                 // 関数を呼ぶ
}

// 値を返す関数を呼び出す
void vmCallF(int n, int idx) {
  call(n, idx);                                 // 関数を呼ぶ
  int acc = allocAcc();                         // 戻り値用 Acc を割り当てる
  BUG(acc!=0, "vmCallF");                       //   必ず G0 が選択されるはず
  pushStk(ACC, acc);                            // 仮想スタックに G0 を記録
}

// 無条件ジャンプ
void vmJmp(int lab) {                           // lab はラベル番号
  printf("\tJMP\t.L%d\n", lab);                 //  JMP .Ln
}

// 論理値によりジャンプするプログラムを生成する
static void boolJmp(boolean tf, int lab) {
  if (topSta==FLAG) {                           // 論理値がフラグにある
    BUG(flgPtr!=stkPtr, "boolJmp");             //   flgPtrは正しいか?
    char *op;                                   //   使用する bcc 命令
    if (tf) op = jcc[flgJcc];                   //   Trueでジャンプする Jcc命令
    else    op = jcc[(flgJcc+3)%6];             //   FalseでジャンプするJcc命令
    printf("\t%s\t.L%d\n", op, lab);            //     Jcc .Ln
  } else {                                      // 論理値がフラグ以外にある
    if (flgPtr!=stkPtr) {                       //  フラグに反映されていない
      clearFlg();                               //  フラグ値の変化にそなえる
      if (topSta!=RVAR) loadStk(0);             //  レジスタ変数以外ならロード
      printf("\tCMP\t%s,#0\n", regs[topAux]);   //  フラグ値に反映させる
    }                                           //     CMP Acc,#0
    if (tf) printf("\tJNZ\t.L%d\n", lab);       //     JNZ .Ln(Trueでジャンプ)
    else    printf("\tJZ\t.L%d\n", lab);        //     JZ  .Ln(Falseでジャンプ)
  }
  if (isAcc(stkPtr)) relAcc(topAux);            // Acc を使用していたら解放
  popStk(1);                                    // スタックが 1 縮小
}

// スタックから論理値を取り出し true なら lab へジャンプ
void vmJT(int lab) {                            // lab はラベル番号
  boolJmp(true, lab);                           //  true のとき lab へジャンプ
}

// スタックから論理値を取り出し false なら lab へジャンプ
void vmJF(int lab) {                            // lab はラベル番号
  boolJmp(false, lab);                          // false のとき lab へジャンプ
}

// 定数をスタックにロードする
void vmLdCns(int c) {                           // c は定数値
  pushStk(CNST, c);                             // 仮想スタックに (CNST,c)
}

// 大域変数の値をスタックに積む
void vmLdGlb(int idx) {                         // idx は名前表のインデクス
  pushStk(GVAR, idx);                           // 仮想スタックに (GVAR,idx)
}

// n番目のローカル変数の値をスタックに積む(レジスタ変数と分離する)
void vmLdLoc(int n) {                           // n はローカル変数番号(n>=1)
  if (n<=RVarSIZ)                               // レジスタ変数なら
    pushStk(RVAR, n-1+AccSIZ);                  //   仮想スタックに (RVAR,reg)
  else                                          // ローカル変数
    pushStk(LVAR, (n-RVarSIZ)*2);               //   仮想スタックに (LVAR,offs)
}                                               //   (offs は FP からの距離)

// n番目の引数の値をスタックに積む
void vmLdArg(int n) {                           // n は引数番号(n>=1)
  pushStk(ARG, (n+1)*2);                        // 仮想スタックに (ARG,offs)
}                                               //   (offs は FP からの距離)

// 文字列のアドレスをスタックに積む
void vmLdStr(int lab) {                         // lab はラベル番号
  pushStk(STR, lab);                            // 仮想スタックに (STR,lab)
}

// ラベルの値(アドレス)をスタックに積む
void vmLdLab(int idx) {                         // idx は名前表のインデクス
  pushStk(ADDR, idx);                           // 仮想スタックに (ADDR,idx)
}

// スタックトップの値を大域変数にストアする(POPはしない)
void vmStGlb(int idx) {                         // idx は名前表のインデクス
  if (topSta!=RVAR) loadStk(0);                 // レジスタにロードする
  printf("\tST\t%s,%c%s\n", regs[topAux],       // グローバル変数へストア
	 getPref(idx), ntGetName(idx));         //   ST Reg,_name
}

// スタックトップの値をn番目のローカル変数にストアする(POPはしない)
void vmStLoc(int n) {                           // n はローカル変数番号(n>=1)
  if (n<=RVarSIZ) {                             // レジスタ変数なら
    int reg = n-1+AccSIZ;                       //   レジスタの番号
    if (topSta!=RVAR || topAux!=reg)            //   必要があれば
      loadReg(reg, stkPtr);                     //     値をレジスタ変数に格納
    if (topSta!=ACC) {                          //   もともとAcc以外に値があった
      if (isAcc(stkPtr)) relAcc(topAux);        //     Acc を開放(間接アドレス)
      topSta = RVAR;                            //     今後は、
      topAux = reg;                             //      今回レジスタ変数に
    }                                           //       ロードした値を使用する
  } else {                                      // メモリ上のローカル変数なら
    if (topSta!=RVAR) loadStk(0);               //   レジスタにロードし
    printf("\tST\t%s,-%d,FP\n",                 //   ローカル変数へストア
	   regs[topAux], (n-RVarSIZ)*2);        //   ST Acc,-n,FP
  }
}

// スタックトップの値をn番目の引数にストアする(POPはしない)
void vmStArg(int n) {                           // n は引数番号(n>=1)
  if (topSta!=RVAR) loadStk(0);                 // レジスタにロードし
  printf("\tST\t%s,%d,FP\n",                    // 引数へストアする
	 regs[topAux], (n+1)*2);                //   ST Acc,n,FP
}

// まず、スタックから添字とワード配列の番地を取り出す
// 次にワード配列の要素の内容をスタックに積む
void vmLdWrd() {
  if (!isCnst0(stkPtr) || secSta!=RVAR) {       // Acc を利用する必要がある
    loadStk(1);                                 //  アドレスを Acc にロードする
    if (!isCnst0(stkPtr)) {                     //  添字が定数 0 以外なら
      clearFlg();                               //   フラグ値の変化にそなえる
      calReg("ADDS", secAux, stkPtr);           //   添字をスケーリングして足す
    }
  }
  if (isAcc(stkPtr)) relAcc(topAux);            // 添字が Acc なら解放する
  popStk(1);                                    // 添字をスタックから捨てる
  topSta = WINDR;                               // スタックには要素のアドレス
}

// まず、スタックから添字とバイト配列の番地を取り出す
// 次にバイト配列の要素の内容をスタックに積む
void vmLdByt() {
  if ((topSta==ACC || topSta==STK) &&           // 添字が Acc を利用し、かつ、
      secSta!=ACC && secSta!=STK) {             //  アドレスがメモリオペランド
    clearFlg();                                 //   フラグ値の変化にそなえる
    loadStk(0);                                 //   添字を Acc にロードし
    calReg("ADD", topAux, stkPtr-1);            //   左右逆に計算する
    if (isAcc(stkPtr-1)) relAcc(secAux);        //   アドレスが Acc 利用なら解放
    secAux = topAux;                            //   計算結果を格納した Acc
  } else if (!isCnst0(stkPtr)||secSta!=RVAR) {  // Acc を利用する必要がある
    loadStk(1);                                 //  アドレスを Acc にロードする
    if (!isCnst0(stkPtr)) {                     //  添字が定数 0 以外なら
      clearFlg();                               //   フラグ値の変化にそなえる
      calReg("ADD", secAux, stkPtr);            //   アドレスに添字を足す
    }
    if (isAcc(stkPtr)) relAcc(topAux);          // 添字が Acc なら解放する
  }
  popStk(1);                                    // 添字をスタックから捨てる
  topSta = BINDR;                               // スタックには要素のアドレス
}

// まず、スタックから添字とバイト配列の番地を取り出す
// 次に、配列の要素に新しいスタックトップの値を
// ストアする(後半でPOPはしない)
void vmStWrd() {
  vmLdWrd();                                    // レジスタにアドレスを計算する
  if (secSta!=RVAR) loadStk(1);                 // Src を Acc にロード
//printf("\tST\t%s,%%%s\n",                     //   ST Acc,%Dst
  printf("\tST\t%s,0,%s\n",                     //   ST Acc,0,Dst
	 regs[secAux], regs[topAux]);
  if (topAux<AccSIZ) relAcc(topAux);            // Dst が Acc なら解放する
  popStk(1);                                    // アドレスは捨てる
}

// まず、スタックトップからバイトデータの番地を取り出す
// 次に、その番地に新しいスタックトップの値を
// ストアする(後半でPOPはしない)
void vmStByt() {
  vmLdByt();                                    // レジスタにアドレスを計算する
  if (secSta!=RVAR) loadStk(1);                 // データを Acc にロード
  printf("\tST\t%s,@%s\n",                      //   ST Acc,@Dst
	 regs[secAux], regs[topAux]);
  if (topAux<AccSIZ) relAcc(topAux);            // Dst が Acc なら解放する
  popStk(1);                                    // アドレスは捨てる
}                                               //   使用できない

// まず、スタックから整数を取り出し２の補数を計算する
// 次に、計算結果をスタックに積む
void vmNeg() {
  clearFlg();                                   // フラグ値の変化にそなえる
  loadStk(0);                                   // スタックトップを Acc に移動
  char *reg = regs[topAux];                     // Acc のレジスタ名
  printf("\tXOR\t%s,#-1\n", reg);               //   XOR Acc,#-1
  printf("\tADD\t%s,#1\n", reg);                //   ADD Acc,#1
  flgPtr = stkPtr;                              // フラグが有効
}

// まず、スタックから論理値を取り出し否定を計算する
// 次に、計算結果をスタックに積む
void vmNot() {
  clearFlg();                                   // フラグ値の変化にそなえる
  loadStk(0);                                   // スタックトップを Acc に移動
  printf("\tXOR\t%s,#1\n", regs[topAux]);       //   XOR Acc,#1
  flgPtr = stkPtr;                              // フラグが有効
}

// まず、スタックから整数を取り出し１の補数を計算する
// 次に、計算結果をスタックに積む
void vmBNot() {
  clearFlg();                                   // フラグ値の変化にそなえる
  loadStk(0);                                   // スタックトップを Acc に移動
  printf("\tXOR\t%s,#-1\n", regs[topAux]);      //   XOR Acc,#-1
  flgPtr = stkPtr;                              // フラグが有効
}

// まず、スタックから整数を取り出し文字型で有効なビット数だけ残しマスクする
// 次に、計算結果をスタックに積む
void vmChar() {
  clearFlg();                                   // フラグ値の変化にそなえる
  loadStk(0);                                   // スタックトップを Acc に移動
  printf("\tAND\t%s,#0x00ff\n", regs[topAux]);  //   AND Acc,#0x00ff
  flgPtr = stkPtr;                              // フラグが有効
}

// まず、スタックから整数を取り出し最下位ビットだけ残しマスクする
// 次に、計算結果をスタックに積む
void vmBool() {
  clearFlg();                                   // フラグ値の変化にそなえる
  loadStk(0);                                   // スタックトップを Acc に移動
  printf("\tAND\t%s,#1\n", regs[topAux]);       //   AND Acc,#1
  flgPtr = stkPtr;                              // フラグが有効
}

// 多くの演算命令の共通処理
static void cal(char *op) {
  clearFlg();                                   // フラグ値の変化にそなえる
  loadStk(1);                                   // Dst を Acc にロード
  if (topSta==STK || topSta==FLAG) loadStk(0);  // 直接演算不可の Src ならロード
  calReg(op, secAux, stkPtr);                   // 演算(op acc,src)を行う
  if (isAcc(stkPtr)) relAcc(topAux);            // Src が Acc を使用中なら解放
  popStk(1);                                    // スタックが縮小
  flgPtr = stkPtr;                              // フラグが有効
}

#define SwapInt(x,y) {int t=(x); (x)=(y); (y)=t;}

// 左右逆順になっても良い演算で少し効率アップ
static void rcal(char *op) {
  if (topSta==ACC && secSta!=ACC &&             // Src がAccで Dst がAcc以外の
      secSta!=STK && secSta!=FLAG) {            //  直接演算可のオペランドなら
    SwapInt(topSta, secSta);                    //    Src と Dst を交換し
    SwapInt(topAux, secAux);
  }
  cal(op);                                      // 通常の演算手順で処理
}

// まず、スタックから整数を二つ取り出し和を計算する
// 次に、計算結果をスタックに積む
void vmAdd() {
  rcal("ADD");                                  // 左右逆順ありのコード生成
}

// まず、スタックから整数を二つ取り出し差を計算する
// 次に、計算結果をスタックに積む
void vmSub() {
  cal("SUB");                                   // 左右逆順なしのコード生成
}

// まず、スタックからシフトするビット数、シフトされるデータの順で取り出す。
// 次に、左シフトした結果をスタックに積む
void vmShl() {
  cal("SHLA");                                  // 左右逆順なしのコード生成
}

// まず、スタックからシフトするビット数、シフトされるデータの順で取り出す。
// 次に、右シフトした結果をスタックに積む
void vmShr() {
  cal("SHRA");                                  // 左右逆順なしのコード生成
}

// まず、スタックから整数を二つ取り出しビット毎の論理積を計算する
// 次に、計算結果をスタックに積む
void vmBAnd() {
  rcal("AND");                                  // 左右逆順ありのコード生成
}

// まず、スタックから整数を二つ取り出しビット毎の排他的論理和を計算する
// 次に、計算結果をスタックに積む
void vmBXor() {
  rcal("XOR");                                  // 左右逆順ありのコード生成
}

// まず、スタックから整数を二つ取り出しビット毎の論理和を計算する
// 次に、計算結果をスタックに積む
void vmBOr() {
  rcal("OR");                                   // 左右逆順ありのコード生成
}

// 2 のベキ乗を判定
static int isPower2(int n) {
  int p = 1;
  for (int i=0; i<NWORD-1; i=i+1) {
    if (n==p) return i;
    p = p << 1;
  }
  return -1;
}

// まず、スタックから整数を二つ取り出し積を計算する
// 次に、計算結果をスタックに積む
void vmMul() {
  int n;
  if (topSta==CNST && (n=isPower2(topAux))>0) { // x * (2^n) なら
    topAux = n;                                 //  (n==0 は削除されているはず)
    cal("SHLA");                                //  x << n に置換える
  } else if (secSta==CNST &&                    //
	     (n=isPower2(secAux))>0) {          // (2^n) * x なら 
    secSta = topSta;                            //
    secAux = topAux;                            // x << n に置き換える
    topSta = CNST;                              //
    topAux = n;                                 //
    cal("SHLA");                                //
  } else  rcal("MUL");                          //  普通の掛け算のコード生成
}

// まず、スタックから整数を二つ取り出し商を計算する
// 次に、計算結果をスタックに積む
void vmDiv() {
  int n;
  if (topSta==CNST && (n=isPower2(topAux))>0) { // x / (2^n) なら
    topAux = n;                                 //  (n==0 は削除されているはず)
    cal("SHRA");                                //  x >> n に置換える
  } else  cal("DIV");                           //  普通の割り算のコード生成
}

// まず、スタックから整数を二つ取り出し剰余を計算する
// 次に、計算結果をスタックに積む
void vmMod() {
  int n;
  if (topSta==CNST && (n=isPower2(topAux))>0) { // x % (2^n) なら
    topAux = (1<<n) - 1;                        //  (n==0 は削除されているはず)
    rcal("AND");                                //  x & ((1<<n)-1) に置換える
  } else  cal("MOD");                           //  普通の割り算のコード生成
}

// jcc を左右が逆の条件にする
static int swapJcc(int jcc ) {
  int cc = jcc;
  if      (cc==JGT) cc = JLT;                   // GT( >  ) は LT( <  )
  else if (cc==JGE) cc = JLE;                   // GE( >= ) は LE( <= )
  else if (cc==JLT) cc = JGT;                   // LT( <  ) は GT( >  )
  else if (cc==JLE) cc = JGE;                   // LE( <= ) は GE( >= )
  return cc;
}

// 比較のコード生成
static void cmp(int jcc) {
  if (isCnst0(stkPtr) &&                        // Src が定数 0 で
      flgPtr==stkPtr-1) {                       //   Dst の評価がフラグにあれば
                                                //    何もしない
  } else if (isCnst0(stkPtr-1) &&               // Dst が定数の 0 で
      flgPtr==stkPtr) {                         //   Src の評価がフラグにあれば
    jcc = swapJcc(jcc);                         //    フラグ値判断用の Jcc 変更
  } else {                                      // Src, Dst の両方が定数 0 以外
    if ((topSta==ACC || topSta==RVAR) &&        //   Src がどれかレジスタにあり
	secSta!=ACC && secSta!=RVAR &&          //   Dst がメモリオペランドなら
	secSta!=STK && secSta!=FLAG ) {         //   (CNST,GVAR,LVAR,ARG,...)
      jcc = swapJcc(jcc);                       //   条件を逆にし
      SwapInt(topSta, secSta);                  //    Dst と Src を入れ替える
      SwapInt(topAux, secAux);
    }
    clearFlg();                                 //   フラグ値の変化にそなえる
    if (secSta!=RVAR) loadStk(1);               //   Dst を Acc にロード
    if (topSta==STK||topSta==FLAG) loadStk(0);  //   直接演算不可ならロード
    calReg("CMP", secAux, stkPtr);              //   比較を行う
  }
  if (isAcc(stkPtr)) relAcc(topAux);            // Src が Acc を使用中なら解放
  if (isAcc(stkPtr-1)) relAcc(secAux);          // Dst が Acc を使用中なら解放
  popStk(2);                                    // 一旦、スタックを解放
  if (accCnt>=AccSIZ) outStk();                 // Acc に空きがなければ作り
  pushStk(FLAG, allocAcc());                    // 結果 (FLAG, Acc) を記録する
  flgPtr = stkPtr;                              // フラグが値を持っている
  flgJcc = jcc;                                 // 論理値に変換するための Jcc
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmGt() {
  cmp(JGT);                                     // 比較命令を生成
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmGe() {
  cmp(JGE);                                     // 比較命令を生成
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmLt() {
  cmp(JLT);                                     // 比較命令を生成
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmLe() {
  cmp(JLE);                                     // 比較命令を生成
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmEq() {
  cmp(JEQ);                                     // 比較命令を生成
}

// まず、スタックから整数を二つ取り出し比較する
// 次に、比較結果(論理値)をスタックに積む
void vmNe() {
  cmp(JNZ);                                     // 比較命令を生成
}

// スタックから値を一つ取り出し捨てる
void vmPop() {
  if (isAcc(stkPtr)) relAcc(topAux);            // スタックトップが Acc なら解放
  //  else BUG(topSta!=RVAR, "vmPop");              // スタックトップがレジスタ以外
  popStk(1);
}

/*
 *  マクロ命令
 */
// フラグ以外にある論理値を Acc に論理値をロードする
static void boolOrAnd(int lab1, int lab2, int lab3, int x, int nx) {
  loadStk(0);                                   //   Acc にロードする
  char *acc = regs[topAux];
  printf("\tJMP\t.L%d\n", lab3);                //       JMP .L3
  vmTmpLab(lab1);                               //  .L1:
  printf("\tLD\t%s,#%d\n", acc, x);             //       LD  Acc,#x
  if (lab2!=-1) {
    printf("\tJMP\t.L%d\n", lab3);              //       JMP .L3
    vmTmpLab(lab2);                             //  .L2:
    printf("\tLD\t%s,#%d\n", acc, nx);          //       LD  Acc,#nx
  }
  vmTmpLab(lab3);                               //  .L3:
}

// フラグにある論理値を Acc に論理値をロードする
static void flagOrAnd(int lab1, int lab2, int lab3, char *jcc, int x, int nx) {
  char *acc = regs[topAux];                     //  論理値を求める Acc
  if (lab2==-1) {                               //  lab2 が未割り当てなら
    printf("\tLD\t%s,#%d\n", acc, nx);          //         LD  Acc,#nx 
    printf("\t%s\t.L%d\n", jcc, lab3);          //         Jcc .L3
    vmTmpLab(lab1);                             //    .L1:
    printf("\tLD\t%s,#%d\n", acc, x);           //         LD  Acc,#x 
  } else {                                      //  lab2 が割り当て済み
    printf("\t%s\t.L%d\n", jcc, lab2);          //         Jcc .L2
    vmTmpLab(lab1);                             //    .L1:
    printf("\tLD\t%s,#%d\n", acc, x);           //         LD  Acc,#x 
    printf("\tJMP\t.L%d\n", lab3);              //         JMP .L3
    vmTmpLab(lab2);                             //    .L2:
    printf("\tLD\t%s,#%d\n", acc, nx);          //         LD  Acc,#nx 
  }
  vmTmpLab(lab3);                               //  .L3:
  topSta = ACC;                                 //  状態は (ACC, Acc) になる
}

// BOOLOR .L1,.L2,.L3 マクロ命令(論理式の最後で論理値を Acc にロードするマクロ)
void vmBoolOR(int lab1, int lab2, int lab3) {
  if (topSta==FLAG)                             // フラグに結果があるなら
    flagOrAnd(lab1, lab2, lab3, jcc[(flgJcc+3)%6], 1, 0);
  else                                          // フラグ以外にあるなら
    boolOrAnd(lab1, lab2, lab3, 1, 0);
}

// BOOLAND .L1,.L2,.L3 マクロ命令(論理式の最後で論理値を Acc にロードするマクロ)
void vmBoolAND(int lab1, int lab2, int lab3) {
  if (topSta==FLAG)                             // フラグに結果があるなら
    flagOrAnd(lab1, lab2, lab3, jcc[flgJcc], 0, 1);
  else                                          // フラグ以外にあるなら
    boolOrAnd(lab1, lab2, lab3, 0, 1);
}

/*
 *  データ生成用の疑似命令
 */
// DW name      (ポインタデータの生成)
void vmDwName(int idx) {
  printf("\tDW\t%c%s\n",
	 getPref(topAux), ntGetName(topAux));   //    DW  _name
}

// DW .Ln       (ポインタデータの生成)
void vmDwLab(int lab) {
  printf("\tDW\t.L%d\n", lab);                  //    DW  .Ln
}

// DW .Sn       (STRING用ポインタデータの生成)
void vmDwLabStr(int lab) {
  printf("\tDW\t.S%d\n", lab);                  //    DW  .Sn
}

// DW N         (整数データの生成)
void vmDwCns(int n) {
  printf("\tDW\t%d\n", n);                      //    DW  N
}

// DB N         (整数データの生成)
void vmDbCns(int n) {
  printf("\tDB\t%d\n", n);                      //    DB  N
}

// WS N         (ワード領域の生成)
void vmWs(int n) {
  printf("\tWS\t%d\n", n);                      //    WS  N
}

// BS N         (バイト領域の生成)
void vmBs(int n) {
  printf("\tBS\t%d\n", n);                      //    WS  N
}

// STRING "..." (文字列データの生成)
void vmStr(char *s) {
  printf("\tSTRING\t\"%s\"\n", s);              //    STRING "str"
}

// 10進数を読んで値を返す
static int getDec() {
  int v = 0;                                     // 初期値は 0
  char ch = fgetc(fp);
  boolean minusflg = false;
  if(ch==EOF)
    return EOF;
  else if(ch=='-'){
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

static void callfunc0(int op){
       if(op==22) vmRet();
  else if(op==23) vmRetI();
  else if(op==24) vmMReg();
  else if(op==25) vmArg();
  else if(op==26) vmLdWrd();
  else if(op==27) vmLdByt();
  else if(op==28) vmStWrd();
  else if(op==29) vmStByt();
  else if(op==30) vmNeg();
  else if(op==31) vmNot();
  else if(op==32) vmBNot();
  else if(op==33) vmChar();
  else if(op==34) vmBool();
  else if(op==35) vmAdd();
  else if(op==36) vmSub();
  else if(op==37) vmShl();
  else if(op==38) vmShr();
  else if(op==39) vmBAnd();
  else if(op==40) vmBXor();
  else if(op==41) vmBOr();
  else if(op==42) vmMul();
  else if(op==43) vmDiv();
  else if(op==44) vmMod();
  else if(op==45) vmGt();
  else if(op==46) vmGe();
  else if(op==47) vmLt();
  else if(op==48) vmLe();
  else if(op==49) vmEq();
  else if(op==50) vmNe();
  else if(op==51) vmPop();
}

static void callfunc1(int op, int a1){
       if(op==0)  vmName(a1);
  else if(op==1)  vmTmpLab(a1);
  else if(op==2)  vmTmpLabStr(a1);
  else if(op==3)  vmJmp(a1);
  else if(op==4)  vmJT(a1);
  else if(op==5)  vmJF(a1);
  else if(op==6)  vmLdCns(a1);
  else if(op==7)  vmLdGlb(a1);
  else if(op==8)  vmLdLoc(a1);
  else if(op==9)  vmLdArg(a1);
  else if(op==10) vmLdStr(a1);
  else if(op==11) vmLdLab(a1);
  else if(op==12) vmStGlb(a1);
  else if(op==13) vmStLoc(a1);
  else if(op==14) vmStArg(a1);
  else if(op==15) vmDwName(a1);
  else if(op==16) vmDwLab(a1);
  else if(op==17) vmDwLabStr(a1);
  else if(op==18) vmDwCns(a1);
  else if(op==19) vmDbCns(a1);
  else if(op==20) vmWs(a1);
  else if(op==21) vmBs(a1);
}

static void callfunc2(int op, int a1, int a2){
       if(op==52)  vmEntry(a1, a2);
  else if(op==53)  vmEntryK(a1, a2);
  else if(op==54)  vmEntryI(a1, a2);
  else if(op==55)  vmCallP(a1, a2);
  else if(op==56)  vmCallF(a1, a2);
}

int main(int argc, char *argv[]){
  int op;
  if (argc==2){
    if((fp = fopen(argv[1],"r")) == NULL){   // 中間ファイルをオープン
      perror(argv[1]);                       // オープン失敗の場合は、メッ
      exit(1);                               // セージを出力して終了
    }
  }else if(argc==1){
    fp = stdin;
  }else{
    exit(1);
  }
    while(true){
    op = getDec();
    if(op==EOF)
      return 0;
    if(22<=op && op<=51){                    // 引数0の関数
      callfunc0(op);
    }else if(0<=op && op<=21){   // 引数1の関数
      int a1 = getDec();
      callfunc1(op, a1);
    }else if(52<=op && op<=56){              // 引数2の関数
      int a1 = getDec();
      int a2 = getDec();
      callfunc2(op, a1, a2);
    }else if(op==57 || op==58){              // 引数3の関数
      int a1 = getDec();
      int a2 = getDec();
      int a3 = getDec();
      if(op==57) vmBoolOR(a1, a2, a3);
      else       vmBoolAND(a1, a2, a3);
    }else if(op==60){ // 名前表登録
      fgetc(fp);  // P読み捨て
      fgetc(fp);  // 空白読み捨て
      int i=0;
      char c;
      while((c=fgetc(fp))!=' '){
        if(i>StrMAX)
          error("名前が長すぎる");
        str[i] = c;
        i = i+1;
      }
      str[i] = '\0';
      int scp  = getDec();
      int type = getDec();
      int dim  = getDec();
      int val  = getDec();
      int pub  = getDec();
      ntDefName(str, scp, type, dim, val, pub);
    }else if(op==59){
      int i=0;
      char ch;
      while((ch=fgetc(fp))!='\n'){               // 改行がくるまで文字列
        if(i>StrMAX)
          error("文字列が長すぎる");
        str[i] = ch;
        i = i+1;
      }
      str[i] = '\0';
      vmStr(str);
    }else{
      error("bug");
    }
  }
  return 0;
}
