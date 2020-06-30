#include <stdio.h>
FILE *g(FILE *_cmm_1P);
static FILE *h(FILE *_cmm_1P){
return g(_cmm_1P);
}
static _CA _cmm_0S={8,{"aaa.txt"}};
static _CA _cmm_1S={2,{"r"}};
void f(){
FILE *_cmm_1L;
FILE *_cmm_2L;
(_cmm_2L=__fOpen(&_cmm_0S,&_cmm_1S));
(_cmm_1L=_cmm_2L);
(_cmm_1L=h(_cmm_2L));
}
