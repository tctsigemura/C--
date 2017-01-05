#!/bin/sh

INCDIR=/usr/local/cmmInclude
LIBDIR=/usr/local/cmmLib

for i in $*; do
   j=`basename ${i}`
   echo '[!!!' ${j} '!!!]'
   n=`expr ${j} : '\([^\.]*\)'`
   cc -E -DC -Wno-comment -std=c99 -nostdinc -I${INCDIR} -I${LIBDIR} - < ${i} |
#   ../../../src/StepByStep/dc--.sh > t.$$
   ../../../src/t-c-- > t.$$
   diff -I "\.L\d+" -I "\.S\d+" ${n}.s t.$$
done

rm t.$$
