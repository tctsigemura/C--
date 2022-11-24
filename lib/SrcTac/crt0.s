;
; Programing Language C-- "Compiler"
;    Tokuyama kousen Advanced educational Computer
;
; Copyright (C) 2016 - 2019 by
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
; 2019.01.20  in()，out() 追加
; 2019.01.10  エラー番号が変更になった（EUSTK）
; 2018.11.14  32ビット演算を追加
; 2016.02.25  新規作成（TacOS の usr/lib/crt0.s をもとに）
;
; $Id$
;

;------------------------------スタートアップルーチン---------------------------
; スタートアップルーチンに入った時点のスタックの状態
; addr SP からの相対 スタックの中身
; 00
;       SP+0         argc                            今, SP はここを指している
;       SP+2         argv
;       SP+4         envp
;       SP+6         argv[0](argv[0]の文字列へのポインタ)
;       SP+8         argv[1](argv[1]の文字列へのポインタ)
;       ...          ...
;       SP+?         null
;       SP+?         envp[0](envp[0]の文字列へのポインタ)
;       ...          ...
;       SP+?         null
;       SP+?         argv[0]の文字列
;       SP+?         argv[1]の文字列
;       ...          ...
;       SP+?         envp[0]の文字列
;       ...          ...
; FF

;プログラムの開始位置（このルーチンは必ずテキストの先頭に配置する)
.start
    call    __stdlibInit        ; stdlibの初期化ルーチンを呼ぶ
    call    __stdioInit         ; stdio の初期化ルーチンを呼ぶ
    ld      g0,4,sp             ; envp を
    push    g0                  ;   スタックに積む
    call    __environInit       ; environ の初期化ルーチンを呼ぶ
    pop     g0                  ; スタックを戻す
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

;; 32ビット加算ルーチン
__add32                     ; int[] _add32(int[] dst, int[] src);
        ld      g0,2,sp     ; ディスティネーション(アドレス)
        ld      g1,4,sp     ; ソース(アドレス)
        ld      g2,2,g0     ; ディスティネーション下位ワード
        add     g2,2,g1     ; ソース下位ワード
        st      g2,2,g0     ; ディスティネーション下位ワード
        ld      g2,0,g0     ; ディスティネーション上位ワード
        jnc     .L2
        add     g2,#1       ; キャリーがあった場合は +1 する
.L2
        add     g2,0,g1     ; ソース上位ワード
        st      g2,0,g0     ; ディスティネーション上位ワード
        ret

;; 32ビット減算ルーチン
__sub32                     ; int[] _sub32(int[] dst, int[] src);
        ld      g0,2,sp     ; ディスティネーション(アドレス)
        ld      g1,4,sp     ; ソース(アドレス)
        ld      g2,2,g0     ; ディスティネーション下位ワード
        sub     g2,2,g1     ; ソース下位ワード
        st      g2,2,g0     ; ディスティネーション下位ワード
        ld      g2,0,g0     ; ディスティネーション上位ワード
        jnc     .L3
        sub     g2,#1       ; ボローがあった場合は -1 する
.L3
        sub     g2,0,g1     ; ソース上位ワード
        st      g2,0,g0     ; ディスティネーション上位ワード
        ret

;; 32ビット掛け算ルーチン
__mul32                     ; int[] _mul32(int[] dst, int src)
        push    g3
        ld      g0,4,sp     ; ディスティネーション(アドレス)
        ld      g1,2,g0     ; ディスティネーション下位ワード(B)
        ld      g2,#0       ; (g1,g2) <= (B,0)
        ld      g3,#16      ; カウンタ
.A1     ld      g0,#0       ; g0をとりあえず0にする
        shll    g1,#1       ; g1 <<= 1
        jnc     .A2         ; g1の最上位が1だったなら
        ld      g0,6,sp     ;  g0にソースをロード
.A2     shll    g2,#1       ; g2 <<= 1
        jnc     .A3         ; キャリーがあったら
        add     g1,#1       ;  g1 += 1
.A3     add     g2,g0       ; g2 += g0
        jnc     .A4         ; キャリーがあったら
        add     g1,#1       ;  g1 += 1
.A4     sub     g3,#1       ; 16回繰り返したか
        jnz     .A1
        ld      g0,4,sp     ; ディスティネーション(アドレス)
        st      g1,0,g0     ; ディスティネーション上位ワード
        st      g2,2,g0     ; ディスティネーション下位ワード
        pop     g3
        ret

;; 32ビット割算ルーチン
__div32                     ; int[] _div32(int[] dst, int src)
        push    g3
        ld      g0,4,sp     ; ディスティネーション(アドレス)
        ld      g1,0,g0     ; ディスティネーション上位ワード
        ld      g2,2,g0     ; ディスティネーション下位ワード
        ld      g0,#17      ; (g1,g2)は被除数, g0はカウンタ
        ld      g3,#0       ; g3は商
.D1     cmp     g1,6,sp     ; 被除数から除数が引けるなら引く
        jc      .D2         ;
        sub     g1,6,sp     ;
        add     g3,#1       ; 引いた時は商の最下位ビットを1にする
.D2     sub     g0,#1       ; 17回繰り返したなら
        jz      .D4         ;  終了
        shll    g1,#1       ; 被除数を左にシフト
        shll    g2,#1       ;
        jnc     .D3         ;
        add     g1,#1       ;
.D3     shll    g3,#1       ; 商を左にシフト
        jmp     .D1
.D4     ld      g0,4,sp     ; ディスティネーション(アドレス)
        st      g1,0,g0     ; 余りを上位ワードに格納
        st      g3,2,g0     ; 商を下位ワードに格納
        pop     g3
        ret

;; 32ビット剰余算ルーチン
__mod32                     ; int _mod32(int[] dst, int src)
        push    g3
        ld      g0,4,sp     ; ディスティネーション(アドレス)
        ld      g1,0,g0     ; ディスティネーション上位ワード
        ld      g2,2,g0     ; ディスティネーション下位ワード
        ld      g0,#17      ; (g1,g2)は被除数, g0はカウンタ
        ld      g3,#0       ; g3は商
.M1     cmp     g1,6,sp     ; 被除数から除数が引けるなら引く
        jc      .M2         ;
        sub     g1,6,sp     ;
        add     g3,#1       ; 引いた時は商の最下位ビットを1にする
.M2     sub     g0,#1       ; 17回繰り返したなら
        jz      .M4         ;  終了
        shll    g1,#1       ; 被除数を左にシフト
        shll    g2,#1       ;
        jnc     .M3         ;
        add     g1,#1       ;
.M3     shll    g3,#1       ; 商を左にシフト
        jmp     .M1
.M4     ld      g0,g1       ; 余りを返す
        pop     g3
        ret

;; SP の値を取得する
__sp
        ld      g0,sp
        ret

;; ワード(16bit)を I/O ポートから入力する
__in                            ; int in(int p);
        ld      g1,2,sp	        ; ポートアドレス
        in      g0,g1	        ; I/O ポートから入力する
        ret

;; ワードを I/O ポートへ出力する
__out                           ; void out(int p,int v);
        ld      g0,2,sp	        ; ポートアドレス
        ld      g1,4,sp	        ; 出力データ
        out     g1,g0	        ; I/O ポートへ出力する
        ret

;; ヒープとスタックの間に 10Byte 以上の余裕があるかチェックする
__stkChk
        ld      g0,__alcAddr    ; G0 にヒープ領域の最後をロード
        add     g0,#10          ; 10Byte 分の余裕を持たせる
        cmp     g0,sp           ; G0 と G1 を比較 [ヒープの最後+10Byte] - [SP]
        jnc     .stkOverFlow    ; ユーザスタックがオーバーフローしている
        ret

; スタックがオーバーフローした場合、 _exit(EUSTK) を実行
.stkOverFlow
        ld      g0,#-25         ; パラメータ(EUSTK)
        push    g0              ;   をスタックに積む
        call    __exit          ; exit を呼ぶ
