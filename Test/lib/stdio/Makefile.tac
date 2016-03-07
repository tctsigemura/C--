#
# Makefile.tac : C-- 言語からTacOSの実行形式に変換する手順
#
INCDIR=/usr/local/cmmInclude
LIBDIR=/usr/local/cmmLib

CFLAGS+=-Wno-parentheses-equality
CPP=cc -E -std=c99 -nostdinc -I${INCDIR} - < 

.SUFFIXES: .o .cmm .s
.cmm.s:
	${CPP} $*.cmm | c-- > $*.s

.s.o:
	as-- $*.s

TARGET=l010
OBJS=l010.o

${TARGET}.exe: ${OBJS}
	ld-- mod.o ${LIBDIR}/libtac.o ${OBJS} > ${TARGET}.sym
	objexe-- ${TARGET}.exe mod.o 600 | sort --key=1 > ${TARGET}.map

clean:
	rm -f ${TARGET} mod.o ${OBJS} *.sym *.lst *.map *.exe *~
