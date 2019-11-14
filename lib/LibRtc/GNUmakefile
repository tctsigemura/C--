#
#  Programing Language C-- "Compiler"
#    Tokuyama kousen Advanced educational Computer
#
# Copyright (C) 2019
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
# lib/Makefile.rtc : C言語版（実行時エラーチェックあり）用
#
# 2019.11.13 : environ を追加
# 2019.03.10 : 初期バージョン
#

# make するソースのサフィックス（拡張子） の一覧
.SUFFIXES: .c .cmm

# .cmm -> .c
.cmm.c:
	cm2c -S -rtc -nostdinc -I../../include -I../SrcC -c $*.cmm

OBJS= ctype.c string.c wrapper.h environ.h
TMP= ../SrcCmm/ctype.c ../SrcCmm/string.c

all: ${OBJS}

${OBJS}: ${TMP}
	mv ${TMP} .
	cp ../SrcC/wrapper.h .
	cp ../SrcC/environ.h .
	rtc-c-- ../../include/ctype.hmm >> wrapper.h
	rtc-c-- ../../include/string.hmm >> wrapper.h
	cat environ.h >> wrapper.h

install: ${OBJS}
	install -d -m 755 /usr/local/cmmLib/LibRtc
	install -m 644 *.[ch]  /usr/local/cmmLib/LibRtc
	install -m 644 ../SrcC/cfunc.hmm /usr/local/cmmLib/LibRtc
	install -m 644 ../SrcC/wrapper.c /usr/local/cmmLib/LibRtc
	install -m 644 ../SrcC/printf.c  /usr/local/cmmLib/LibRtc
	install -m 644 ../SrcC/environ.c  /usr/local/cmmLib/LibRtc
	install -m 644 ../SrcC/environ.h  /usr/local/cmmLib/LibRtc

clean:
	rm -f ${TMP} ${OBJS} *~
