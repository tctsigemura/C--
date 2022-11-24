#include <stdio.h>
FILE *g(FILE *f);
static FILE *h(FILE *f){
return g(f);
}
static _CA _cmm_0S={8,{"aaa.txt"}};
static _CA _cmm_1S={2,{"r"}};
void f(){
FILE *f;
FILE *p;
(p=__fOpen(&_cmm_0S,&_cmm_1S));
(f=p);
(f=h(p));
}
