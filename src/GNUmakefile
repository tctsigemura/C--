#
#  Programing Language C-- "Compiler"
#    Tokuyama kousen Educational Computer 16 bit Version
#
# Copyright (C) 2009-2022 by
#                      Dept. of Computer Science and Electronic Engineering,
#                      Tokuyama College of Technology, JAPAN
#
#   上記著作権者は，Free Software Foundation によって公開されている GNU 一般公
# 衆利用許諾契約書バージョン２に記述されている条件を満たす場合に限り，本ソース
# コード(本ソースコードを改変したものを含む．以下同様)を使用・複製・改変・再配
# 布することを無償で許諾する．
#
#   本ソースコードは＊全くの無保証＊で提供されるものである。上記著作権者および
# 関連機関・個人は本ソースコードに関して，その適用可能性も含めて，いかなる保証
# も行わない．また，本ソースコードの利用により直接的または間接的に生じたいかな
# る損害に関しても，その責任を負わない．
#
#
# GNUmakefile : c-- の makefile
#
# 2022.11.22         : -DCをトランスレータのソースと，トランスレータが処理する
#                      ソースの両方で使用．トランスレータ側を-D_Cに変更した．
# 2020.03.21         : cm2e, cm2c, cm2i, cm2v のヘルプ表示にバージョン番号追加
# 2019.03.11         : Run Time Check の略称を RTC にする
# 2019.03.10         : ソースコードのファイル名変更
# 2019.02.21         : 配列境界とnullポインタの実行時チェック
# 2018.01.27 v3.1.10 : ドライバで -nostdinc -I オプションを使用できる
# 2016.11.20 v3.1.9  : トランスレータのfor文中でcontinueを使用した時のバグ訂正
# 2016.10.16 v3.1.8  : トランスレータのif文入れ子時のバグ対策，コンパクトな出力
#                    : if文のテスト入力(t700.cmm)追加、Makefile改善
# 2016.10.09 v3.1.7  : トランスレータのif文入れ子時のバグ解決，doc改訂
# 2016.09.24         : ic-c--, cm2i を追加（中間コードを出力するバージョン）
# 2016.09.16 v3.1.6  : 中間言語の仕様変更
# 2016.09.14 v3.1.5  : トランスレータのテストでエラーになる問題を解決
# 2016.09.12 v3.1.4  : バグ修正(doc,字句解析)
# 2016.08.27 v3.1.3  : バグ修正(doc,stdib.hmm)と若干の改良
# 2016.08.13 v3.1.2  : バグ修正と若干の改良
# 2016.08.13         : feofの仕様変更
# 2016.08.12         : cm2c, cm2e, cm2v が cc -E ではなく cpp -xc++ を使用
#                      cm2c, cm2e, cm2v のエラー処理改良
# 2016.06.29 v3.1.1  : 可変個引数関数の実引数にvoid型を渡せるバグ修正
# 2016.06.12 v3.1.0  : トランスレータが正常に使用できる
# 2016.05.24         : cm2c, cm2e, cm2v を追加
# 2016.05.20         : トランスレータ版では C を定義するように変更
# 2016.02.05 v3.0.0  : トランスレータと統合
# 2016.01.18 v2.1.2  : vm2tac.cmm の vmPop() の意味の無い式に関するバグ修正
# 2015.12.16 v2.1.1  : void以外の関数の return チェックに関するバグ修正
# 2015.10.04         : -Wall オプションを追加
# 2015.08.31 v2.1.0  : H8 サポート中止
# 2012.12.27         : TaC 番の NBYTE を 8 に変更
# 2012.08.20 v2.0.0  : code.c 削除、vm2tac.c 追加
# 2011.05.02         : install のオプションから -o root を削除(Cygwin対応)
# 2010.12.14 v1.0.1  : VER の定義を訂正して v1.0.1 をタグ付けする
# 2010.09.12 v1.0.0  : 最適化と外部変数の定数式による初期化ができる H8 統合版
# 2010.09.06         : 整数のビット数 NINT を定義してコンパイルするように変更
# 2010.09.05         : optree.c、optree.h を追加
# 2010.08.21         : DATE を定義してコンパイルするように変更
# 2010.08.18         : H8 Tiny 版を追加
# 2010.07.23         : VER ARC を定義してコンパイルするように変更
# 2010.07.20         : Subversion の管理に入る
# 2010.04.09 v0.6.3  : install のオプションから -g wheel を削除(Ubuntu対応)
#

CC=cc -std=c99 -Wall
VER=`cat ../VERSION`(src)
DATE=`date`

all : c-- vm-c-- ic-c-- c-c-- rtc-c-- cm2e cm2c cm2i cm2v

c-- : GNUmakefile main.c parser.c parser.h lexer.c lexer.h util.h util.c \
      namtbl.h namtbl.c sytree.h sytree.c code.h vmCode.c vm2tac.c \
      optree.h optree.c
	${CC} ${CFLAGS} -DDATE=\""${DATE}"\" -DVER=\""${VER}"\" \
	-DARC=\"TaC\" -DNWORD=16 -DNBYTE=8 -o c-- \
	main.c parser.c lexer.c namtbl.c util.c sytree.c \
	vmCode.c vm2tac.c optree.c

vm-c-- : GNUmakefile main.c parser.c parser.h lexer.c lexer.h util.h \
      util.c namtbl.h namtbl.c sytree.h sytree.c code.h vmCode.c vm.h vm2vm.c \
      optree.h optree.c
	${CC} ${CFLAGS} -DDATE=\""${DATE}"\" -DVER=\""${VER}"\" \
	-DARC=\"VM\" -DNWORD=16 -DNBYTE=8 -o vm-c-- \
	main.c parser.c lexer.c namtbl.c util.c sytree.c \
	vmCode.c vm2vm.c optree.c

ic-c-- : GNUmakefile main.c parser.c parser.h lexer.c lexer.h util.h \
      util.c namtbl.h namtbl.c sytree.h sytree.c code.h vmCode.c vm.h vm2ic.c \
      optree.h optree.c
	${CC} ${CFLAGS} -DDATE=\""${DATE}"\" -DVER=\""${VER}"\" \
	-DARC=\"IC\" -DNWORD=16 -DNBYTE=8 -o ic-c-- \
	main.c parser.c lexer.c namtbl.c util.c sytree.c \
	vmCode.c vm2ic.c optree.c

c-c-- : GNUmakefile main.c parser.c parser.h lexer.c lexer.h util.h util.c \
      namtbl.h namtbl.c sytree.h sytree.c code.h cCode.c \
      optree.h optree.c
	${CC} ${CFLAGS} -DDATE=\""${DATE}"\" -DVER=\""${VER}"\" \
	-DARC=\"C\" -D_C -DNWORD=32 -DNBYTE=8 -o c-c-- \
	main.c parser.c lexer.c namtbl.c util.c sytree.c \
	cCode.c optree.c

rtc-c-- : GNUmakefile main.c parser.c parser.h lexer.c lexer.h util.h util.c \
      namtbl.h namtbl.c sytree.h sytree.c code.h cCode.c \
      optree.h optree.c
	${CC} ${CFLAGS} -D_RTC -DDATE=\""${DATE}"\" -DVER=\""${VER}"\" \
	-DARC=\"C\" -D_C -DNWORD=32 -DNBYTE=8 -o rtc-c-- \
	main.c parser.c lexer.c namtbl.c util.c sytree.c \
	cCode.c optree.c

#h8300hn-c-- : Makefile parser.c lexer.c lexer.h util.h util.c \
#      namtbl.h namtbl.c sytree.h sytree.c code.h vmCode.c vm.h vm2h8300hn.c \
#      optree.h optree.c
#	${CC} ${CFLAGS} -DDATE=\""${DATE}"\" -DVER=\""${VER}"\" \
#	-DARC="\"Renesas H8/300H Tiny\"" -DNWORD=16 -DNBYTE=8 -o h8300hn-c-- \
#	parser.c lexer.c namtbl.c util.c sytree.c vmCode.c vm2h8300hn.c \
#	optree.c

cm2e : GNUmakefile cm2e.in
	sed -e "s/%%ver%%/${VER}/" -e "s/%%date%%/${DATE}/" cm2e.in > cm2e

cm2c : GNUmakefile cm2c.in
	sed -e "s/%%ver%%/${VER}/" -e "s/%%date%%/${DATE}/" cm2c.in > cm2c

cm2i : GNUmakefile cm2i.in
	sed -e "s/%%ver%%/${VER}/" -e "s/%%date%%/${DATE}/" cm2i.in > cm2i

cm2v : GNUmakefile cm2v.in
	sed -e "s/%%ver%%/${VER}/" -e "s/%%date%%/${DATE}/" cm2v.in > cm2v

install : c-- vm-c-- ic-c-- c-c-- rtc-c-- cm2c cm2e cm2i cm2v # h8300hn-c--
	install -d -m 755 /usr/local/bin
	install -m 755 c-- /usr/local/bin
	install -m 755 vm-c-- /usr/local/bin
	install -m 755 ic-c-- /usr/local/bin
	install -m 755 c-c-- /usr/local/bin
	install -m 755 rtc-c-- /usr/local/bin
	install -m 755 cm2c /usr/local/bin
	install -m 755 cm2e /usr/local/bin
	install -m 755 cm2i /usr/local/bin
	install -m 755 cm2v /usr/local/bin
#	install -m 755 h8300hn-c-- /usr/local/bin

clean :
	rm -f c-- vm-c-- c-c-- rtc-c-- ic-c-- cm2e cm2c cm2i cm2v \
        *.o *.c~ *.h~ *.txt~ *.cmm~  Makefile~ cm2?~ GNUmakefile~

