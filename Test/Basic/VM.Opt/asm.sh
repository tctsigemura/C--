#!/bin/sh

for i in $*; do
    j=`basename ${i}`
    n=`expr ${j} : '\([^\.]*\)'`
    echo '[!!!' "${n}.cmm => ${n}.s" '!!!]'
    ../../../src/vm-c-- -O ${i} > ${n}.s
done
