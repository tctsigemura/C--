#
INCDIR=/usr/local/cmmInclude/
VER=\"3.0.0\"
DATE="\"`date`\""

CPFLAGS+=-DNWORD=8 -DNBYTE=16 -DDATE=${DATE} -DVER=${VER} -DARC=\"C\"
CFLAGS+=-Wno-parentheses-equality

CPP=cc -E -std=c99 -nostdinc -I${INCDIR} ${CPFLAGS} - < 

TRANS=../C--2C/trans

.SUFFIXES: .o .cmm

# .cmm -> .c
.cmm.c:
	${CPP} $*.cmm | ${TRANS} -I${INCDIR} ${CMFLAGS} > $*.c

OBJS=syntax.o lexical.o namtbl.o optree.o sytree.o util.o vm2tac.o vmCode.o

all: c-c--

c-c--: ${OBJS}
	cc -o c-c-- ${CFLAGS} ${OBJS}

clean:
	rm -f ${OBJS} *.cmm~ *.hmm~ Makefile~ c-c--
