#
#  Programing Language C-- "Compiler"
#    Tokuyama kousen Advanced educational Computer
#
# Copyright (C) 2016 by
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
# lib/Makefile : makefile
#
# 2016.05.26 : cm2e を使用するように変更
#              wrapper.h 追加
# 2016.02.27 : 初期バージョン
#

# 使用されそうなフラグ
#CPFLAGS+=-DDEBUG
#CMFLAGS+=-K

# make するソースのサフィックス（拡張子） の一覧
.SUFFIXES: .o .cmm .s

# .cmm -> .o
.s.o:
	as-- $*.s

# .cmm -> .o
.cmm.o:
	cm2e -c  $*.cmm

OBJS= crt0.o ctype.o stdio.o printf.o stdlib.o string.o syslib.o

all: libtac.o

libtac.o: ${OBJS}
	ld-- libtac.o ${OBJS} > libtac.sym

install: libtac.o
	install -d -m 755 /usr/local/cmmLib
	install -m 644 libtac.o /usr/local/cmmLib
	install -m 644 cfunc.hmm /usr/local/cmmLib
	install -m 644 wrapper.c /usr/local/cmmLib
	install -m 644 wrapper.h /usr/local/cmmLib

clean:
	rm -f *.o *.lst libtac.sym *~
