#include <stdio.h>
FILE *g(FILE *f);
static FILE *h(FILE *f){
return g(f);
}
#define _cmm_0S "aaa.txt"
#define _cmm_1S "r"
void f(){
FILE *f;
FILE *p;
(p=_fOpen(_cmm_0S,_cmm_1S));
(f=p);
(f=h(p));
}
