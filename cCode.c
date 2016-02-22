/*
 * Programing Language C-- "Compiler"
 *    Tokuyama kousen Educational Computer 16bit Ver.
 *
 * Copyright (C) 2002-2014 by
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
 * syntax.c : C--トランスレータのコード生成部
 *
 * 2014.04.29 v2.1.0  : 初期バージョン(柳くん)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "code.h"
#include "namtbl.h"
#include "sytree.h"
#include "lexical.h"

static void traceTree(int node);
static boolean printFlag = true;

// staticの出力
static void printStatic(int idx) {
  if(!ntGetPub(idx))                                // public ではないなら
    printf("static ");                              //   static を出力
}

// 型名を出力する
static void printType(int idx) {
  int typ = ntGetType(idx);
  if (typ==TyVOID) printf("void");                  // void を出力
  else if (typ==TyINT) printf("int");               // intを出力
  else if (typ==TyBOOL) printf("int");              // boolean なら int を出力 
  else if (typ==TyCHAR) printf("char");             // charを出力
  else if (typ==TyINTR)  printf("interrupt");       // interruptを出力
  else if (typ==TyDOTDOTDOT) printf("...");         // ...を出力
  else if (typ <= 0 && ntGetType(-typ) == TySTRUC)  // 構造体なら
      printf("struct %s",ntGetName(-typ));          //  struct 構造体名を出力
  else if (typ <= 0 && ntGetType(-typ) == TyREF)    // typedef なら
      printf("%s",ntGetName(-typ));                 //  型名を出力
  else error("バグ...printType");                   // 型ではなかったとき
}

// 後置演算以外の演算子の出力
static void printOP(int op){
  if (op==SyNEG)       printf("-");                 // 単項演算子 -
  else if (op==SyNOT)  printf("!");                 // 単項演算子 !
  else if (op==SyBNOT) printf("~");                 // 単項演算子 ~
  else if (op==SyADD)  printf("+");                 // ２項演算子 +
  else if(op==SySUB)   printf("-");                 // ２項演算子 -
  else if(op==SySHL)   printf("<<");                // ２項演算子 <<
  else if(op==SySHR)   printf(">>");                // ２項演算子 >>
  else if(op==SyBAND)  printf("&");                 // ２項演算子 &
  else if(op==SyBXOR)  printf("^");                 // ２項演算子 ^
  else if(op==SyBOR)   printf("|");                 // ２項演算子 |
  else if(op==SyMUL)   printf("*");                 // ２項演算子 *
  else if(op==SyDIV)   printf("/");                 // ２項演算子 /
  else if(op==SyMOD)   printf("%%");                // ２項演算子 %
  else if(op==SyGT)    printf(">");                 // ２項演算子 >
  else if(op==SyGE)    printf(">=");                // ２項演算子 >=
  else if(op==SyLT)    printf("<");                 // ２項演算子 <
  else if(op==SyLE)    printf("<=");                // ２項演算子 <=
  else if(op==SyEQU)   printf("==");                // ２項演算子 ==
  else if(op==SyNEQ)   printf("!=");                // ２項演算子 !=
  else if(op==SyASS)   printf("=");                 // ２項演算子 =
  else if(op==SyCOMM)  printf(",");                 // ２項演算子 ,
  else if(op==SyOR)    printf("||");                // ２項演算子 ||
  else if(op==SyAND)   printf("&&");                // ２項演算子 &&
  else error("バグ...printOP");                     // 演算子でなかった
}

static void printExp(int node);

// 引数の出力
static void printArg(int node){
  if(node==SyNULL) error("バグ...printArg");        // SyNULLなら呼出し側にバグ
  if(syGetType(node)==SySEMI) {                     // 引数リストなら
    printArg(syGetLVal(node));                      //   左ノードを出力
    printf(",");                                    //   引数の間に , を出力
    printArg(syGetRVal(node));                      //   右ノードを出力
  } else {                                          // 引数なら
    printExp(node);                                 //   引数式を出力
  }
}

// 式の出力
static void printExp(int node){
  int typ = syGetType(node);
  int lVal = syGetLVal(node);
  int rVal = syGetRVal(node);
  if(typ==SyCNST)                                   // 定数なら
    printf("%d",syGetLVal(node));                   //   値を出力
  else if(typ==SySTR)                               // 文字列なら
    printf("_cmm_str_L%d",lVal);                    //   文字列変数名を出力
  else if (typ==SyLOC)                              // ローカル変数なら
    printf("%s",ntGetName(lVal));                   //   名前を出力
  else if (typ==SyARG)                              // 仮引数なら
    printf("%s",ntGetName(lVal));                   //   名前を出力
  else if (typ==SyGLB)                              // グローバル変数なら
    printf("%s",ntGetName(lVal));                   //   名前を出力
  else if (typ==SyLABL)                             // addrofなら
    printf("addrof(%s)",ntGetName(lVal));           //   addrof(変数)を出力
  else if (typ==SyFUNC){                            // 関数なら
    printf("%s(",ntGetName(lVal));                  //   "名前("と
    if (rVal!=SyNULL) printArg(rVal);               //   "引数)"を出力する
    printf(")");                                    //
  } else if (typ==SyIDXW || typ==SyIDXB) {          // [] なら
    printExp(syGetLVal(lVal));                      //   左辺式を出力
    printf("[");
    printExp(syGetRVal(lVal));                      //   [右辺式]を出力
    printf("]");
  } else if(typ==SyDOT) {                           // . なら
    printExp(lVal);                                 //  左辺式を出力
    printf("->");                                   //  アロー演算子を出力
    int s_type = syGetLVal(rVal);                   // 構造体の型を取得
    int f_num = syGetRVal(rVal);                    // フィールド番号を取得
    printf("%s",ntGetName(s_type+f_num+1));         // フィールド名を出力
  } else if(typ==SyCHAR){
    printf("(0xff&");
    printExp(lVal);
    printf(")");
  } else if(typ==SyBOOL){
    printf("(0x01&");
    printExp(lVal);
    printf(")");
  } else if (SyIS1OPR(typ)) {                       // 後置以外の単項演算なら
    printf("(");
    printOP(typ);
    printExp(lVal);                                 //   "(演算子 式)"を出力
    printf(")");
  } else {                                          // それ以外なら２項演算
    printf("(");
    printExp(lVal);
    printOP(typ);                                   //   "(左辺 演算子 右辺)"
    printExp(rVal);                                 //    と出力
    printf(")");
  }
}

// if文の出力
static void printIf(int node){
  printf("if (");                                   // if(
  printExp(syGetLVal(node));                        // 条件式
  printf(")");                                      // )
  traceTree(syGetRVal(node));                       // 文
}

// if-else文の出力
static void printEls(int node){
  printIf(syGetLVal(node));                        // if文
  printf("else ");                                  // else
  traceTree(syGetRVal(node));                      // 文
}

// while文の出力
static void printWhl(int node){
  int lVal = syGetLVal(node);
  int rVal = syGetRVal(node);
  printf("while (");               // while(
  if(lVal==SyNULL)
    printf("1");
  else
    printExp(lVal);                  // 条件式
  printf(")");                     // )
  if(syGetType(rVal)==SyNULL)
    printf(";\n");
  else
    traceTree(syGetRVal(node));      // 文
}

// do-while文の出力
static void printDo(int node){
  int lVal = syGetLVal(node);
  int rVal = syGetRVal(node);
  printf("do");                    // do
  traceTree(lVal);                 // 文　
  printf("while (");               // while (
  printExp(rVal);                  // 条件式
  printf(");\n");                  // );と出力
}

// break文の出力
static void printBrk(int node){
  printf("break;\n");              // break;と出力
}

// continue文の出力
static void printCnt(int node){
  printf("continue;\n");           // continueと出力
}

// return文の出力
static void printRet(int node){
  int lVal = syGetLVal(node);      // return式を取り出して
  printf("return ");               // return
  printExp(lVal);                  // 式
  printf(";\n");                   // ; と出力
}

// 変数宣言の出力
static void printVAR(int node){  
  int idx = syGetLVal(node);       // インデックスを取り出す
  printType(idx);                  // 型を出力
  printf(" ");
  for(int i = 0; i < ntGetDim(idx); i++)
    printf("*");
  if(ntGetType(idx) <= 0 )         // 構造体かtypedefなら
    printf("*");                   // * がいる
  printf("%s;\n",ntGetName(idx));  // 変数名を出力
}

// ブロックの出力
static void printBLK(int node){
  printf("{\n");                   // { を出力 
  traceTree(syGetLVal(node));      // ブロックで囲まれている範囲を出力
  printf("}\n");                   // } を出力
}

static void traceTree(int node){
  if (node==SyNULL) return;                // 何も無い
  int typ = syGetType(node);
  if (typ==SyIF) printIf(node);            // if 文
  else if (typ==SyELS) printEls(node);     // if-else 文
  else if (typ==SyWHL) printWhl(node);     // while 文
  else if (typ==SyDO)  printDo(node);      // do-while 文
  else if (typ==SyBRK) printBrk(node);     // break 文
  else if (typ==SyCNT) printCnt(node);     // continue 文
  else if (typ==SyRET) printRet(node);     // return 文
  else if (typ==SyVAR) printVAR(node);     // 変数宣言
  else if (typ==SyBLK) printBLK(node);     // ブロック
  else if (typ==SySEMI) {                  // セミコロン
    traceTree(syGetLVal(node));            // 先に左側をコード生成
    traceTree(syGetRVal(node));            // 次に右側をコード生成
  }
  else{                                    // 式文
    printExp(node);                        // 式の出力
    printf(";\n");
  }
}    

// 関数の引数の出力
static void printFArg(int idx){                
  int cnt = ntGetCnt(idx);                  // 引数の数を取得
  if(cnt == 0)                              // 引数がないなら
    return;                                 //  終了
  else{                                     // 引数が存在するなら
    int i = 1;
    while(true){
      int dim = ntGetDim(idx+i);            // 引数の次元数を取得
      printType(idx+i);                     // 型を出力
      printf(" ");
      for(int j=0; j<dim; j++)              // 次元数の数だけ * を出力
	printf("*");
      if(ntGetType(idx+i)<=0)
	printf("*");
      printf("%s",ntGetName(idx+i));        // 変数名を出力
      if(i==cnt)                            // 引数の個数分出力したなら
	return;                             //  終了
      else{                                 // まだ引数があるなら
	printf(",");                        //  , を出力
	i++;                                //  i をインクリメント
      }
    }
  }
}

// 関数の１行目を印刷する
static void printFunc(int funcIdx) {
  printStatic(funcIdx);                // static が必要なら出力
  printType(funcIdx);                  // 型を出力
  printf(" ");
  int dim = ntGetDim(funcIdx);         // 次元数を取得
  for(int i = 0; i < dim; i++)         // 次元数の数だけ
    printf("*");                       // * を出力
  if(ntGetType(funcIdx)<=0)
    printf("*");
  printf("%s(", ntGetName(funcIdx));   // 名前を出力
  printFArg(funcIdx);                  // 引数の出力
  printf(")");
}

// プロトタイプ宣言の出力
void genProto(int funcIdx) {
  if (!printFlag) return;

  printFunc(funcIdx);
  printf(";\n");                       //  ; を出力して終了
}

// 関数の出力
void genFunc(int funcIdx,boolean prot, boolean kernFlg){
  if (!printFlag) return;

  printFunc(funcIdx);
  int root = syGetRoot();              //  構文木の根をとって
  traceTree(root);                     //  解析開始
}

int lab = 0;

// 配列のラベルの割当
static int newLab(){
  int l = lab;
  lab++;
  return l;
}

// 非初期化配列の出力
static int printArray0(int node, int cnt, int dim, int idx){
  int typ  = syGetType(node);
  int rVal = syGetRVal(node);
  int lVal = syGetLVal(node);
  int l = -1;                                          // この次元の最初のラベル
  if (typ==SySEMI) {                                   // 最後の次元でない時
    if (syGetType(rVal)!=SyCNST)                       // 定数でなければエラー
      error("バグ...genArray0_1");
    int size = syGetLVal(rVal);                        // 現在の配列サイズ
    int ln = printArray0(lVal, cnt*size, dim-1, idx);  // 入れ子の配列を生成し
    for (int i=0; i<cnt; i++) {                        // 前の次元の要素数分
      int l2 = newLab();                               // ラベルを生成し
      if (l==-1) l = l2;                               // この次元の最初を記憶
      printf("static ");                               // 必ず static を出力
      printType(idx);                                  // 型を出力
      printf(" ");
      for(int j=0; j<dim; j++)                         // 現在の次元数分の
	printf("*");                                   //  * を出力
      if(ntGetType(idx)<=0)                            // 構造体なら
	printf("*");                                   //  もう一つ * を出力
      printf("_cmm_%s_cmm_%d[] = {",ntGetName(idx),l2);                         
      printf("_cmm_%s_cmm_%d",ntGetName(idx),ln);      // 最初 , はいらない
      ln = ln + 1;
      for (int j=1; j<size; j++) {                     // 要素数の数だけ
	printf(", _cmm_%s_cmm_%d",ntGetName(idx),ln);  // インスタンスを生成
	ln = ln + 1;
      }
      printf("};\n");                                  // 最後に } をつける
    }
  } else if (typ==SyCNST) {                            // 最後の次元の時
    if (ntGetType(idx)<=0){                            // 構造体なら
      int l3 = -1;
      for(int i=0; i<cnt*lVal; i++){                   // 構造体を生成
	int l2 = newLab();                             // ラベルを生成し
	if(l3 == -1)  l3 = l2;                         // 最初の添え字を保存
	printf("static ");                             // 必ず static
	printType(idx);                                // 型を出力
	printf(" _cmm_%s_cmm_%d;\n",ntGetName(idx),l2);// 名前を出力
      }
      for(int i=0; i<cnt; i++){                        // 構造体配列を生成
	printf("static ");                             // 必ず static
	printType(idx);                                // 型を出力
	int l2 = newLab();                             // ラベルを生成し
	if (l==-1) l = l2;                             // この次元の最初を記憶
	printf(" *_cmm_%s_cmm_%d[] = {",ntGetName(idx),l2);
	int ll = i*lVal+l3;
	printf("&_cmm_%s_cmm_%d",ntGetName(idx),ll);   // 最初は, をつけない
	for (int j=1; j<lVal; j++) {                   // 要素数の数だけ
	  ll = i*lVal+j+l3;                            // 変換して出力
	  printf(", &_cmm_%s_cmm_%d",ntGetName(idx),ll);   
	}
	printf("};\n");                                // 最後に } をつける
      }
    }
    else{                                              // 構造体でないなら
      for (int i=0; i<cnt; i++) {                      // 前の次元の要素数分の
	int l2 = newLab();                             // ラベルを生成し
	if (l==-1) l = l2;                             // この次元の最初を記憶
	printf("static ");                             // 必ず static を出力
	printType(idx);                                // 型を出力
	printf(" _cmm_%s_cmm_%d[%d];\n",ntGetName(idx),l2,lVal);
      }
    }
  } else error("バグ...genArray0_2");                // それ以外ならバグ
  return l;
}

// 非初期化配列の出力
static void printArray(int node, int dim, int idx){
  int lVal = syGetLVal(node);                   
  int l = printArray0(lVal, 1, dim-1, idx);
  printStatic(idx);                            // staticかどうか出力
  printType(idx);                              // 型を出力
  printf(" ");
  for(int i=0; i<dim; i++)                     // 次元数文の * を出力
    printf("*");
  if(ntGetType(idx)<=0)                        // 構造体なら
    printf("*");                               //   もう一個 * を出力
                                               // 比初期化配列を出力
  printf("%s = _cmm_%s_cmm_%d;\n",ntGetName(idx),ntGetName(idx),l);
}


static void printList1(int node, int idx, int n, int dim){
  int typ  = syGetType(node);
  int lval = syGetLVal(node);
  int rval = syGetRVal(node);
  if(n == 1)                                        // 二個目以降なら
    printf(",");                                    //  ','もいる
  if (typ==SyCNST)                                  // 数なら
    printf("%d",lval);                              //  ただ出力
  else if(typ==SySTR)                               // 文字列なら
    printf("_cmm_str_L%d",lval);                    //  文字列変数名を出力    
  else if(typ==SyLIST){                             // リストなら
    if(ntGetType(idx)<=0&&dim==2)                   //  構造体だったなら
      printf("&");                                  //   '&'が必要
    printf("_cmm_%s_cmm_%d",ntGetName(idx),rval);   //  仮変数を出力
  }
  else if(typ==SyARRY){                             // 非初期化配列だったなら
    printf("_cmm_%s_cmm_%d",ntGetName(idx),rval);   // 右ノードに添え字がある
  }
} 

// 配列の初期化コード生成
static void printList0(int node, int idx, int dim, int dim2, int n) {
  int typ  = syGetType(node);
  int lVal = syGetLVal(node);
  int rVal = syGetRVal(node);
  if (typ==SySEMI) {                             // リストの途中
    printList0(lVal, idx, dim, dim2, 0);         //  左から
    printList0(rVal, idx, dim, dim2, 1);         //  右の順番で辿る
  } else if (dim==0) {                           // 出力すべき深さ      
    printList1(node,idx,n,dim2);                 //  データを出力  
  } else if (typ==SyLIST) {                      // 初期化配列を発見
    if (dim==1) {                                // コード生成直前
      int l = newLab();                          //  ラベル番号を取り出し
      sySetRVal(node, l);                        //  番号を保存
      printf("static ");                         //  必ず'static'を出力
      printType(idx);                            //  型を出力
      printf(" ");
      int i;
      for(i = 0; i < dim2-1; i++)                // 次元数分の * を出力
	printf("*");
      printf("_cmm_%s_cmm_%d",ntGetName(idx),l); // 変数名の出力
      if(!(ntGetType(idx)<=0&&dim==dim2))
	printf("[]");                            // '[]'を出力
      printf(" = {");
      printList0(lVal, idx, dim-1, dim2, n);     // 配列の本体を辿る
      printf("};\n");
    }
    else
      printList0(lVal, idx, dim-1, dim2-1, 0);   // 配列の本体を辿る
  } else if (typ==SyARRY) {                      // 非初期化配列が含まれていた
    if (dim==1){                                 // 出力するべき深さなら
      int l = printArray0(lVal, 1, dim, idx);    // 非初期化配列生成
      sySetRVal(node, l);                        // ラベル番号を保存しておく
    }
  }
}

// 初期化配列の出力
static void printList(int node, int idx) {      // 開始位置と配列の次元、型
  int dim = ntGetDim(idx);
  if(ntGetType(idx)<=0)                         // 構造体なら
    dim = dim + 1;                              // 次元数より一つ多めに探索
  int dim2 = dim;                               // 次元数を保管
  while (dim>0) {                               // 深い方(末端)から順に
    printList0(node, idx, dim, dim2, 0);        //   変換する
    dim--;
  }
  printStatic(idx);
  printType(idx);
  printf(" ");
  int i;
  for(i = 0; i < dim2; i++)
    printf("*");
  printf("%s = ",ntGetName(idx));
  if(ntGetType(idx)<=0&&dim2==1)
    printf("&");
  int l = newLab();
  printf("_cmm_%s_cmm_%d;\n",ntGetName(idx),l-1);
}

// 初期化データの出力
void genData(int idx){
  if (!printFlag) return;

  int root = syGetRoot();            // 構文木の先頭取得
  int typ = syGetType(root);         // ノードの種類を取得
  //  int lVal = syGetLVal(root);        // 左ノードを取得
  int dim = ntGetDim(idx);           // 次元数を取得
  if(typ==SyARRY)                    // 非初期化配列なら
    printArray(root,dim,idx);        //  比初期化配列の変換出力部へ
  else if (typ==SyLIST)              // 初期化配列なら
    printList(root,idx);             //  初期化配列の変換出力部へ
  else {                             // それ以外なら
    printStatic(idx);                //  static が必要なら出力
    printType(idx);                  // 型を出力
    printf(" ");
    for(int i = 0; i < dim; i++)     //次元数分の * を出力
      printf("*");
    printf("%s = ",ntGetName(idx));  // 名前 = を出力
    printExp(root);                  // 右辺の出力
    printf(";\n");                   //
  }
  lab = 0;                           // 配列用のラベルを初期化する
}

// 非初期化変数の出力
void genBss(int idx){
  if (!printFlag) return;

  int dim = ntGetDim(idx);           // 次元数の取得
  printStatic(idx);                  // static が必要なら出力
  printType(idx);                    // 型を出力
  printf(" ");
  for(int i = 0; i < dim; i++)       // 次元数分の * を出力
    printf("*");
  if(ntGetType(idx)<=0)
    printf("*");
  printf("%s;\n", ntGetName(idx));   // 名前を出力
}

// 文字列のラベルの割当
static int slab = 0;
static int newSLab(){
  int l = slab;
  slab++;
  return l;
}

// 文字列の出力
int genStr(char *str){
  if (!printFlag) return 0;

  int lab = newSLab();                             // ラベルを割り付け
  printf("#define _cmm_str_L%d \"%s\"\n",lab,str); // 出力
  return lab;                                      // ラベル番号を返す
}

// 構造体の出力
void genStruc(int structIdx){
  if (!printFlag) return;

  int cnt = ntGetCnt(structIdx);                // 構造体の要素数を取得
  printf("struct %s{\n",ntGetName(structIdx));  // 構造体名を出力
  for(int i=0; i<cnt; i++){                     // 要素数だけ
    int dim = ntGetDim(structIdx+i+1);          // 次元数を取得
    printType(structIdx+i+1);                   // 型を出力
    printf(" ");
    for(int j=0; j<dim; j++)                    // 次元数分の * を出力
      printf("*");
    if(ntGetType(structIdx+i+1)<=0)
      printf("*");
    printf("%s;\n",ntGetName(structIdx+i+1));   // 名前を出力
  }
   printf("};\n");  
}

// 出力許可
void genOn() {
  printFlag = true;
}

// インクルードディレクティブを印刷して以降の出力禁止
void genOff(char *hdr){
  printf("#include <%s>\n", hdr);
  printFlag = false;
}
