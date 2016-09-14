#!/bin/sh

INCDIR=/usr/local/cmmInclude
LIBDIR=/usr/local/cmmLib

for i in $*; do
    j=`basename ${i}`
    n=`expr ${j} : '\([^\.]*\)'`
    echo '[!!!' "${n}.cmm => ${n}.s" '!!!]'
   cpp -xc++ -Wno-comment -nostdinc -I${INCDIR} -I${LIBDIR} ${i} |
    ../../../src/c-- -O > ${n}.s
done
