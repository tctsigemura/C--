;
; Programing Language C-- "Compiler"
;    Tokuyama kousen Advanced educational Computer
;
;  Copyright (C) 2016 - 2019 by
;                       Dept. of Computer Science and Electronic Engineering,
;                       Tokuyama College of Technology, JAPAN
;
;    上記著作権者は，Free Software Foundation によって公開されている GNU 一般公
;  衆利用許諾契約書バージョン２に記述されている条件を満たす場合に限り，本ソース
;  コード(本ソースコードを改変したものを含む．以下同様)を使用・複製・改変・再配
;  布することを無償で許諾する．
;
;    本ソースコードは＊全くの無保証＊で提供されるものである。上記著作権者および
;  関連機関・個人は本ソースコードに関して，その適用可能性も含めて，いかなる保証
;  も行わない．また，本ソースコードの利用により直接的または間接的に生じたいかな
;  る損害に関しても，その責任を負わない．
;

;
; lib/syslib.s : システムコールに関するユーザ用ライブラリを集めたファイル
;
; 2019.10.18 : conRead, conWrite を ttyRead, ttyWrite に変更，ttyCtl を追加
; 2016.02.27 : _exit を __exit に変更
; 2016.02.24 : TacOS の usr/lib からコピー
;
; $Id$
;

; C-- 言語では記述できない内容を書く(レジスタの指定など)
; 名前が.から始まる関数・変数は、プログラム内だけで参照されるローカルなラベル
; 名前が_から始まる関数・変数は、C-- プログラムから参照できるグローバルなラベル

_errno  dw      0           ; エラー番号

; システムコール番号
; 0      exec
; 1      _exit
; 2      wait
; 3      sleep
; 4      creat
; 5      remove
; 6      mkDir
; 7      rmDIr
; 8      open
; 9      close
; 10     read
; 11     write
; 12     seek
; 13     ttyRead
; 14     ttyWrite
; 15     ttyCtl
; 16     malloc
; 17     free


; ユーザスタックに積まれたパラメータをカーネルスタックに積み直す
; ここでは、ユーザスタックから G1~G3 レジスタに取り出してレジスタ渡しを行う
; SVC 割込み後、SVC ハンドラ内でレジスタ渡ししたパラメータを
;   カーネルスタックに積み直す
; G0~G2 はアキュムレータなため破壊してもよいが、
;   G3 はレジスタ変数なため破壊してはいけない
; # パラメータ数が3つ以下の場合は、残りのレジスタに0を格納しておく


; ユーザスタックの状態(push g3後)
; addr  SP からの相対    スタックの状態
; 00
;       SP+0            G3      今、SP はここを指している
;       SP+2            PC
;       SP+4            prm1
;       SP+6            prm2
;       SP+8            prm3
; FF

; パラメータが1つの場合の処理
.l1
        push    g3              ; G3 はレジスタ変数なので破壊してはいけない
        ;  **  この時点で上のスタックの状態になっている  **
        ld      g1,4,sp         ; G1 に第1パラメータを格納
        ld      g2,#0           ; G2 に 0 を格納
        ld      g3,#0           ; G3 に 0 を格納
        jmp     .cmn            ; 共通処理にジャンプ

; パラメータが2つの場合の処理
.l2
        push    g3              ; G3 はレジスタ変数なので破壊してはいけない
        ;  **  この時点で上のスタックの状態になっている  **
        ld      g1,4,sp         ; G1 に第1パラメータを格納
        ld      g2,6,sp         ; G2 に第2パラメータを格納
        ld      g3,#0           ; G3 に 0 を格納
        jmp     .cmn            ; 共通処理にジャンプ

; パラメータが3つの場合の処理
.l3
        push    g3              ; G3 はレジスタ変数なので破壊してはいけない
        ;  **  この時点で上のスタックの状態になっている  **
        ld      g1,4,sp         ; G1 に第1パラメータを格納
        ld      g2,6,sp         ; G2 に第2パラメータを格納
        ld      g3,8,sp         ; G3 に第3パラメータを格納

; .l1~l3 の共通処理
.cmn
        svc                     ; SVC 割り込みを発生させる
        cmp     g0,#0           ; システムコールの返り値が
        jnm     .restr          ; 0以上なら errno を操作しない
        st      g0,_errno       ; errno にシステムコールのエラー番号を格納
.restr  pop     g3              ; G3 を復元
        ret

_exec
        ld      g0,#0           ; G0 にシステムコール番号を格納
        jmp     .l3

__exit
        ld      g0,#1           ; G0 にシステムコール番号を格納
        jmp     .l1

_wait
        ld      g0,#2           ; G0 にシステムコール番号を格納
        jmp     .l1

_sleep
        ld      g0,#3           ; G0 にシステムコール番号を格納
        jmp     .l1

_creat
        ld      g0,#4           ; G0 にシステムコール番号を格納
        jmp     .l1

_remove
        ld      g0,#5           ; G0 にシステムコール番号を格納
        jmp     .l1

_mkDir
        ld      g0,#6           ; G0 にシステムコール番号を格納
        jmp     .l1

_rmDir
        ld      g0,#7           ; G0 にシステムコール番号を格納
        jmp     .l1

_open
        ld      g0,#8           ; G0 にシステムコール番号を格納
        jmp     .l2

_close
        ld      g0,#9           ; G0 にシステムコール番号を格納
        jmp     .l1

_read
        ld      g0,#10          ; G0 にシステムコール番号を格納
        jmp     .l3

_write
        ld      g0,#11          ; G0 にシステムコール番号を格納
        jmp     .l3

_seek
        ld      g0,#12          ; G0 にシステムコール番号を格納
        jmp    .l3

_ttyRead
        ld      g0,#13          ; G0 にシステムコール番号を格納
        jmp     .l2

_ttyWrite
        ld      g0,#14          ; G0 にシステムコール番号を格納
        jmp     .l2

_ttyCtl
        ld      g0,#15          ; G0 にシステムコール番号を格納
        jmp     .l2

; malloc(#16) と free(#17) システムコールはユーザプロセスでは使用できない
; ユーザプロセスでは malloc と free はライブラリ関数として実現される
