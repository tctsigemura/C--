#include <stdio.h>
#define _cmm_0S ((unsigned char*)"\377\376")
#define _cmm_1S ((unsigned char*)"a=%04x(00ff)\n")
#define _cmm_2S ((unsigned char*)"b=%04x(0100)\n")
#define _cmm_3S ((unsigned char*)"c=%04x(00ff)\n")
#define _cmm_4S ((unsigned char*)"d=%04x(00ff)\n")
#define _cmm_5S ((unsigned char*)"f=%04x(00fe)\n")
int main(){
int _cmm_1L;
(_cmm_1L=255);
int _cmm_2L;
(_cmm_2L=256);
unsigned char _cmm_3L;
(_cmm_3L=255);
int _cmm_4L;
(_cmm_4L=_cmm_3L);
unsigned char *_cmm_5L;
(_cmm_5L=_cmm_0S);
int _cmm_6L;
(_cmm_6L=_cmm_5L[1]);
printf(_cmm_1S,_cmm_1L);
printf(_cmm_2S,_cmm_2L);
printf(_cmm_3S,_cmm_3L);
printf(_cmm_4S,_cmm_4L);
printf(_cmm_5S,_cmm_6L);
return 0;
}
