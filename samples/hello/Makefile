#
INCDIR=/usr/local/cmmInclude/

#CPFLAGS+=
CFLAGS+=-Wno-parentheses-equality

CPP=cc -E -std=c99 -nostdinc -I${INCDIR} ${CPFLAGS} - < 

TRANS=c-c--

.SUFFIXES: .o .cmm .c

# .cmm -> .c
.cmm.c:
	${CPP} $*.cmm | ${TRANS} ${CMFLAGS} > $*.c

OBJS=hello.o

all: ${OBJS}
	cc ${CFLAGS} ${OBJS}
