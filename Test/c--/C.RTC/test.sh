#!/bin/sh

LIBDIR=/usr/local/cmmLib/LibRtc
RTC="-D_RTC"

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
   rm -f $$.c $$.s
   j=`basename ${i}`
   echo '[!!!' ${j} '!!!]'
   n=`expr ${j} : '\([^\.]*\)'`
   cpp ${CPPFLAGS} ${RTC} ${INCDIR} -include cfunc.hmm ${i} |
   ../../../src/rtc-c-- -O > $$.c
   diff ${n}.c $$.c
   cc -S ${CFLAGS} ${RTC} ${INCDIR} -include wrapper.h $$.c
done

rm $$.c $$.s
