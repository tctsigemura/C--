#!/bin/sh

INCDIR=/usr/local/cmmInclude
LIBDIR=/usr/local/cmmLib

for i in $*; do
   rm -f $$.c $$.s
   j=`basename ${i}`
   echo '[!!!' ${j} '!!!]'
   n=`expr ${j} : '\([^\.]*\)'`
   cpp -DC -xc++ -Wno-comment -nostdinc -I${INCDIR} -I${LIBDIR} ${i} |
   ../../../src/c-c-- -O > $$.c
   diff ${n}.c $$.c
   cc -S -funsigned-char -Wno-parentheses-equality -Wno-tautological-compare  \
     -Wno-pointer-sign -Wno-int-conversion -Wno-unused-value -Wno-unsequenced \
     $$.c 
done

rm $$.c $$.s
