#
#  Programing Language C-- "Compiler"
#    Tokuyama kousen Educational Computer 16 bit Version
#
# Copyright (C) 2009-2016 by
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
# Makefile : c-- の makefile
#
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
VER=\"3.0.0\"
DATE="\"`date`\""

all : c-- vm-c-- c-c--

c-- : Makefile main.c syntax.c syntax.h lexical.c lexical.h util.h util.c \
      namtbl.h namtbl.c sytree.h sytree.c code.h vmCode.c vm2tac.c \
      optree.h optree.c
	${CC} ${CFLAGS} -DDATE=${DATE} -DVER=${VER} -DARC=\"TaC\" \
	-DNWORD=16 -DNBYTE=8 -o c-- \
	main.c syntax.c lexical.c namtbl.c util.c sytree.c \
	vmCode.c vm2tac.c optree.c

vm-c-- : Makefile main.c syntax.c syntax.h lexical.c lexical.h util.h util.c \
      namtbl.h namtbl.c sytree.h sytree.c code.h vmCode.c vm.h vm2vm.c \
      optree.h optree.c
	${CC} ${CFLAGS} -DDATE=${DATE} -DVER=${VER} -DARC=\"VM\" \
	-DNWORD=16 -DNBYTE=8 -o vm-c-- \
	main.c syntax.c lexical.c namtbl.c util.c sytree.c \
	vmCode.c vm2vm.c optree.c

c-c-- : Makefile main.c syntax.c syntax.h lexical.c lexical.h util.h util.c \
      namtbl.h namtbl.c sytree.h sytree.c code.h cCode.c \
      optree.h optree.c
	${CC} ${CFLAGS} -DDATE=${DATE} -DVER=${VER} -DARC=\"C\" \
	-DNWORD=32 -DNBYTE=8 -o c-c-- \
	main.c syntax.c lexical.c namtbl.c util.c sytree.c \
	cCode.c optree.c

#h8300hn-c-- : Makefile syntax.c lexical.c lexical.h util.h util.c \
#      namtbl.h namtbl.c sytree.h sytree.c code.h vmCode.c vm.h vm2h8300hn.c \
#      optree.h optree.c
#	${CC} ${CFLAGS} -DDATE=${DATE} -DVER=${VER} \
#	-DARC="\"Renesas H8/300H Tiny\"" -DNWORD=16 -DNBYTE=8 -o h8300hn-c-- \
#	syntax.c lexical.c namtbl.c util.c sytree.c vmCode.c vm2h8300hn.c \
#	optree.c

install : c-- vm-c-- c-c-- # h8300hn-c--
	install -d -m 755 /usr/local/bin
	install -m 755 c-- /usr/local/bin
	install -m 755 vm-c-- /usr/local/bin
	install -m 755 c-c-- /usr/local/bin
#	install -m 755 h8300hn-c-- /usr/local/bin

clean :
	rm -f c-- vm-c-- c-c-- *.o *.c~ *.h~ *.txt~ *.cmm~ Makefile~
#	rm -f h8300hn-c-- vm-c-- c-- *.o *.c~ *.h~ *.txt~ *.cmm~ Makefile~

