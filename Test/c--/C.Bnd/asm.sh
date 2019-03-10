#!/bin/sh

INCDIR=/usr/local/cmmInclude
LIBDIR=/usr/local/cmmLib

for i in $*; do
    j=`basename ${i}`
    n=`expr ${j} : '\([^\.]*\)'`
    echo '[!!!' "${n}.cmm => ${n}.c" '!!!]'
   cpp -DC -xc++ -Wno-comment -nostdinc -I${INCDIR} -I${LIBDIR} ${i} |
   ../../../src/cb-c-- -O > ${n}.c
#   cc -nostdinc -nostdlib -S ${n}.c
#   rm -f ${n}.s
done
