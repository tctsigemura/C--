#!/bin/sh

for i in $*; do
   j=`basename ${i}`
   echo '[!!!' ${j} '!!!]'
   n=`expr ${j} : '\([^\.]*\)'`
   ../../../src/lx ${i}
   ../../../src/sn ${i%.*}.lx
   ../../../src/op ${i%.*}.sm
   ../../../src/vm ${i%.*}.op
   ../../../src/tac ${i%.*}.vm > t.$$

   diff ${n}.s t.$$
done

rm t.$$ ../*.lx ../*.sm ../*.op ../*.vm
