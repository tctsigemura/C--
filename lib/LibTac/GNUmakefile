#
#  Programing Language C-- "Compiler"
#    Tokuyama kousen Advanced educational Computer
#
# Copyright (C) 2016 - 2019 by
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
# LibTac/GNUmakefile : TaC用ライブラリのmakefile
#
# 2019.11.13 : environ を追加
# 2019.03.15 : LibTac に移動
# 2018.02.24 : ../include の .hmm を使用するように変更
# 2018.01.31 : .s にも cm2e を使用するように変更
# 2016.06.01 : シンボリックリンク crt0.h を追加
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
	cm2e -c $*.s

# .cmm -> .o
.cmm.o:
	cm2e -nostdinc -I../../include -I../SrcC -c $*.cmm

# crt0.o を最初に置くこと
OBJS= ../SrcTac/crt0.o  ../SrcTac/syslib.o ../SrcTac/wrap-syslib.o \
      ../SrcTac/stdio.o ../SrcTac/stdlib.o ../SrcTac/printf.o \
      ../SrcCmm/ctype.o ../SrcCmm/string.o ../SrcTac/environ.o

all: libtac.o

libtac.o: ${OBJS}
	ld-- libtac.o ${OBJS} > libtac.sym
	rm ${OBJS}

install: libtac.o
	install -d -m 755 /usr/local/cmmLib
	install -m 644 libtac.o /usr/local/cmmLib

clean:
	rm -f ${OBJS} libtac.* *~
