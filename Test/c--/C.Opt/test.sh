#!/bin/sh

INCDIR=/usr/local/cmmInclude
LIBDIR=/usr/local/cmmLib

for i in $*; do
   rm -f $$.c $$.s
   j=`basename ${i}`
   echo '[!!!' ${j} '!!!]'
   n=`expr ${j} : '\([^\.]*\)'`
   cc -E -DC -std=c99 -nostdinc -I${INCDIR} -I${LIBDIR} - < ${i} |
   ../../../src/c-c-- -O > $$.c
   diff ${n}.c $$.c
   cc -S -Wno-parentheses-equality -Wno-tautological-compare \
    -Wno-unused-value -Wno-unsequenced $$.c 
done

rm $$.c $$.s
