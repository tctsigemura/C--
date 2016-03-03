;
; Programing Language C-- "Compiler"
;    Tokuyama kousen Advanced educational Computer
;
; Copyright (C) 2016 by
;                      Dept. of Computer Science and Electronic Engineering,
;                      Tokuyama College of Technology, JAPAN
;
;   上記著作権者は，Free Software Foundation によって公開されている GNU 一般公
; 衆利用許諾契約書バージョン２に記述されている条件を満たす場合に限り，本ソース
; コード(本ソースコードを改変したものを含む．以下同様)を使用・複製・改変・再配
; 布することを無償で許諾する．
;
;   本ソースコードは＊全くの無保証＊で提供されるものである。上記著作権者および
; 関連機関・個人は本ソースコードに関して，その適用可能性も含めて，いかなる保証
; も行わない．また，本ソースコードの利用により直接的または間接的に生じたいかな
; る損害に関しても，その責任を負わない．
;

; lib/crt0.s : ユーザプロセス用スタートアップ
;
; 2016.02.25  新規作成（TacOS の usr/lib/crt0.s をもとに）
;
; $Id$
;

;------------------------------スタートアップルーチン---------------------------
; スタートアップルーチンに入った時点のスタックの状態
; addr SP からの相対 スタックの中身
; 00
;       SP+0         argc                              今, SP はここを指している
;       SP+2         argv
;       SP+4         argv[0](argv[0]の文字列へのポインタ)
;       SP+6         argv[1](argv[1]の文字列へのポインタ)
;       ...          ...
;       SP+?         null
;       SP+?         argv[0]の文字列
;       SP+?         argv[1]の文字列
;       ...          ...
; FF

;プログラムの開始位置（このルーチンは必ずテキストの先頭に配置する)
.start
    call    __stdlibInit        ; stdlibの初期化ルーチンを呼ぶ
    call    __stdioInit         ; stdio の初期化ルーチンを呼ぶ
    call    _main               ; ユーザプログラムのメインに飛ぶ
    push    g0                  ; メインの戻り値をスタックに積む
    call    _exit               ; プログラムの最後まで到達した場合は終了する

;---------------------------ライブラリルーチンの処理---------------------------
; [sp+2]が第1引数、[sp+4]が第2引数

;; アドレスから整数へ変換
__aToI                          ; int _aToI(void[] a);
;; 整数からアドレスへ変換
__iToA                          ; void[] _iToA(int a);         
;; アドレスからアドレスへ変換
__aToA                          ; void[] _aToA(void[] a);
        ld      g0,2,sp
        ret

;; アドレスと整数の加算
__addrAdd                       ; void[] _addrAdd(void[] a, int i);
        ld      g0,2,sp
        add     g0,4,sp
        ret

;; 符号なし整数の比較
__uCmp                          ; int _uCmp(int a, int b);
;; アドレス比較(第1引数の方が大きいとき1, 等しいとき0、小さいとき-1)
__aCmp                          ; int _aCmp(void[] a, void[] b);
        ld      g0,2,sp
        cmp     g0,4,sp
        ld      g0,#1
        jhi     .L1
        ld      g0,#0
        jz      .L1
        ld      g0,#-1
.L1     ret

;; 呼び出した C-- 関数の第1引数のアドレスを返す
__args                          ; void[] _args();
        ld      g0,fp
        add     g0,#4
        ret

;; SP の値を取得する
__sp
        ld      g0,sp
        ret

;; ヒープとスタックの間に 10Byte 以上の余裕があるかチェックする 
__stkChk
        ld      g0,__alcAddr    ; G0 にヒープ領域の最後をロード
        add     g0,#10          ; 10Byte 分の余裕を持たせる
        cmp     g0,sp           ; G0 と G1 を比較 [ヒープの最後+10Byte] - [SP]
        jnc     .stkOverFlow    ; ユーザスタックがオーバーフローしている
        ret
        
; スタックがオーバーフローした場合、 exit(EUSTK) を実行
.stkOverFlow
        ld      g0,#-24         ; パラメータ(lib/syscall.hmm の EUSTK)
        push    g0              ;   をスタックに積む
        call    _exit           ; exit を呼ぶ
