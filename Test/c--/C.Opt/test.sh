#!/bin/sh

for i in $*; do
   j=`basename ${i}`
   echo '[!!!' ${j} '!!!]'
   n=`expr ${j} : '\([^\.]*\)'`
   ../../../src/c-c-- -O ${i} > t.$$
   diff ${n}.c t.$$
done

rm t.$$
