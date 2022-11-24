#!/bin/sh

INCDIR=/usr/local/cmmInclude
LIBDIR=/usr/local/cmmLib

if [ `uname` = Darwin ]; then
  CPPFLAGS="-xc++ -Wno-comment -nostdinc -nostdlibinc -nobuiltininc" # macOS
else
  CPPFLAGS="-Wno-comment -nostdinc"                                 # Ubuntu
fi

for i in $*; do
   j=`basename ${i}`
   echo '[!!!' ${j} '!!!]'
   n=`expr ${j} : '\([^\.]*\)'`
   cpp ${CPPFLAGS} -I${INCDIR} -I${LIBDIR} ${i} |
   ../../../src--/c-- -O > t.$$
   diff ${n}.s t.$$
done

rm t.$$
