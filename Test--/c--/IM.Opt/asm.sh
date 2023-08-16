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
   n=`expr ${j} : '\([^\.]*\)'`
   echo '[!!!' "${n}.cmm => ${n}.i" '!!!]'
   cpp ${CPPFLAGS} -I${INCDIR} -I${LIBDIR} ${i} |
   ../../../src--/ic-c-- -O > ${n}.i
done

