#include <stdio.h>
#define _cmm_0S "\377\376"
#define _cmm_1S "a=%04x(00ff)\n"
#define _cmm_2S "b=%04x(0100)\n"
#define _cmm_3S "c=%04x(00ff)\n"
#define _cmm_4S "d=%04x(00ff)\n"
#define _cmm_5S "e[1]=%04x(00fe)\n"
#define _cmm_6S "f=%04x(00fe)\n"
int main(){
int a;
(a=255);
int b;
(b=256);
char c;
(c=255);
int d;
(d=c);
char *e;
(e=_cmm_0S);
int f;
(f=e[1]);
_printf(_cmm_1S,a);
_printf(_cmm_2S,b);
_printf(_cmm_3S,c);
_printf(_cmm_4S,d);
_printf(_cmm_5S,e[1]);
_printf(_cmm_6S,f);
return 0;
}
