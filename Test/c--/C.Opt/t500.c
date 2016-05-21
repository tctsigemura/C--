#include <stdio.h>
FILE *g(FILE *_cmm_1P);
static FILE *h(FILE *_cmm_1P){
return g(_cmm_1P);
}
#define _cmm_0S ((unsigned char*)"aaa.txt")
#define _cmm_1S ((unsigned char*)"r")
void f(){
FILE *_cmm_1L;
FILE *_cmm_2L;
(_cmm_2L=fopen(_cmm_0S,_cmm_1S));
(_cmm_1L=_cmm_2L);
(_cmm_1L=h(_cmm_2L));
}
