#
INCDIR=../../cmmInclude/

#CPFLAGS+=
CFLAGS+=-Wno-parentheses-equality

CPP=cc -E -std=c99 -nostdinc -I${INCDIR} ${CPFLAGS} - < 

TRANS=../../src/c-c--

.SUFFIXES: .o .cmm .c

# .cmm -> .c
.cmm.c:
	${CPP} $*.cmm | ${TRANS} ${CMFLAGS} > $*.c

OBJS=syntax.o lexical.o namtbl.o optree.o sytree.o util.o vm2tac.o vmCode.o

all: ${OBJS}
	cc ${CFLAGS} ${OBJS}
