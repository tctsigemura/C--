#!/bin/sh

for i in $*; do
   j=`basename ${i}`
   echo '[!!!' ${j} '!!!]'
   n=`expr ${j} : '\([^\.]*\)'`
   ../../../c-- -O ${i} > t.$$
   diff ${n}.s t.$$
done

rm t.$$
