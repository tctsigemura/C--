#!/bin/sh

LIBDIR=/usr/local/cmmLib/LibNortc
RTC=""

INCDIR="-I/usr/local/cmmInclude -I${LIBDIR}"
if [ `uname` = Darwin ]; then
  CPPFLAGS="-xc++ -Wno-comment -nostdinc -nostdlibinc -nobuiltininc -DC" # macOS
else
  CPPFLAGS="-Wno-comment -nostdinc -DC"                                 # Ubuntu
fi

CFLAGS="-g -O0 \
   -funsigned-char -Wno-parentheses-equality -Wno-tautological-compare \
   -Wno-pointer-sign -Wno-int-conversion -Wno-unused-value -Wno-unsequenced \
   -Wno-dangling-else -Wno-format-security"

for i in $*; do
   j=`basename ${i}`
   n=`expr ${j} : '\([^\.]*\)'`
   echo '[!!!' "${n}.cmm => ${n}.c" '!!!]'
   cpp ${CPPFLAGS} ${RTC} ${INCDIR} -include cfunc.hmm ${i} |
   ../../../src/c-c-- -O > ${n}.c
#   cc -S ${CFLAGS} ${RTC} ${INCDIR} -include wrapper.h ${n}.c
#   rm -f ${n}.s
done

