#include <stdio.h>
static _CA _cmm_0S={3,{"\377\376"}};
static _CA _cmm_1S={14,{"a=%04x(00ff)\n"}};
static _CA _cmm_2S={14,{"b=%04x(0100)\n"}};
static _CA _cmm_3S={14,{"c=%04x(00ff)\n"}};
static _CA _cmm_4S={14,{"d=%04x(00ff)\n"}};
static _CA _cmm_5S={17,{"e[1]=%04x(00fe)\n"}};
static _CA _cmm_6S={14,{"f=%04x(00fe)\n"}};
int __main(){
int a;
(a=255);
int b;
(b=256);
char c;
(c=255);
int d;
(d=c);
_CA *e;
(e=&_cmm_0S);
int f;
(f=(*_CCA(e,1,"../t600.cmm",12)));
_printf(&_cmm_1S,a);
_printf(&_cmm_2S,b);
_printf(&_cmm_3S,c);
_printf(&_cmm_4S,d);
_printf(&_cmm_5S,(*_CCA(e,1,"../t600.cmm",18)));
_printf(&_cmm_6S,f);
return 0;
}
