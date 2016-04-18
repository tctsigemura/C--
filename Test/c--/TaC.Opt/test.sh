#!/bin/sh

for i in $*; do
   j=`basename ${i}`
   echo '[!!!' ${j} '!!!]'
   n=`expr ${j} : '\([^\.]*\)'`
   ../../../src/lx ${i}
   ../../../src/d-c-- > t.$$
   diff ${n}.s t.$$
done

rm t.$$
