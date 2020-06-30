#!/bin/sh

INCDIR=/usr/local/cmmInclude
LIBDIR=/usr/local/cmmLib

CPPFLAGS="-xc++ -Wno-comment -nostdinc -nostdlibinc -nobuiltininc"

for i in $*; do
   j=`basename ${i}`
   n=`expr ${j} : '\([^\.]*\)'`
   echo '[!!!' "${n}.cmm => ${n}.s" '!!!]'
   cpp ${CPPFLAGS} -I${INCDIR} -I${LIBDIR} ${i} |
   ../../../src/vm-c-- -O > ${n}.s
done

