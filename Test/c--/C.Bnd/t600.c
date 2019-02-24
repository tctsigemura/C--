#include <stdio.h>
static _CA _cmm_0S={7,{"\377\376"}};
static _CA _cmm_1S={14,{"a=%04x(00ff)\n"}};
static _CA _cmm_2S={14,{"b=%04x(0100)\n"}};
static _CA _cmm_3S={14,{"c=%04x(00ff)\n"}};
static _CA _cmm_4S={14,{"d=%04x(00ff)\n"}};
static _CA _cmm_5S={17,{"e[1]=%04x(00fe)\n"}};
static _CA _cmm_6S={14,{"f=%04x(00fe)\n"}};
int main(){
int _cmm_1L;
(_cmm_1L=255);
int _cmm_2L;
(_cmm_2L=256);
char _cmm_3L;
(_cmm_3L=255);
int _cmm_4L;
(_cmm_4L=_cmm_3L);
_CA *_cmm_5L;
(_cmm_5L=&_cmm_0S);
int _cmm_6L;
(_cmm_6L=(*_CCA(_cmm_5L,1,__FILE__,__LINE__)));
printf(&_cmm_1S,_cmm_1L);
printf(&_cmm_2S,_cmm_2L);
printf(&_cmm_3S,_cmm_3L);
printf(&_cmm_4S,_cmm_4L);
printf(&_cmm_5S,(*_CCA(_cmm_5L,1,__FILE__,__LINE__)));
printf(&_cmm_6S,_cmm_6L);
return 0;
}
