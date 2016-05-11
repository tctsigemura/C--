#!/bin/sh

for i in $*; do
    j=`basename ${i}`
    n=`expr ${j} : '\([^\.]*\)'`
    echo '[!!!' "${n}.cmm => ${n}.c" '!!!]'
    ../../../src/c-c-- -O ${i} > ${n}.c
done
