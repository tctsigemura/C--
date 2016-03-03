#
# C-- 言語から色々なファイルに変換する手順
#

INCDIR=/usr/local/cmmInclude
LIBDIR=/usr/local/cmmLib

CFLAGS+=-Wno-parentheses-equality
%.c : CPPFLAGS = -DC

# プリプロセッサの実行方法
CPP=cc -E ${CPPFLAGS} -std=c99 -nostdinc -I${INCDIR} -I${LIBDIR} - < 

all: hello hello.v hello.s hello.exe

# MacOS, UNIX の実行形式を作る
hello: hello.c
	cc -o hello ${CFLAGS} hello.c ${LIBDIR}/wrapper.c

# TacOS の実行形式を作る
hello.exe: hello.s
	as-- hello.s
	ld-- mod.o /usr/local/cmmLib/libtac.o hello.o > hello.sym
	objexe-- hello.exe mod.o 600 | sort --key=1 > hello.map

# C言語に変換する
hello.c: hello.cmm
	${CPP} hello.cmm | c-c-- > hello.c

# C--コンパイラの中間言語に変換する
hello.v: hello.cmm
	${CPP} hello.cmm | vm-c-- > hello.v

# TaCのアセンブリ言語に変換する
hello.s: hello.cmm
	${CPP} hello.cmm | c-- > hello.s

clean:
	rm -f hello hello.c hello.s hello.v *.lst *.sym *.map *.o *.exe *~
