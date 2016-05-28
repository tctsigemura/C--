#
INCDIR=/usr/local/cmmInclude/
LIBDIR=/usr/local/cmmLib/
VER=3.0.0
DATE=`date`

CPFLAGS+=-DNWORD=8 -DNBYTE=32
CPFLAGS+="-DDATE=\"${DATE}\"" "-DVER=\"${VER}\"" "-DARC=\"C\""

.SUFFIXES: .o .cmm

# .cmm -> .o
.cmm.o:
	cm2c -c ${CPFLAGS} $*.cmm >$*.c

OBJS=main.o syntax.o lexical.o namtbl.o optree.o sytree.o util.o \
    vm2tac.o vmCode.o

all: c-c--

c-c--: ${OBJS}
	cm2c -o c-c-- ${OBJS}

clean:
	rm -f ${OBJS} *.cmm~ *.hmm~ Makefile~ c-c--
