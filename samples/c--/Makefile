#
INCDIR=/usr/local/cmmInclude/
LIBDIR=/usr/local/cmmLib/
VER=3.0.0
DATE=`date`

CPFLAGS+=-DNWORD=8 -DNBYTE=32
CPFLAGS+="-DDATE=\"${DATE}\"" "-DVER=\"${VER}\""

SRCS=lexical.cmm namtbl.cmm syntax.cmm util.cmm main.cmm optree.cmm sytree.cmm

all: c-- # c-c-- v-c--

c--:
	cm2c -o c-- ${CPFLAGS} -DARC=\"TaC\" ${SRCS} vmCode.cmm vm2tac.cmm

c-c--:
	cm2c -o c-c-- ${CPFLAGS} -DARC=\"C\" ${SRCS}

v-c--:
	cm2c -o v-c-- ${CPFLAGS} -DARC=\"VM\" ${SRCS}

clean:
	rm -f *.cmm~ *.hmm~ Makefile~ c-- c-c-- v-c--
