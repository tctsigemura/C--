#
# TacOS Source Code
#    Tokuyama kousen Educational Computer 16 bit Version
#
# Copyright (C) 2016 - 2018 by
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
# GNUmakefile : ユーザプログラム用の Makefile
#
# 2018.11.24         : clean で *.dSYM を消すように変更
# 2016.08.11         : 初期バージョン
#
# $Id$
#

#----------------------------------------------------------------------------
NAME=echo
SRCS=${NAME}.cmm
#----------------------------------------------------------------------------

all : clean ${NAME}.exe ${NAME}

${NAME}.exe : ${SRCS}
	cm2e -o ${NAME} ${SRCS}

${NAME} : ${SRCS}
	cm2c -o ${NAME} ${SRCS}

clean :
	rm -f ${NAME} *.o *.lst *.sym *.exe *.map *~
	rm -rf ${NAME}.dSYM

