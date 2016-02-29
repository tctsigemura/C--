#
# C-- 言語からTacOSの実行形式に変換する手順
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

OBJS=hello.o

hello.exe: ${OBJS}
	ld-- mod.o ${LIBDIR}/libtac.o ${OBJS} > hello.sym
	objexe-- hello.exe mod.o 600 | sort --key=1 > hello.map

clean:
	rm -f hello.exe mod.o ${OBJS} *.sym *.lst *.map
