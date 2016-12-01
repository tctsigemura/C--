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
 * semantic.c : C--コンパイラの意味解析ルーチン
 *
 * 2016.07.10         :  新規作成
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "namtbl.h"
#include "sytree.h"
#include "semantic.h"

static int funcIdx;
static int locIdx;
static boolean krnFlag;

/*
 * 式の処理
 */
// 式を監視(watch)するデータ構造
struct watch {
  int type;                                    // 式の型
  int dim;                                     // 式が配列を表すとき次元数
  boolean lhs;                                 // Left-Hand-Side Expression
};

// watch を割り当てる
static struct watch* newWatch(void) {
  return ealloc(sizeof(struct watch));
}

// watch に値を設定する
static void setWatch(struct watch *w, int t, int d, int l) {
  w->type = t;
  w->dim  = d;
  w->lhs  = l;

}

// watch の領域を解放する
static void freeWatch(struct watch *w) {
  free(w);                                       // 領域をシステムに返却する
}

// 式(w)の型が type 型の基本型か調べる
static void chkType(struct watch *w, int type) {
  if (w->dim>0)        error("配列型は使用できない");
  if (w->type==TyVOID) error("void型は使用できない");
  if (type==TyBOOL && w->type!=TyBOOL)  error("論理型が必要");
  if (type==TyINT  && w->type!=TyINT )  error("整数型が必要");
  if (type==TyCHAR && w->type!=TyCHAR)  error("文字型が必要");
  if (type!=TyBOOL && type!=TyINT && type!=TyCHAR) error("バグ...chkType");
}

// 式(w)の値を type, dim 型へ代入可能か調べる
static void chkCmpat(struct watch* w, int type, int dim) {
  if (!(w->type==TyVOID && w->dim>0 && dim>0) &&    // void[] はどの [] にも OK
      !(type==TyVOID && dim>0 && w->dim>0) &&
      !(w->type==TyVOID && w->dim==1 && type<=0)&&  // void[] と構造体は OK
      !(type==TyVOID && dim==1 && w->type<=0) &&
      (w->type!=type || w->dim!=dim)){               // それ以外は正確に型が合
    printf("%d, %d", type, w->type);
    error("代入/比較/初期化/引数/returnの型が合わない");// わないと代入できない
  }
}

// プロトタイプ宣言
static struct watch *chkExpr(int node);
static struct watch *chkAsExpr(int node);
static void chkFcSemi(int node, int lastIdx);     // FuncCallのSySEMIを読む

static int prmIdx;

static void chkFcSemi(int node, int lastIdx) {
  struct watch *w;
  if (syGetType(node)==SySEMI) {               // SySEMIならば
    chkFcSemi(syGetLVal(node), lastIdx);       //  左辺もSySEMIかも
    w = chkAsExpr(syGetRVal(node));            //  右辺を代入式解析
  } else {                                     // SySEMIでなければ
    w = chkAsExpr(node);                       //  自身を代入式解析
  }
  if (prmIdx>lastIdx) error("引数が多い");
  if (ntGetType(prmIdx)!=TyDOTDOTDOT) {        // 可変個引数でなければ
    chkCmpat(w, ntGetType(prmIdx),             //  式を引数に代入可能か
                ntGetDim(prmIdx));             //  チェックして 
    prmIdx = prmIdx + 1;                       //  仮引数番号を進める
  } else if (w->type==TyVOID && w->dim==0) {   // 可変個引数でも
    error("void型の関数は引数にできない");     //  void型は使用不可
  }
}

static void chkArgs(int node, int func) {      // 引数の型，次元チェック
  if (node==SyNULL) return;                    // 引数がなければ帰る
  prmIdx = func + 1;                           // 仮引数を指すインデクス
  int lastIdx = func + ntGetCnt(func);         // 最後の仮引数のインデクス
  chkFcSemi(node, lastIdx);                    // 引数を読む
  if (ntGetType(lastIdx)!=TyDOTDOTDOT &&       // 引数最後が'...'でなく
      prmIdx<=lastIdx) error("引数が少ない");  // prmIdxが小さい
  if (prmIdx!=lastIdx+1) error("バグ...chkArgs");
}

// 因子
static struct watch *chkFactor(int node) {
  struct watch *w = newWatch();
  int type = 0;
  int dim = 0;
  boolean lhs = false;
  if(syGetType(node)==SyCNST) {                // 定数ならば
    type = syGetRVal(node);                    //  右値に型が入っている
    dim = 0;                                   //  次元は0
  } else if(syGetType(node)==SyLOC) {          // ローカル変数ならば
    int idx = syGetLVal(node);                 //  左値に通し番号
    type = ntGetType(locIdx + idx);            //  通し番号がオフセット
    dim  = ntGetDim(locIdx + idx);             //  次元を読むときも同様
    lhs  = true;
  } else if(syGetType(node)==SyPRM) {          // 仮引数ならば
    int idx = syGetLVal(node);                 //  左値に通し番号
    type = ntGetType(funcIdx + idx);           //  通し番号がオフセット
    dim  = ntGetDim(funcIdx + idx);            //  次元を読むときも同様
    lhs  = true;
  } else if(syGetType(node)==SyGLB) {          // グローバル変数ならば
    type = ntGetType(syGetLVal(node));         //  名前表から型を読む
    dim  = ntGetDim(syGetLVal(node));          //  名前表から次元を読む
    lhs  = true;
  } else if(syGetType(node)==SyFUNC) {         // 関数コールならば
    int l = syGetLVal(node);
    int r = syGetRVal(node);
    type = ntGetType(l);                       //  名前表から型を読む
    dim  = ntGetDim(l);                        //  名前表から次元を読む
    if (ntGetType(l)==TyINTR)
      error("interrupt型関数は呼び出せない");
    if (r==SyNULL && ntGetCnt(l)!=0)           // 引数が必要なのに ない
      error("引数がない");
    else chkArgs(r, l);                        // 引数をチェック
  } else if(syGetType(node)==SyLABL) {         // 大域ラベルならば
    type = ntGetType(syGetLVal(node));         //  名前表から型を読む
    dim  = ntGetDim(syGetLVal(node));          //    次元を読む
    int s = ntGetScope(syGetLVal(node));       //    スコープを読む
    if (s!=ScPROT && s!=ScFUNC &&              //  関数名でも
        s!=ScCOMM && s!=ScGVAR)                //  大域変数でもなければ
      error("addrof( に大域名が続かない");     //   エラー
  } else if(syGetType(node)==SySTR) {          // 文字列ならば
    type = TyCHAR;                             //  CHAR型配列 
    dim = 1;                                   //  次元は1
  } else if(syGetType(node)==SySIZE) {         // sizeofならば
    type = syGetLVal(node);                    //  左値が型
    dim  = syGetRVal(node);                    //  右値が次元？
    if (type<=0&&ntGetType(-type)==TyREF)      // typedef なら
      error("typedefされた型はsizeofで使用できない");
#ifdef C                                       // トランスレータは sizeof を
                                               //   そのままC言語ソースに出力
#else                                          // コンパイラは sizeof を計算
    int s = NWORD / 8;                         //   INT またはポインタのサイズ
    if (dim==0) {                              //   配列以外で
      if (type<=0)                             //     構造体の場合は
        s = s * ntGetCnt(-type);               //       フィールド数×INTのサイズ
      else if (type==TyCHAR ||
	             type==TyBOOL)                   //     char, boolean なら
        s = NBYTE / 8;                         //       バイトのサイズ
    }
    sySetType(node,SyCNST);                    //   定数
    sySetLVal(node, s);                        //   サイズ
    sySetRVal(node, TyINT);                    //   型はINT
#endif
  } else {                                     // これら以外は渡されないはず
    error("バグ...semantic.c : chkFactor(watch *w);");
  }
  setWatch(w, type, dim, lhs);                 //  型と次元とlhsを返す
  return w;
}

// 単項演算
static struct watch *chkUniExpr(int node) {
  struct watch *w;
  if (syGetType(node)==SyPLS) {                // 単項演算'+'
    w = chkExpr(syGetLVal(node));              //  左辺を式解析
    chkType(w, TyINT);                         //  左辺はINT型
    w->lhs = false;
    int l = syGetLVal(node);                   // 左のノード
    sySetType(node,syGetType(l));              //   自身を左の値に
    sySetLVal(node,syGetLVal(l));              //     書き換えることで
    sySetRVal(node,syGetRVal(l));              //       "SyPLS"を消去
  } else if ((syGetType(node)==SyNEG ) ||
             (syGetType(node)==SyBNOT)) {      // INTに付く演算子
    w = chkExpr(syGetLVal(node));              //  左辺を式解析
    chkType(w, TyINT);                         //  左辺はINT型
    w->lhs = false;
  } else if(syGetType(node)==SyCHAR) {         // CHARを返す演算子
    w = chkExpr(syGetLVal(node));              //  左辺を式解析
    if (w->dim!=0 || w->type!=TyINT)
      error("chr 括弧内が int 以外になっている");
    w->type = TyCHAR;                          //  CHAR型を返す
    w->lhs = false;
  } else if(syGetType(node)==SyBOOL) {         // BOOLを返す演算子
    w = chkExpr(syGetLVal(node));              //  左辺を式解析
    if (w->dim!=0 || w->type!=TyINT)
      error("bool 括弧内が int 以外になっている");
    w->type = TyBOOL;                          //  BOOL型を返す
    w->lhs = false;
  } else if(syGetType(node)==SyORD) {          // INT型を返す演算子
    w = chkExpr(syGetLVal(node));              //  左辺を式解析
    if ((w->dim!=0) || 
        (w->type!=TyBOOL && w->type!=TyCHAR))
      error("ord 括弧内が char, boolean 以外になっている");
    w->type = TyINT;                           //  INT型を返す
    w->lhs = false;
  } else if(syGetType(node)==SyNOT) {          // BOOLに付く演算子
    w = chkExpr(syGetLVal(node));              //  左辺を式解析
    chkType(w, TyBOOL);                        //  左辺はBOOL型
    w->type = TyBOOL;                          //  BOOL型を返す
    w->lhs = false;
  } else {                                     // 単項演算子でないなら
    w = chkFactor(node);                       //  因子解析
  }
  return w;
}


// 大小比較演算
static struct watch *chkCmpExpr(int node) {
  struct watch *w;
  struct watch *l;                             // 左辺式用
  struct watch *r;                             // 右辺式用
  if ((syGetType(node)==SyGT) ||
      (syGetType(node)==SyGE) ||
      (syGetType(node)==SyLT) ||
      (syGetType(node)==SyLE)) {               // 大小比較演算式ならば
    l = chkExpr(syGetLVal(node));              //  左辺を式解析
    r = chkExpr(syGetRVal(node));              //  右辺を式解析
    chkType(l, TyINT);                         //  左右は共にINT型
    chkType(r, TyINT);
    w = newWatch();
    setWatch(w, TyBOOL, 0, false);             // 次元0のBOOL型を返す
  } else {                                     // 比較演算式でないなら
    w = chkUniExpr(node);                      //  単項演算式解析
  }
  return w;
}

// 比較演算
static struct watch *chkEquExpr(int node) {
  struct watch *w;
  struct watch *l;                             // 左辺式用
  struct watch *r;                             // 右辺式用
  if ((syGetType(node)==SyEQU) ||
      (syGetType(node)==SyNEQ)) {              // 比較演算式ならば
    l = chkExpr(syGetLVal(node));              //  左辺を式解析
    r = chkExpr(syGetRVal(node));              //  右辺を式解析
    chkCmpat(r, l->type, l->dim);              //  代入可能ならば比較可能
    w = newWatch();
    setWatch(w, TyBOOL, 0, false);             //  次元0のBOOL型を返す
  } else {                                     // 比較演算式でないなら
    w = chkCmpExpr(node);                      //  大小比較演算式解析
  }
  return w;
}

// 論理積和
static struct watch *chkLogExpr(int node) {
  struct watch *w;
  struct watch *r;                             // 右辺式用
  if (SyISLOPR(syGetType(node))) {             // 論理積和式ならば
    w = chkExpr(syGetLVal(node));              //  左辺を式解析
    r = chkExpr(syGetRVal(node));              //  右辺を式解析
    chkType(w, TyBOOL);                        //  左右共にBOOL型
    chkType(r, TyBOOL);
    w->lhs = false;
  } else {                                     // 論理積和でないなら
    w = chkEquExpr(node);                      //  比較演算式解析
  }
  return w;
}

// 二項演算
static struct watch *chkBiExpr(int node) {
  struct watch *w;                             // 左辺式用
  struct watch *r;                             // 右辺式用
  if (syGetType(node)==SyIDXW) {               // 配列参照ならば
    w = chkExpr(syGetLVal(node));              //  左辺式を解析
    int stype = w->type;
    if (w->dim>0 || stype<=0) stype = SyIDXW;  // 参照はワード配列
    else if (stype==TyINT)    stype = SyIDXW;  // int はワード配列
    else if (stype==TyCHAR)   stype = SyIDXB;  // char はバイト配列
    else if (stype==TyBOOL)   stype = SyIDXB;  // boolean はバイト配列
    else error("バグ...chkBiExpr, IDX判別");
    sySetType(node, stype);                    //  ワードかバイトかを設定
    r = chkExpr(syGetRVal(node));              //  右辺を式解析
    if (r->type!=TyINT || r->dim!=0)           //  右はINT型
      error("配列の添字が整数以外になっている");
    w->dim = w->dim - 1;                       //  次元を1つ下げる
    if(w->dim<0) error("添字が多すぎる");
    w->lhs = true;
  } else if (syGetType(node)==SyDOT) {         // 構造体ならば
    w = chkExpr(syGetLVal(node));              //  左辺式を解析
    if (w->type>0 || w->dim>0)                 //   式(w)が基本型または配列の
      error("構造体以外に'.' がある");         //   場合に'.'は続かない
    int idx = syGetRVal(node);                 //  右値はフィールド名のidx
    if (syGetType(idx)!=SyCNST)
      error("バグ...SyDOT");
    idx = syGetLVal(idx);                      //  SyCNSTの左値がインデクス
    int n = ntSrcField(w->type,ntGetName(idx));//  構造体をフィールド名で検索
    w->type = ntGetType(n);                    // 式(w)をフィールドの型と
    w->dim  = ntGetDim(n);                     //   次元に変更する
    w->lhs  = true;
  } else if (SyIS2OPR(syGetType(node))) {      // それ以外の二項演算式
    w = chkExpr(syGetLVal(node));              //  左辺を式解析
    r = chkExpr(syGetRVal(node));              //  右辺を式解析
    chkType(w, TyINT);                         //  左右共にINT型
    chkType(r, TyINT);
    w->lhs = false;
  } else {                                     // 二項演算式でないなら
    w = chkLogExpr(node);                      //  論理積和演算式解析
  }
  return w;
}

// 代入式
static struct watch *chkAsExpr(int node) {
  struct watch *w;
  struct watch *r;                             // 右辺式用
  if (syGetType(node)==SyASS) {                // 代入式ならば
    w = chkExpr(syGetLVal(node));              //  左辺を式解析
    r = chkExpr(syGetRVal(node));              //  右辺を式解析
    if (!w->lhs) error("左辺が代入可能ではない");
    chkCmpat(r, w->type, w->dim);              //  代入可能かチェック
    w->lhs = false;
  } else {                                     // 代入式でないなら
    w = chkBiExpr(node);                       //  二項演算演算式解析
  }
  return w;
}


// カンマ式
static struct watch *chkExpr(int node) {
  struct watch *w;
  struct watch *r;                             // 右辺式用
  if (syGetType(node)==SyCOMM) {               // カンマ式ならば
    w = chkAsExpr(syGetLVal(node));            //  左辺は代入式として解析
    r = chkExpr(syGetRVal(node));              //  右辺もカンマ式かも
    chkType(r, w->type);                       //  左右は同じ型
    w->lhs = false;
  } else {                                     // カンマ式でないなら
    w = chkAsExpr(node);                       //  代入式として解析
  }
  return w;
}

// 再帰のためのプロトタイプ宣言
static void traceTree(int node);

// if 文
static void chkIf(int node) {
  struct watch* w = chkExpr(syGetLVal(node));     // 条件式の解析
  chkType(w, TyBOOL);                             // 条件式は論理型
  traceTree(syGetRVal(node));                     // 本文の解析
  freeWatch(w);
}

// if-else 文
static void chkEls(int node) {
  int ifNode = syGetLVal(node);                   // if 文部分
  chkIf(ifNode);                                  // if文解析
  traceTree(syGetRVal(node));                     // else 節の本文解析
}

// while 文 と for 文
static void chkWhl(int node) {
  struct watch *w = chkExpr(syGetLVal(node));     // 条件式の解析
  chkType(w, TyBOOL);                             // 条件式は論理型
  int sta = syGetRVal(node);                      // 本文 + 再初期化
  traceTree(syGetLVal(sta));                      // 本文の解析
  if (syGetRVal(sta)!=SyNULL)                     // 再初期化がある場合
    traceTree(syGetRVal(sta));                    //   再初期化を解析
  freeWatch(w);
}

// do-while 文
static void chkDo(int node) {
  struct watch *w = chkExpr(syGetRVal(node));     // 条件式の解析
  chkType(w, TyBOOL);                             // 条件式は論理型
  traceTree(syGetLVal(node));                     // 本文の解析
  freeWatch(w);
}

// break 文
static void chkBrk(int node) {
  ;
}

// continue 文
static void chkCnt(int node) {
  ;
}

// return 文
static void chkRet(int node) {
  if (syGetLVal(node)!=SyNULL) {                  // returnがあれば
    struct watch *w = chkExpr(syGetLVal(node));   //  式の解析
    int t = ntGetType(funcIdx);                   //  関数の型を読む
    int d = ntGetDim(funcIdx);                    //  関数の次元を読む
    chkCmpat(w, t, d);                            //  代入可能かチェック
  }
}

static void chkName(int curType, int curDim, int curScope) {
  if (curScope!=ScPROT && curType==TyVOID && curDim==0)  // 関数以外でvoid型は
    error("void型変数/引数は使用できない");              //   ポインタだけOK
  if (curScope!=ScPROT && curType==TyINTR)               // 関数以外でintrrupt
    error("interrupt型変数/引数は使用できない");         //   型は使用できない
  if (curType==TyINTR && !krnFlag)                       // カーネルのみintrrupt
    error("interrupt型はカーネルのみで使用可");          //   型は使用できる
}

// 構文木をトレースする
static void traceTree(int node) {
  if (node==SyNULL) return;                       // 何も無い
  int ty = syGetType(node);
  if (ty==SyIF) chkIf(node);                      // if 文
  else if (ty==SyELS) chkEls(node);               // if-else 文
  else if (ty==SyWHL) chkWhl(node);               // while 文
  else if (ty==SyDO)  chkDo(node);                // do-while 文
  else if (ty==SyBRK) chkBrk(node);               // break 文
  else if (ty==SyCNT) chkCnt(node);               // continue 文
  else if (ty==SyRET) chkRet(node);               // retrun 文
  else if (ty==SyBLK || ty==SySEMI) {             // ブロック
    traceTree(syGetLVal(node));                   //   先に左側を解析
    traceTree(syGetRVal(node));                   //   次に右側を解析
  } else if (ty==SyVAR) {                         // ローカル変数宣言
    int curType = syGetLVal(syGetRVal(node));
    int curDim  = syGetRVal(syGetRVal(node));
    chkName(curType, curDim, ScLVAR);
  } else {                                        // 式文
    chkExpr(node);                                //  式 解析
  }
}
static int strcIdx;

// null初期化が適切か調べる
static boolean chkNull(int node, int rval, int lval, int dim, int type) {
  if((node==SyCNST) &&
     (rval==TyREF)  &&
     (lval==0)         ) {         // null初期化なら
    if (!(dim!=0||type<=0))        // 構造体or配列のはず
      error("nullの使用が不適切");
    return false;                  // null初期化が適切
  }
  return true;                     // null初期化ではない
}

// ノードの内容を上書きする
static void setNode(int node, int typ, int l, int r) {
  if (node==SyNULL) error("BUG : setNode");
  sySetType(node, typ);
  sySetLVal(node, l);
  sySetRVal(node, r);
}

// この演算子は左辺式を必要とする
static boolean needLeft(int node) {
  int op = syGetType(node);
  return SyIS1OPR(op)|| SyIS2OPR(op) || SyISCMP(op) || SyISLOPR(op);
}

// この演算子は右辺式を必要とする
static boolean needRight(int node) {
  int op = syGetType(node);
  return SyIS2OPR(op) || SyISCMP(op) || SyISLOPR(op);
}

static int setBool(boolean flag) {
  if (flag) return 1;
  else      return 0;
}
// 式の定数を計算する
static int calExp(int node) {
  int ty = syGetType(node);                       // 演算子
  int l  = syGetLVal(node);                       // 左辺
  int r  = syGetRVal(node);                       // 右辺

  // 先に演算の対象を最適化する
  if (needLeft(node))  l = calExp(l);             // 左辺あるので処理する
  if (needRight(node)) r = calExp(r);             // 右辺あるので処理する

  // 演算子に応じた最適化を行う
  if (ty==SyNEG) {
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, -syGetLVal(l), syGetRVal(l));
  } else if (ty==SyNOT) { 
    if(syGetRVal(l)!=TyBOOL) error("論理式が必要");
    setNode(node, SyCNST, setBool(!(syGetLVal(l)==1)), syGetRVal(l));
  } else if (ty==SyBNOT) { 
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, ~syGetLVal(l), syGetRVal(l));
  } else if (ty==SyCHAR) { 
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, syGetLVal(l) & BMSK, TyCHAR);
  } else if (ty==SyBOOL) { 
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, syGetLVal(l) & 1, syGetRVal(l));
  } else if (ty==SySIZE) { 
    /*           ???              */
  } else if (ty==SyADD) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, syGetLVal(l)+syGetLVal(r), syGetRVal(l));
  } else if (ty==SySUB) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, syGetLVal(l)-syGetLVal(r), syGetRVal(l));
  } else if (ty==SySHL) { 
    int n = syGetLVal(r);
    if(n<0 || NWORD<=n) error("<< シフト桁数が範囲外");
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, syGetLVal(l) << n, syGetRVal(l));
  } else if (ty==SySHR) { 
    int n = syGetLVal(r);
    if(n<0 || NWORD<=n) error(">> シフト桁数が範囲外");
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, syGetLVal(l) >> n, syGetRVal(l));
  } else if (ty==SyBAND) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, syGetLVal(l) & syGetLVal(r), syGetRVal(l));
  } else if (ty==SyBOR) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, syGetLVal(l) | syGetLVal(r), syGetRVal(l));
  } else if (ty==SyBXOR) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, syGetLVal(l) ^ syGetLVal(r), syGetRVal(l));
  } else if (ty==SyMUL) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, syGetLVal(l) * syGetLVal(r), syGetRVal(l));
  } else if (ty==SyDIV) {
    if(syGetLVal(r)==0) error("0 での割算");
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, syGetLVal(l) / syGetLVal(r), syGetRVal(l));
  } else if (ty==SyMOD) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, syGetLVal(l) % syGetLVal(r), syGetRVal(l));
  } else if (ty==SyGT) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, setBool(syGetLVal(l) > syGetLVal(r)), TyBOOL);
  } else if (ty==SyGE) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, setBool(syGetLVal(l) >= syGetLVal(r)), TyBOOL);
  } else if (ty==SyLT) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, setBool(syGetLVal(l) < syGetLVal(r)), TyBOOL);
  } else if (ty==SyLE) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("整数式が必要");
    if(syGetRVal(l)!=TyINT) error("整数式が必要");
    setNode(node, SyCNST, setBool(syGetLVal(l) <= syGetLVal(r)), TyBOOL);
  } else if (ty==SyEQU) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("比較する型が合わない");
    setNode(node, SyCNST, setBool(syGetLVal(l) == syGetLVal(r)), TyBOOL);
  } else if (ty==SyNEQ) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("比較する型が合わない");
    setNode(node, SyCNST, setBool(syGetLVal(l) != syGetLVal(r)), TyBOOL);
  } else if (ty==SyOR) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("論理式が必要");
    if(syGetRVal(l)!=TyBOOL) error("論理式が必要");
    setNode(node, SyCNST, setBool((syGetLVal(l)==1) || (syGetLVal(r)==1))
            , TyBOOL);
  } else if (ty==SyAND) { 
    if(syGetRVal(l)!=syGetRVal(r)) error("論理式が必要");
    if(syGetRVal(l)!=TyBOOL) error("論理式が必要");
    setNode(node, SyCNST, setBool((syGetLVal(l)==1) && (syGetLVal(r)==1))
            , TyBOOL);
  } else if (ty==SyCNST || ty==SySTR) {
    ;
  } else error("定数式が必要");                     // それ以外はこない
  // 最適化終了後
  
  if (syGetType(node)==SyCNST)                    // 定数になっていたら
    sySetLVal(node, syGetLVal(node) & WMSK);        //   値のオーバーフローを訂正
                                                  //  (全てのノードは計算前に
                                                  //   ここで訂正を受ける)
  return node;
}
 
static void chkStrc(int node, int maxIdx);
static void chkStrc(int node, int maxIdx) {
  struct watch *w = null;
  if (syGetType(node)==SySEMI) {               // SySEMIならば
    chkStrc(syGetLVal(node), maxIdx);          //  左辺はSySEMIかも
    node = syGetRVal(node);                    //  右辺を代入式解析
    if (syGetType(node)==SyLIST)
      error("構造体初期化式で\"{...}\"が多すぎる");
  } else if (syGetType(node)==SyLIST) {        // SyLISTは来ない
    error("構造体初期化式で\"{...}\"が多すぎる");
  }                                            // SySEMIでなければ
  w = chkAsExpr(node);                         //  自身を代入式解析
  if (strcIdx>maxIdx) error("フィールド数が多い");
  int nType = ntGetType(strcIdx);              //  名前表から型をとる
  int nDim  = ntGetDim(strcIdx);               //    次元をとる
  if (chkNull(syGetType(node), syGetRVal(node),
              syGetLVal(node), nDim, nType)) { // null初期化かチェック
    chkCmpat(w, nType, nDim);                  //  式を引数に代入可能か
    calExp(node);                              //  定数式の計算
  }
  strcIdx = strcIdx + 1;                       //  仮引数番号を進める
}

static void chkLstSemi(int node, int type, int dim);

// SyLISTのSySEMIを読む
static void chkLstSemi(int node, int type, int dim) {
  //struct watch *w;
  if (syGetType(node)==SySEMI) {               // SySEMIならば
    chkLstSemi(syGetLVal(node), type, dim);    //  左辺もSySEMIかも
    chkLstSemi(syGetRVal(node), type, dim);    //  右辺はLISTか式
  } else if (syGetType(node)==SyLIST) {        // SyLISTならば
    if (dim==0 && type<=0) {                   //  構造体かもしれない
      sySetRVal(node, SyNULL);                 //  構造体の右値はSyNULL
      strcIdx = -type + 1;                     //  typeがインデクス
      int maxIdx = ntGetCnt(strcIdx-1);        // フィールド数を読む
      maxIdx = maxIdx + strcIdx - 1;
      chkStrc(syGetLVal(node), maxIdx);
      if (strcIdx-1!=maxIdx)
        error("構造体初期化のフィールド数が少ない");
    }  else {                                  //  構造体じゃなければ
      if (dim<=0) error("配列初期化の次元が多すぎる");
      if (type!=TyCHAR && type!=TyBOOL)        // バイト単位の配列以外
        sySetRVal(node, 0);                    //  右値が0
      chkLstSemi(syGetLVal(node), type, dim-1);//  LIST初期化
    }
  } else {                                     // それ以外ならば
    if (chkNull(syGetType(node), syGetRVal(node),
            syGetLVal(node), dim, type)) {     // null初期化かチェック
      struct watch *w = chkBiExpr(node);       //  式解析
      chkCmpat(w, type, dim);                  //  代入可能かチェック
      calExp(node);                            //  定数式の計算
    }
  }
}

  /*
  if (SyIS2OPR(ty)) {             // 2項演算ならば
    chkCnst(syGetLVal(node));
    chkCnst(syGetRVal(node));
  } else if (SyIS1OPR(ty)) {      // 単項演算ならば
    if (ty==SyNOT || ty==SyCHAR || ty==SyBOOL)
      error("定数式が必要");
    chkCnst(syGetLVal(node));
  } else {
    if (ty!=SyCNST && ty!=SyLABL && ty!=SySTR && ty!=SySIZE)
      error("定数式が必要");
  }
  */

static int chkArry(int node);
// SyARRYのSySEMIを読む
static int chkArry(int node) {
  int cnt;                                     // 式の数を数える
  int n;
  struct watch *w = newWatch();
  if (syGetType(node)==SySEMI) {               // SySEMIならば
    cnt = chkArry(syGetLVal(node)) + 1;        //  式の数をカウント
    w = chkBiExpr(syGetRVal(node));            //  右辺を演算式解析
    n = calExp(syGetRVal(node));           //  定数式かチェック
  } else {                                     // SySEMIでなければ
    cnt = 1;                                   //  1つ目の式
    w = chkBiExpr(node);                       //  自身を演算式解析
    n = calExp(node);                      //  定数式かチェック
  }
  if (syGetLVal(n)<=0)
    error("配列のサイズは正");
  chkType(w, TyINT);                           // INT型かチェック
  return cnt;
}

// 初期化データの意味解析
static void semChkData(int curIdx, int idx) {
  //syDebPrintTree();
  if (idx>=0) {                            // 既に登録されていた場合
    if (ntGetScope(idx)!=ScCOMM)
      error("2重定義");                    // コモン以外は2重定義
    ntSetScope(idx, ScGVAR);               // 初期化済データに変更
    ntUndefName(curIdx);                   // 既に登録されているので削除
  }
  int ltype = ntGetType(curIdx);           // 処理中の変数の型と
  int ldim  = ntGetDim(curIdx);            //   次元を取ってくる
  int node  = syGetRoot();
  chkName(ltype, ldim, ScGVAR);            // 名前チェック
  if (syGetType(node)==SyLIST) {           // LISTなら
    if (ltype!=TyCHAR && ltype!=TyBOOL)    // バイト単位の配列以外
      sySetRVal(node, 0);                  //  右値が0
    chkLstSemi(node, ltype, ldim);
  } else if (syGetType(node)==SyARRY) {    // arrayなら
    int cnt = chkArry(syGetLVal(node));    //  式の個数を調べる
    if (ldim<cnt)
      error("array の次元が配列の次元を超える");
  } else {
    if (syGetType(node)==SySEMI)
      error("バグ...SySEMIは来ないはず, semChkData");
    if (chkNull(syGetType(node), syGetRVal(node),
        syGetLVal(node), ldim, ltype)){    // null初期化かチェック
      struct watch *w = chkBiExpr(node);   //  式として解析
      calExp(node);
      chkCmpat(w, ltype, ldim);            //  代入可能かチェック
    }
  }
  //syDebPrintTree();
}

// 非初期化データの意味解析
static void semChkBss(int curIdx, int idx) {
  if (idx>=0) {
    if (ntGetScope(idx)==ScCOMM || ntGetScope(idx)==ScGVAR)
      ntUndefName(curIdx);                 // 既に登録されている
    else
      error("2重定義(以前は関数)");
  } 
}

/***
 *** 公開関数
 ***/

// 関数の意味解析
void semChkFunc(int node, int fidx, boolean kFlag){
  //syDebPrintTree();                        // ### DEBUG ###
  funcIdx = fidx;                          // 名前表上の関数名の番号
  krnFlag = kFlag;
  int curType = ntGetType(funcIdx);        // 扱う関数の型
  int curDim  = ntGetDim(funcIdx);         //         と次元
  boolean curPub  = ntGetPub(funcIdx);     //         とpubフラグ
  int idx = ntSrcGlob(funcIdx);            // 同じ名前のものはないか
  if (idx>=0 && (ntGetScope(idx)==ScCOMM || ntGetScope(idx)==ScGVAR))
    error("2重定義(以前は変数)");
  if (idx>=0 &&
      (ntGetType(idx)!=curType||ntGetDim(idx)!=curDim||ntGetPub(idx)!=curPub))
    error("関数の型が以前の宣言と異なっている");
  if (curType==TyINTR && curDim!=0)
    error("interrupt型の配列は認められない");
  if (idx>=0) {                            // プロトタイプ宣言があれば
    int curCnt = ntGetCnt(funcIdx);
    if (ntGetCnt(idx)!=curCnt)
      error("引数の数が以前の宣言と異なっている");
    for (int i=1;i<=curCnt;i=i+1) {
      curType = ntGetType(funcIdx+i);      //   引数を比較
      curDim  = ntGetDim(funcIdx+i);
      if (ntGetType(idx+i)!=curType || ntGetDim(idx+i)!=curDim)
        error("引数が以前の宣言と異なる"); 
    }
  }
  if (ntGetType(funcIdx)==TyINTR && ntGetCnt(funcIdx)!=0)
    error("interrupt関数は引数を持てない");
  if (idx>=0 && ntGetScope(idx)!=ScPROT) error("関数の2重定義");
  locIdx  = funcIdx + ntGetCnt(funcIdx);   // 1つ目のローカル変数の1つ前
  traceTree(node);                         // 構文木をトレースする
  int lastNode;                            //  関数最後の文のノード
  if (syGetType(syGetRoot())!=SyBLK)       //  SyBLKでないなら
    lastNode = syGetRoot();            //  そのものが，
  else                                     //  SyBLKならば
    lastNode = syGetRVal(syGetRoot()); //  右の値が，関数最後の文
  if ((ntGetType(funcIdx)!=TyVOID ||       // void型の関数,
	     ntGetDim(funcIdx)!=0)      &&       //
       ntGetType(funcIdx)!=TyINTR &&       // interupt型以外の関数が
       syGetType(lastNode)!=SyRET)         // return文で終わっていない
      error("関数が値を返していない");
}

void semChkGVar(int curIdx) {
  int idx = ntSrcGlob(curIdx);             // 2重定義の可能性があるか？
  if (syGetRoot()!=SyNULL) semChkData(curIdx, idx);
  else semChkBss(curIdx, idx);
  int curType = ntGetType(curIdx);
  int curDim  = ntGetDim(curIdx);
  int pubFlag = ntGetPub(curIdx);
  if (idx>=0) {
    if (ntGetType(idx)!=curType||ntGetDim(idx)!=curDim||ntGetPub(idx)!=pubFlag)
      error("変数の型が以前の宣言と異なっている");
  }
  if (curType==TyVOID && curDim==0)      // void型はエラー
    error("void型変数は使用できない");
  if (curType==TyINTR)                   // interrupt型はエラー
    error("interrupt型変数は使用できない");

}
