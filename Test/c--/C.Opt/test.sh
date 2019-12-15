#!/bin/sh

LIBDIR=/usr/local/cmmLib/LibNoRtc
RTC=""

INCDIR="-I/usr/local/cmmInclude -I${LIBDIR}"
CPPFLAGS="-xc++ -Wno-comment -nostdinc -nostdlibinc -nobuiltininc -DC"
CFLAGS="-g -O0 \
   -funsigned-char -Wno-parentheses-equality -Wno-tautological-compare \
   -Wno-pointer-sign -Wno-int-conversion -Wno-unused-value -Wno-unsequenced \
   -Wno-dangling-else -Wno-format-security"

for i in $*; do
   rm -f $$.c $$.s
   j=`basename ${i}`
   echo '[!!!' ${j} '!!!]'
   n=`expr ${j} : '\([^\.]*\)'`
   cpp ${CPPFLAGS} ${RTC} ${INCDIR} -include cfunc.hmm ${i} |
   ../../../src/c-c-- -O > $$.c
   diff ${n}.c $$.c
   cc -S ${CFLAGS} ${RTC} ${INCDIR} -include wrapper.h $$.c
done

rm $$.c $$.s
