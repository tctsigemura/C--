/*
 * Programing Language C-- "Compiler"
 *    Tokuyama kousen Educational Computer 16bit Ver.
 *
 * Copyright (C) 2014-2016 by
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
 * cCode.c : C--トランスレータのコード生成部
 *
 * 2016.11.20         : for 中の continue が最初期化を行わないバグ
 * 2016.10.16         : elseなしif文だけ"{if(e)...}"と出力する方がスマート
 * 2016.10.14         : 2016.10.09 の変更はNG、if文は"if(e){}else{}"と出力する
 * 2016.10.09         : if-else 文の本体が if 文の場合"{}"を出力する
 * 2016.09.19         : SyLABL を SyADDR に変更
 * 2016.09.18         : SyCHAR を SyCHR に変更
 * 2016.06.12 v3.1.0  : 安定版完成
 * 2016.05.04 v3.0.0  : 重村が引き継ぐ
 * 2014.04.29 v2.1.0  : 初期バージョン(柳くん)
 *
 */

#include <stdio.h>
#include <string.h>
#include "util.h"
#include "code.h"
#include "namtbl.h"
#include "sytree.h"

static void traceTree(int node);                    // 再帰呼び出しのため
static boolean inhibitOut = false;                  // 出力を抑制する

// 構造体や配列の初期化用仮変数名を印刷する
static void printTmpLab(int num) {
  printf("_cmm_%dT", num);                          // "_cmm_番号T"
}

/*
// 文字列ラベルを印刷する
static void printStrLab(int lab,boolean ary) {
  printf("_cmm_%dS", lab);                          // "_cmm_番号S"
}
*/

//改定 文字列ラベルを印刷する
static void printStrLab(int lab,boolean ary,boolean func) {
  if(ary==true && func==false) printf("&");         //参照変数に指されている時
  if(func==true){                                   //関数内の文字列ならば
      printf("_cmm_%dS.a", lab);                    //"_cmm_番号S.a"
  }
  else{
  printf("_cmm_%dS", lab);                          //"_cmm_番号S"
  }
}

// ローカル変数名を印刷する（ローカル変数は番号で管理される）
static void printLocVar(int num) {
  printf("_cmm_%dL", num);                          // "_cmm_番号L"
}

// 仮引数名を印刷する（仮引数は番号で管理される）
static void printParam(int num) {
  printf("_cmm_%dP", num);                          // "_cmm_番号P"
}

// 型名を印刷する
static void printTypeName(int typ) {
  if (typ==TyVOID) printf("void ");                 // "void "
  else if (typ==TyINT) printf("int ");              // "int "
  else if (typ==TyBOOL) printf("char ");            // booleanは"char "に置換え
  else if (typ==TyCHAR) printf("char ");            // char なら "char "
  else if (typ==TyINTR) error("interrupt使用禁止"); // エラー
  else if (typ==TyDOTDOTDOT) printf("...");         // "..."
  else if (typ<=0 && ntGetType(-typ)==TySTRUC)      // 構造体なら
      printf("struct %s ", ntGetName(-typ));        //   "struct 構造体名 "
  else if (typ<=0 && ntGetType(-typ)==TyREF)        // typedef なら
      printf("%s ", ntGetName(-typ));               //   "型名 "
  else error("バグ...printTypeName");               // 型ではなかったとき
}


// 改定  配列のデータ構造の型を印刷する
static void printArrayType(int typ,int dim) {
//一次元配列の参照変数は配列を表す構造体と同じ型にしたい
  if(dim<1){
  if (typ==TyINT) printf("IA ");                    // "IA "
  else if (typ==TyBOOL) printf("CA ");              // booleanは"CA "に置換え
  else if (typ==TyCHAR) printf("CA ");              // char なら "CA "
  }
  else{
  printf("RA ");
  }
}
 

/*
// 型を印刷する
static void printType(boolean sta, int typ, int nAst) {
  if (typ<=0) nAst = nAst + 1;                      // 参照型なら * を追加
  if (sta) printf("static ");                       // "[static ]"
  printTypeName(typ);                               // "型名"
  for (int i=0; i<nAst; i=i+1)                      // "[*...]"
    printf("*");                                    // 
}
 
 */
 

// 改定 型を印刷する
//  ary : trueならば配列
//  ref : trueならば構造体を指す参照変数
static void printType(boolean sta, int typ, int nAst, boolean ary, boolean ref) {
  int dim=nAst;
  if (typ<=0) nAst = nAst + 1;                      // 参照型なら * を追加
  if (sta) printf("static ");                       // "[static ]"
  if(dim>=0 && ary==true){                          //配列ならば構造体で表す
      if(ref==true){
          dim=dim-1;
      }
      printArrayType(typ,dim);                      //"構造体型"
      if(ref==true){                                //参照変数ならば*を出力
          printf("*");
      }
  }
  else{
  printTypeName(typ);                               // "型名"
  for (int i=0; i<nAst; i=i+1)                      // "[*...]"
      printf("*");                                  //
  }
}



// 改定  配列アクセス時の型変換を印刷する
static void printArrayTypePoint(int typ) {
  if (typ==TyVOID) printf("((RA *)(");              // "((RA *)( "
  else if (typ==TyINT) printf("((IA *)(");          // "((IA *)( "
  else if (typ==TyBOOL) printf("((CA *)(");         // booleanは"CA "に置換え
  else if (typ==TyCHAR) printf("((CA *)(");         // char なら "CA "
}

// 改定　カーリブラケットの中身サイズ (まだ未完成)
static void printcurly2(int size,int youso,boolean ary){
  printf("{%d,",size);                              //"{配列サイズ,"
  printf("{");                                      // "{"
}
// 改定　データ構造のカーリーブラケットの中身をかく（まだ未完成）
static void printcurly(int size,int youso,boolean ary){
  int j=0;
  printcurly2(size,youso,ary);
  if(ary==true){
  printf("0");                                      // "0で配列を初期化"
  for (int j=1; j<size; j=j+1) {                    // 要素数の数だけ続ける
      printf(",");                                  //  ","
      printf("0");                                  // 配列の初期化
  }
  }
  else if(ary==false){
          printf("&");
          printTmpLab(youso+j);                     // _cmm_%dT
      for (int j=1; j<size; j=j+1) {                // 要素数の数だけ続ける
          printf(",");                              //  ","
          printf("&");                              // "&"
          printTmpLab(youso+j);                     // _cmm_%dT
      }
  }
  printf("}};\n");                                  // "}};\n"
}
/*
// 名前表からグローバル変数や関数の型と名前を印刷する
static void printGlobDcl(int idx) {
  boolean sta = !ntGetPub(idx);                     // static
  int typ = ntGetType(idx);                         // 型名
  int dim = ntGetDim(idx);                          // 次元
  printType(sta, typ, dim);                         // "[static] 型名[*...]"
  printf("%s", ntGetName(idx));                     // "名前"
}
*/


// 改定 名前表からグローバル変数や関数の型と名前を印刷する
//  ary : trueならば配列を表す構造体
//  ref : trueならば構造体を指す参照変数
static void printGlobDcl(int idx,boolean ary,boolean ref) {
  boolean sta = !ntGetPub(idx);                     // static
  int typ = ntGetType(idx);                         // 型名
  int dim = ntGetDim(idx);                          // 次元
  printType(sta, typ, dim, ary, ref);               // "[static] 型名[*...]"
  printf("%s", ntGetName(idx));                     // "名前"
}


// 後置演算以外の演算子を印刷する
static void printOP(int op){
  if      (op==SyNEG)  printf("-");                 // 単項演算子 -
  else if (op==SyNOT)  printf("!");                 // 単項演算子 !
  else if (op==SyBNOT) printf("~");                 // 単項演算子 ~
  else if (op==SyADD)  printf("+");                 // ２項演算子 +
  else if (op==SySUB)  printf("-");                 // ２項演算子 -
  else if (op==SySHL)  printf("<<");                // ２項演算子 <<
  else if (op==SySHR)  printf(">>");                // ２項演算子 >>
  else if (op==SyBAND) printf("&");                 // ２項演算子 &
  else if (op==SyBXOR) printf("^");                 // ２項演算子 ^
  else if (op==SyBOR)  printf("|");                 // ２項演算子 |
  else if (op==SyMUL)  printf("*");                 // ２項演算子 *
  else if (op==SyDIV)  printf("/");                 // ２項演算子 /
  else if (op==SyMOD)  printf("%%");                // ２項演算子 %
  else if (op==SyGT)   printf(">");                 // ２項演算子 >
  else if (op==SyGE)   printf(">=");                // ２項演算子 >=
  else if (op==SyLT)   printf("<");                 // ２項演算子 <
  else if (op==SyLE)   printf("<=");                // ２項演算子 <=
  else if (op==SyEQU)  printf("==");                // ２項演算子 ==
  else if (op==SyNEQ)  printf("!=");                // ２項演算子 !=
  else if (op==SyASS)  printf("=");                 // ２項演算子 =
  else if (op==SyCOMM) printf(",");                 // ２項演算子 ,
  else if (op==SyOR)   printf("||");                // ２項演算子 ||
  else if (op==SyAND)  printf("&&");                // ２項演算子 &&
  else error("バグ...printOP");                     // 演算子でなかった
}

// 再帰のために宣言が必要
//static void printExp(int node);
static void printExp(int node,boolean func);


// 関数呼び出し時に実引数を印刷する
static void printArgs(int node){
  if (syGetType(node)==SySEMI) {                    // 引数リストなら
    printArgs(syGetLVal(node));                     //   左ノード印刷
    printf(",");                                    //   引数の間に","
    printArgs(syGetRVal(node));                     //   右ノード印刷
  } else {                                          // 引数なら
    //printExp(node);                               // 引数式印刷
    printExp(node,true);                            // 改定　引数式印刷(関数呼び出し時のみfuncをtrue)
  }
}





/*
// 式を印刷する
static void printExp(int node){
    int typ = syGetType(node);
    int lVal = syGetLVal(node);
    int rVal = syGetRVal(node);
    if (typ==SyCNST) {                              // 定数なら
        printf("%d", syGetLVal(node));              //   "値"
    } else if (typ==SySTR) {                        // 文字列なら
        printStrLab(lVal);                          //   "文字列ラベル"
    } else if (typ==SyLOC) {                        // ローカル変数なら
        printLocVar(lVal);                          //   "ローカル変数名"
    } else if (typ==SyPRM) {                        // 仮引数なら
        printParam(lVal);                           //   "仮引数名"
    } else if (typ==SyGLB) {                        // グローバル変数なら
        printf("%s", ntGetName(lVal));              //   "変数名"
    } else if (typ==SyADDR) {                       // addrofなら
        printf("((int)&%s)", ntGetName(lVal));      //   "((int)(&変数))"
    } else if (typ==SyFUNC) {                       // 関数なら
        printf("%s(", ntGetName(lVal));             //   "関数名([実引数,...])"
        if (rVal!=SyNULL) printArgs(rVal);          //
        printf(")");                                //
    } else if (typ==SyIDXW || typ==SyIDXB) {        // [] なら
        printExp(lVal);                             //   "左辺式[右辺式]"
        printf("[");                                //
        printExp(rVal);                             //
        printf("]");                                //
    } else if (typ==SyDOT) {                        // . なら
        printExp(lVal);                             //   "左辺式->フィールド名"
        printf("->");                               //
        printf("%s", ntGetName(syGetLVal(rVal)));   //
    } else if (typ==SyCHR) {                        // chr() なら
        printf("(0xff&");                           //   念のため0xffでマスク
        printExp(lVal);                             //   "(0xff & 式)"
        printf(")");                                //
    } else if (typ==SyBOOL) {                       // bool() なら
        printf("(0x01&");                           //   念のため1でマスク
        printExp(lVal);                             //   "(0x01 & 式)"
        printf(")");                                //
    } else if (typ==SySIZE) {                       // sizeof() なら
        if (lVal<=0) rVal = rVal - 1;               //   参照型は*を１つ減らす
        printf("sizeof(");                          //
        printType(false, lVal, rVal);               //   "sizeof(型)"
        printf(")");                                //
    } else if (SyIS1OPR(typ)) {                     // 後置以外の単項演算なら
        printf("(");                                //   "(演算子 式)"
        printOP(typ);                               //
        printExp(lVal);                             //
        printf(")");                                //
    } else {                                        // それ以外なら２項演算
        printf("(");                                //   "(左辺 演算子 右辺)"
        printExp(lVal);                             //
        printOP(typ);                               //
        printExp(rVal);                             //
        printf(")");                                //
    }
}
 */

//cnttがfalseの時、配列は一次元配列なので型変換を行わない
//四次元のchar型の配列アクセス時の型変換は((CA *)((RA*)((RA*)(z->a[0]))->a[1])->a[1])->a[1]
//一番内側だけ一個括弧が多い
//関数を使ってもっとわかりやすく書くようにする

//cntiがfalseの時一番内側のカッコなのでひとつ多く閉じる
static boolean cnti=false;
//cnttがfalseの時、配列は一次元配列なので型変換を行わない
static boolean cntt=false;
//閉じる括弧の数
static int idim2=0;

// 改定 式を印刷する
static void printExp(int node,boolean func){
  int typ = syGetType(node);
  int lVal = syGetLVal(node);
  int rVal = syGetRVal(node);
  int idim=ntGetDim(lVal)-2;                        //(次元の数-2)だけRAへの型変換を行う
    
  if (typ==SyCNST) {                                // 定数なら
      printf("%d", syGetLVal(node));                //   "値"
  } else if (typ==SySTR) {                          // 文字列なら
      printStrLab(lVal,true,func);                  //   "文字列ラベル"
  } else if (typ==SyLOC) {                          // ローカル変数なら
      printLocVar(lVal);                            //   "ローカル変数名"
  } else if (typ==SyPRM) {                          // 仮引数なら
      printParam(lVal);                             //   "仮引数名"
  } else if (typ==SyGLB) {                          // グローバル変数なら
        /* 改定前
         printf("%s", ntGetName(lVal));             //   "変数名"
         */
        // 改定　型変換
        
      int typ2 = ntGetType(lVal);
      if(idim>=0){                                  //二次元以上の場合
      printArrayTypePoint(typ2);                    //配列アクセス時の型変換を印刷
          cntt=true;
      }
      else{                                         //一次元配列の時は型変換を行わない
          cntt=false;
          cnti=true;
      }
      idim2=idim+1;                                 //idim+1の数だけか括弧を閉じる
      while(idim>0){                                //(次元の数-2)だけRAへの型変換を行う
          printf("(RA*)(");
          idim=idim-1;
      }
      printf("%s", ntGetName(lVal));                //   "変数名"
  } else if (typ==SyADDR) {                         // addrofなら
      printf("((int)&%s)", ntGetName(lVal));        //   "((int)(&変数))"
  } else if (typ==SyFUNC) {                         // 関数なら
      printf("%s(", ntGetName(lVal));               //   "関数名([実引数,...])"
      if (rVal!=SyNULL) printArgs(rVal);            //
      printf(")");                                  //
  } else if (typ==SyIDXW || typ==SyIDXB) {          // [] なら
        /* 改定前
         printExp(lVal);                            //  "左辺式[右辺式]"
         printf("[");                               //
         printExp(rVal);                            //
         printf("]");                               //
         */
        //改定　型変換
      printExp(lVal,func);                          //  "左辺式[右辺式]"
      printf("->a[");                               // "->a[
      printExp(rVal,func);                          //
      printf("]");                                  // "]"
        
        //cntiがfalseの時一番内側のカッコなのでいひとつ多く閉じる
        //cnttがfalseの時、配列は一次元配列なので型変換を行わない
        //四次元のchar型の配列アクセス時の型変換は((CA *)((RA*)((RA*)(z->a[0]))->a[1])->a[1])->a[1]
        //一番内側だけ一個括弧が多い
      if(cnti==false){                              //一番内側の括弧を閉じる
          printf(")");
          cnti=true;
      }
        //printf("%d ",idim2);
      if(cntt==true && idim2>0){                    //括弧を閉じる
          printf(")");
          idim2=idim2-1;
      }
      else if(idim2<=0){                            //型変換が完了した後
          cnti=false;                               //次の型変換のためにcntiを初期化
      }
        
        
  } else if (typ==SyDOT) {                          // . なら
      printExp(lVal,func);                          //   "左辺式->フィールド名"
      printf("->");                                 //
      printf("%s", ntGetName(syGetLVal(rVal)));     //
  } else if (typ==SyCHR) {                          // chr() なら
      printf("(0xff&");                             //   念のため0xffでマスク
      printExp(lVal,func);                          //   "(0xff & 式)"
      printf(")");                                  //
  } else if (typ==SyBOOL) {                         // bool() なら
      printf("(0x01&");                             //   念のため1でマスク
      printExp(lVal,func);                          //   "(0x01 & 式)"
      printf(")");                                  //
  } else if (typ==SySIZE) {                         // sizeof() なら
      if (lVal<=0) rVal = rVal - 1;                 //   参照型は*を１つ減らす
      printf("sizeof(");                            //
        //printType(false, lVal, rVal);             // "sizeof(型)"
      printType(false, lVal, rVal, false,false);    //改定
      printf(")");                                  //
  } else if (SyIS1OPR(typ)) {                       // 後置以外の単項演算なら
      printf("(");                                  //   "(演算子 式)"
      printOP(typ);                                 //
      printExp(lVal,func);                          //
      printf(")");                                  //
  } else {                                          // それ以外なら２項演算
      printf("(");                                  //   "(左辺 演算子 右辺)"
      printExp(lVal,func);                          //
      printOP(typ);                                 //
      printExp(rVal,func);                          //
      printf(")");                                  //
      cnti=false;                                   // 改定次の型変換のためにcntiを初期化
        
  }
}



// if文を印刷する
static void printIf(int node){
  printf("{if(");                                   // {if(
　//printExp(node);                                 // 条件式
  printExp(syGetLVal(node),false);                  // 改定 条件式
  printf(")");                                      // )
  traceTree(syGetRVal(node));                       // 文
  printf("}");                                      // }
}

// if-else文を印刷する
static void printEls(int node){
  int nodeIf = syGetLVal(node);
  printf("if(");                                    // if(
　//printExp(node);                                 // 条件式
  printExp(syGetLVal(nodeIf),false);                // 改定 条件式
  printf(")");                                      // )
  traceTree(syGetRVal(nodeIf));                     // 文
  printf("else ");                                  // else
  traceTree(syGetRVal(node));                       // 文
}

// while文（for文も含む）を印刷する
static void printWhl(int node){
  int lVal = syGetLVal(node);                       // 条件式
  int rVal = syGetRVal(node);                       // 本体と 再初期化式
  printf("for(;");                                  // "for(;"
  // 条件式
  //if (lVal!=SyNULL) printExp(lVal);               // 条件式があれば "条件式"
  if (lVal!=SyNULL) printExp(lVal,false);           // 改定 条件式があれば "条件式"
  printf(";");                                      // ";"
  // 再初期化式
  int rIni = syGetRVal(rVal);                       // 再初期化式があれば
  //if (rIni!=SyNULL) printExp(rIni,false);         // "再初期化式"
  if (rIni!=SyNULL) printExp(rIni,false);           // 改定 "再初期化式"
  printf(")\n");                                    // ")"
  // 本体
  traceTree(syGetLVal(rVal));                       // "本体"
}

// do-while文を印刷する
static void printDo(int node){
  int lVal = syGetLVal(node);                       // 本体
  int rVal = syGetRVal(node);                       // 条件式
  printf("do");                                     // "do"
  traceTree(lVal);                                  // "本体"
  printf("while(");                                 // "while ("
　//printExp(lVal);                                 // "条件式"
  printExp(rVal,false);                             // 改定 "条件式"
  printf(");\n");                                   // ");"
}

// break文を印刷する
static void printBrk(int node){
  printf("break;\n");                               // "break;"
}

// continue文を印刷する
static void printCnt(int node){
  printf("continue;\n");                            // "continue;"
}

// return文を印刷する
static void printRet(int node){
  int lVal = syGetLVal(node);                       // 式
  printf("return ");                                // "return "
  //if (lVal!=SyNULL) printExp(lVal);               // 式があれば "式"
  if (lVal!=SyNULL) printExp(lVal,false);           // 改定 式があれば "式"
  printf(";\n");                                    // ";"
}

// ローカル変数宣言を印刷する(変数名は番号から作る)
static void printVAR(int node){  
  int rVal = syGetRVal(node);
  int typ  = syGetLVal(rVal);                       // 型
  int dim  = syGetRVal(rVal);                       // 次元
  //printType(false, typ, dim);                     // "型名[*...]"
  printType(false, typ, dim, false,false);          //改定
  printLocVar(syGetLVal(node));                     // "_cmm_%dL"
  printf(";\n");                                    // ";"
}

// ブロックを印刷する
static void printBLK(int node){
  printf("{\n");                                    // "{"
  traceTree(syGetLVal(node));                       // 先に左側をコード生成
  traceTree(syGetRVal(node));                       // 次に右側をコード生成
  printf("}\n");                                    // "}"
}

// 任意の部分木を印刷する
static void traceTree(int node){
  if (node==SyNULL) { printf(";\n"); return; }      // 何も無い ";"
  int typ = syGetType(node);
  if      (typ==SyCNST) printf(";\n");              // 最適化で消えた文
  else if (typ==SyIF)   printIf(node);              // if 文
  else if (typ==SyELS)  printEls(node);             // if-else 文
  else if (typ==SyWHL)  printWhl(node);             // while 文
  else if (typ==SyDO)   printDo(node);              // do-while 文
  else if (typ==SyBRK)  printBrk(node);             // break 文
  else if (typ==SyCNT)  printCnt(node);             // continue 文
  else if (typ==SyRET)  printRet(node);             // return 文
  else if (typ==SyVAR)  printVAR(node);             // ローカル変数宣言
  else if (typ==SyBLK)  printBLK(node);             // ブロック
  else if (typ==SySEMI) {                           // セミコロン
    traceTree(syGetLVal(node));                     // 先に左側をコード生成
    traceTree(syGetRVal(node));                     // 次に右側をコード生成
  } else {                                          // 式文
    //printExp(lVal);                               // "式;"
    printExp(node,false);                           // 改定"式;"
    printf(";\n");
  }
}

// 関数宣言を印刷する
static void printFuncDcl(int idx) {
//printGlobDcl(idx);                                // "[static]型名[*...]名前"
  printGlobDcl(idx,false,false);                    // "[static]型名[*...]名前"
  printf("(");                                      // "("
  int cnt = ntGetCnt(idx);                          // 仮引数の数
  for (int i=1; i<=cnt; i=i+1) {
    int typ = ntGetType(idx+i);                     // 型
    int dim = ntGetDim(idx+i);                      // 次元
    printType(false, typ, dim, false,false);                     // "型名[*...]"
    if (typ!=TyDOTDOTDOT) printParam(i);            // "仮引数名”
    if (i<cnt) printf(",");                         // ","
  }
  printf(")");                                      // ")"
}

// プロトタイプ宣言を印刷する
void genProto(int idx) {                            // 引数は名前表の添字
  if (inhibitOut) return;                           // 出力抑制中
  printFuncDcl(idx);                                // "[static]型名[*...]名前"
  printf(";\n");                                    // ";"
}

// 関数を印刷する
void genFunc(int idx, boolean prot, boolean kernFlg) {
  if (inhibitOut) return;                           // 出力抑制中
  printFuncDcl(idx);                                // "[static]型名[*...]名前"
  printf("{\n");                                    // "{"
  int root = syGetRoot();
  if (root!=SyNULL && syGetType(root)==SyBLK)       // 必要以上に"{ }"を
    sySetType(root, SySEMI);                        //   出力しないように
  traceTree(root);                                  // "関数本体"
  printf("}\n");                                    // "}"
}

// 配列初期化用の仮ラベルを割当てる
static int tmpLab = 0;
static int newTmpLab(){
  int l = tmpLab;
  tmpLab = tmpLab + 1;
  return l;
}

/* // 中間のポインタ配列を印刷する
static void printPtrArray(int vType, int dim, int cnt, int lab) {
  printType(true, vType, dim);                      // "static 型名[*...]"
  printTmpLab(newTmpLab());                         // "_cmm_%dT"
  printf("[]={");                                   // "[]={"
  printTmpLab(lab);                                 // "_cmm_%dT"
  for (int j=1; j<cnt; j=j+1) {                     // 要素数の数だけ
    printf(",");                                    //  ",_cmm_%dT"  
    printTmpLab(lab+j);                             //
  }
  printf("};\n");                                   // "};"
}
 */

///////////////
// 改定　中間のポインタ配列を印刷する
static void printPtrArray(int vType, int dim, int cnt, int lab) {
  printf("static ");                                // "[static ]"
  printf("RA ");                                    // "型名"
  printTmpLab(newTmpLab());                         // "_cmm_%dT"
  printf("=");                                      // "="
  printcurly(cnt,lab,false);                        //
  /*
  printf("{");
  printf("%d,{",cnt);                               // "配列サイズ,{"
  printf("&");                                      // "&"
  printTmpLab(lab);                                 // "_cmm_%dT"
  for (int j=1; j<cnt; j=j+1) {                     // 要素数の数だけ続ける
      printf(",");                                  //  ","
      printf("&");                                  // "&"
      printTmpLab(lab+j);                           // _cmm_%dT
  }
  printf("}};\n");                                  // "}};"
      */
}

// printArray0 : "array(n1, n2, ... )" に対応する部分を印刷する
//   戻り値 : この次元のインスタンスの仮変数名の開始番号
//   vType : 初期化している変数の型
//   dim   : いま着目しているのは第何次元か
//   node  : この次元の要素数を示すSyCNSTか、SyCNSTが右にあるSySEMI
//   cnt   : この次元で必要なインスタンスの数
/* static int printArray0(int vType, int dim, int node, int cnt) {
    int typ  = syGetType(node);
    int rVal = syGetRVal(node);
    int lVal = syGetLVal(node);
    int l = tmpLab;
    if (typ==SySEMI) {                              // 最後の次元でない時
      if (syGetType(rVal)!=SyCNST)                  // 定数でなければエラー
      error("バグ...printArray0_1");
    int size = syGetLVal(rVal);                     // 次の次元の要素数
    // 次の次元の配列を生成
    int ln = printArray0(vType, dim-1, lVal, cnt*size);
    l = tmpLab;
    for (int i=0; i<cnt; i++) {                     // 前の次元の要素数分
      printPtrArray(vType, dim, size, ln);          // 中間のポインタ配列
      ln = ln + size;
    }
  } else if (typ==SyCNST) {                         // 最後の次元の時
    for (int i=0; i<cnt; i++) {                     // 前の次元の要素数分
      printType(true, vType, dim);                  // "static 型名[*...]"
      printTmpLab(newTmpLab());                     // "_cmm_%dT"
      printf("[%d];\n", syGetLVal(node));           // "[%d];"
    }
  } else error("バグ...printArray0_2");             // それ以外ならバグ
  return l;
}
 */


// 改定　printArray0 : "array(n1, n2, ... )" に対応する部分を印刷する
static int printArray0(int vType, int dim, int node, int cnt) {
  int typ  = syGetType(node);
  int rVal = syGetRVal(node);
  int lVal = syGetLVal(node);
  int l = tmpLab;
  if (typ==SySEMI) {                                // 最後の次元でない時
      if (syGetType(rVal)!=SyCNST)                  // 定数でなければエラー
          error("バグ...printArray0_1");
      int size = syGetLVal(rVal);                   // 次の次元の要素数
      // 次の次元の配列を生成
      int ln = printArray0(vType, dim-1, lVal, cnt*size);
      l = tmpLab;
      for (int i=0; i<cnt; i++) {                   // 前の次元の要素数分
          printPtrArray(vType, dim, size, ln);      // 中間のポインタ配列
          ln = ln + size;
      }
  } else if (typ==SyCNST) {                         // 最後の次元の時
      for (int i=0; i<cnt; i++) {                   // 前の次元の要素数分
          printType(true,vType,dim,true,false);     // "static 型名[*...]"
          printTmpLab(newTmpLab());                 // "_cmm_%dT"
          printf("=");                              // "="
            
          /*
          printf("{%d,",lVal);                      //"{配列サイズ,"
          printf("{");                              // "{"
          printf("0");                              // "0で配列を初期化"
          for (int j=1; j<lVal; j=j+1) {            // 要素数の数だけ続ける
              printf(",");                          //  ","
              printf("0");                          // 配列の初期化
          }
          printf("}};\n");                          // "}};\n"
          */
          printcurly(lVal,0,true);
      }
  } else error("バグ...printArray0_2");             // それ以外ならバグ
  return l;
}

/* // 非初期化配列を印刷する
static void printArray(int node, int idx){
  int lVal = syGetLVal(node);                       // SyARRAY の左辺
  int typ = ntGetType(idx);                         // 配列のデータ型
  int dim = ntGetDim(idx) - 1;                      // インスタンスの次元数
  int ln = printArray0(typ, dim, lVal, 1);          // 配列インスタンス出力
  printGlobDcl(idx);                                // "[static]型名[*...]名前"
  printf("=");                                      // "="
  printTmpLab(ln);                                  // "_cmm_%dT;
  printf(";\n");
}
 */

// 改定　非初期化配列を印刷する
static void printArray(int node, int idx){

  int lVal = syGetLVal(node);                       // SyARRAY の左辺
  int typ = ntGetType(idx);                         // 配列のデータ型
  int dim = ntGetDim(idx) - 1;                      // インスタンスの次元数
  int ln = printArray0(typ, dim, lVal, 1);          // 配列インスタンス出力
  printGlobDcl(idx,true,true);                      //"[static]型名[*...]名前"
  printf("=");                                      //"="
  printf("&");                                      //"&"
  printTmpLab(ln);                                  // "_cmm_%dT;
  printf(";\n");                                    //";\n"
}

// 初期値のリストを印刷する
static void printList2(int node, int dim) {
  int typ = syGetType(node);                        // ノードの種類
  int lVal = syGetLVal(node);                       // 左辺
  int rVal = syGetRVal(node);                       // 右辺
  if (typ==SySEMI) {
    printList2(lVal, dim);                          // 左辺を処理
    printf(",");                                    // ","
    printList2(rVal, dim);                          // 右辺を処理
  } else if (typ==SyLIST || typ==SyARRY) {
    //if (dim<=0)                                   //改定　配列を構造体で表現しているためif文は不要
    printf("&");                                    // "&" (構造体の初期化)
    printTmpLab(rVal);                              // "_cmm_%dT"
  } else if (typ==SyCNST) {
    printf("%d", lVal);                             // "数値"
  } else if (typ==SySTR) {                          // 文字列なら
    printStrLab(lVal,true,false);                   // "文字列ラベル"
  } else error("バグ...printList2");
}
 



static int listsize=0;                              //Listの配列のサイズ
static int listptrsize=0;                           //Listの中間ポインタ配列のサイズ

//  改定　リストのサイズをゲット
static void printListsize(int node, int dim) {
  int typ = syGetType(node);                        // ノードの種類
  int lVal = syGetLVal(node);                       // 左辺
  int rVal = syGetRVal(node);                       // 右辺
  if (typ==SySEMI) {
      printListsize(lVal,dim);                      // 左辺を処理
      printListsize(rVal,dim);                      //右辺を処理
  } else if (typ==SyLIST || typ==SyARRY) {
      listptrsize=listptrsize+1;                    //中間ポインタ配列のサイズゲット
  } else if (typ==SyCNST) {
      listsize=listsize+1;                          // "配列のサイズゲット"
    
  } else if (typ==SySTR) {                          // 文字列なら
      listptrsize=listptrsize+1;
  //  printStrLab(lVal);                            //   "文字列ラベル"
     
  } else error("バグ...printList2");
}

// SyLIST の内部の各 SyLIST について printList0() を実行
static void printList0(int vType, int dim, int node);
static void printList1(int vType, int dim, int node) {
  int typ = syGetType(node);                        // ノードの種類
  int lVal = syGetLVal(node);                       // 左辺
  int rVal = syGetRVal(node);                       // 右辺
  if (typ==SySEMI) {                                // リストの一部なら
    printList1(vType, dim, lVal);                   //   左辺の内部を探す
    printList1(vType, dim, rVal);                   //   右辺の内部を探す
  } else if (typ==SyLIST) {                         // 内部の SyLIST なら
    printList0(vType, dim, node);                   //   printList0() を実行
  } else if (typ==SyARRY) {                         // 内部の SyARRY なら
    int ln = printArray0(vType, dim, lVal, 1);      //   printArray0() を実行
    sySetRVal(node, ln);                            // SyARRAY にラベル番号記録
} else if (typ!=SyCNST && typ!=SySTR)
    error("バグ...printList1");
}

/*// 初期化データを印刷する
static void printList0(int vType, int dim, int node) {
  printList1(vType, dim-1, syGetLVal(node));        // 配下の各 SyLIST を処理
  int lab = newTmpLab();                            // このノードの名前を決定
  sySetRVal(node, lab);                             // 名前を記録
  printType(true, vType, dim);                      // "static 型[*...]"
  printTmpLab(lab);                                 // "_cmm_%dT"
  if (dim>=0) printf("[]");                         // "[]" (配列の初期化)
  printf("={");                                     // "={"
  printList2(syGetLVal(node), dim);                 // "1,2..."or"_cmm_%dT..."
  printf("};\n");                                   // "};"
}
*/


//改定 初期化データを印刷する
static void printList0(int vType, int dim, int node) {
  printList1(vType, dim-1, syGetLVal(node));        // 配下の各 SyLIST を処理
  int lab = newTmpLab();                            // このノードの名前を決定
  sySetRVal(node, lab);                             // 名前を記録
  printType(true,vType,dim,true,false);
  printTmpLab(lab);                                 // "_cmm_%dT"
  printf("=");                                      //"={"
  printListsize(syGetLVal(node), dim);              // 配列サイズをゲット
  printf("{");
  if(listptrsize==0){                               //Listの配列のサイズを出力
      printf("%d,",listsize);
  }
  else if(listsize==0) {                            //Listの中間ポインタ配列のサイズを出力
      printf("%d,",listptrsize);
  }
  listptrsize=0;                                    //次の配列のために初期化する
  listsize=0;                                       //次の配列のために初期化する
  printf("{");                                      //"{"
  printList2(syGetLVal(node), dim);                 // "1,2..."or"_cmm_%dT..."
  printf("}};\n");                                  // "}};"
     
    /*
    if(listptrsize==0){                             //Listの配列のサイズを出力
        printcurly(listsize,,false);
    }
    else if(listsize==0) {                          //Listの中間ポインタ配列のサイズを出力
        printcurly(listptrsize,,false)
    }
     */
}

/*
 // 初期化リスト({e1, e2, ...})を印刷する
static void printList(int node, int idx) {
  int typ = ntGetType(idx);                         // 配列のデータ型
  int dim = ntGetDim(idx);                          // インスタンスの次元数
  printList0(typ, dim-1, node);                     // リストを出力する
  printGlobDcl(idx);                                // "[static]型名[*...]名前"
  printf("=");                                      // "="
  if (dim<=0) printf("&");                          // "&" (構造体の初期化)
  printTmpLab(syGetRVal(node));                     // "_cmm_%dT"(木に記録あり)
  printf(";\n");                                    // ";"
}
 
 */



// 改定 初期化リスト({e1, e2, ...})を印刷する
static void printList(int node, int idx) {
  int typ = ntGetType(idx);                         // 配列のデータ型
  int dim = ntGetDim(idx);                          // インスタンスの次元数
  printList0(typ, dim-1, node);                     // リストを出力する
  printGlobDcl(idx,true,true);                      //"[static]型名[*...]名前"
  printf("=");                                      // "="
  printf("&");                                      //"&"
  printTmpLab(syGetRVal(node));                     // "_cmm_%dT;
  printf(";\n");                                    //"\n"
}
// 初期化データを印刷する
void genData(int idx){                              // 引数は名前表の添字
  if (inhibitOut) return;                           // 出力抑制中
  int root = syGetRoot();                           // 構文木の先頭取得
  if (root==SyNULL) error("バグ...genData");        // 構文木が必ず必要
  int typ = syGetType(root);                        // ノードの種類を取得
  if (typ==SyARRY) {                                // v=array(n1,...) の場合
    printArray(root, idx);                          //   非初期化配列出力
  } else if (typ==SyLIST) {                         // v = {e1,e2,...} の場合
    printList(root, idx);                           //   配列等の初期化を出力
  } else {                                          // v = 式 の場合
    //printGlobDcl(idx);                            //  "[static]型名[*...]名前
    printGlobDcl(idx ,true,true);                   //  "[static]型名[*...]名前
    printf("=");                                    //  "="
    //printExp(lVal);                               // "式"
    printExp(root,false);                           // 改定 "式"
    printf(";\n");                                  //  ";"
  }
}

// 非初期化変数を印刷する
void genBss(int idx){                               // 引数は名前表の添字
  if (inhibitOut) return;                           // 出力抑制中
  //printGlobDcl(idx);                              // "[static]型名[*...]名前"
  printGlobDcl(idx,false,false);                    // "[static]型名[*...]名前"
  printf(";\n");                                    // ";"
}

// 文字列ラベルを割当てる
static int strLab = 0;
static int newStrLab(){
  int l = strLab;
  strLab = strLab + 1;
  return l;
}

/*
// 文字列を印刷する
int genStr(char *str){
  if (inhibitOut) return 0;                         // 出力抑制中
  int lab = newStrLab();                            // ラベルを割り付ける
  printf("#define ");
  printStrLab(lab);                                 // 文字列ラベルを印刷
  printf(" \"%s\"\n", str);                         // 文字列を出力
  return lab;                                       // ラベル番号を返す
}
 */

// 改定 文字列を印刷する
int genStr(char *str){
  int i=0;
  int cntj=0;
  if (inhibitOut) return 0;                         // 出力抑制中
  int lab = newStrLab();                            // ラベルを割り付ける
  for(i=0;*str !='\0';str++){                       //文字列のサイズをゲット
          cntj=cntj+1;
  }
  str=str-cntj;                                     //ポインタの位置を戻す
  printType(true,TyCHAR,0,true,false);              // "static 型名[*...]"
  printStrLab(lab,false,false);                     // 文字列ラベルを印刷
  printf("={");                                     // "={"
  printf("%d,{",cntj);                              //配列のサイズ
  printf("\"%s\"", str);                            // 文字列を出力
  printf("}};\n");                                  //"}};\n"
  return lab;                                       // ラベル番号を返す
}


// 構造体宣言を印刷する
void genStruc(int idx){
  if (inhibitOut) return;                           // 出力抑制中
  int last = ntGetCnt(idx) + idx;                   // 構造体要素の最終位置
  printf("struct %s {\n", ntGetName(idx));          // "struct 名前 {"
  for(int i=idx+1; i<=last; i++){                   // 各構造体メンバについて
    int typ = ntGetType(i);                         // 型
    int dim = ntGetDim(i);                          // 次元
    char *name = ntGetName(i);                      // 名前
    //printType(false, typ, dim);                   // "型名[*...]"
    printType(false,typ, dim, false,false);         // "型名[*...]"
    printf("%s;\n", name);                          // "名前;"
  }
  printf("};\n");                                   // "};"
}

// インクルードディレクティブを印刷して以降の出力を禁止する
// (# 行番号 "path")ディレクティブの処理
#define  CMMINC "/cmmInclude/"               // C--用システムヘッダの目印
 
void genDirect(int ln, char *fname) {        // # 行番号 "ファイル名" の処理
  if (strstr(fname, CMMINC)!=null &&         // システムディレクトリの
      strEndsWith(fname, ".hmm")) {          // ヘッダファイルなら
    if (ln==1) {                             //  そのファイルが初めてのとき
      fname[strlen(fname)-2]='\0';           //    ".hmm" を ".h" に改変し
      fname = strrchr(fname, '/')+1;         //     basename を切り出し
      printf("#include <%s>\n", fname);      //     "#include <ファイル名>"
    }
    inhibitOut = true;                       //  システムヘッダ内部は出力しない
  } else {
    inhibitOut = false;                      // システムヘッダ以外は出力する
  }
}
