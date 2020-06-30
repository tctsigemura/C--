#include <stdio.h>
#define _cmm_0S "/* */"
#define _cmm_1S "//"
#define _cmm_2S "/*"
#define _cmm_3S "/*"
#define _cmm_4S "abcde\"fghi"
#define _cmm_5S "\""
int main(){
_printf(_cmm_0S);
_printf(_cmm_1S);
_printf(_cmm_2S);
_printf(_cmm_3S);
_printf(_cmm_4S);
_printf(_cmm_5S);
putchar(10);
putchar(39);
return 0;
}
