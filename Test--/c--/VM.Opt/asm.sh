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
   echo '[!!!' "${n}.cmm => ${n}.s" '!!!]'
   cpp ${CPPFLAGS} -I${INCDIR} -I${LIBDIR} ${i} |
   ../../../src--/vm-c-- -O > ${n}.s
done

