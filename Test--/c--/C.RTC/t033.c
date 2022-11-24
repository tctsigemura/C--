int f1();
_IA *f2();
static int a=1;
static int b=2;
static _IA *c;
static _RA *d;
typedef struct _X X;
struct _X {
int b;
int a;
};
static X *e;
static void f3(){
((*_ICA(c,1,"../t033.cmm",18))=(b=((a*b)+(*_ICA((*_RCA(d,(5+(((X*)_CP(e,"../t033.cmm",18))->a)),"../t033.cmm",18)),(((((*_ICA(c,4,"../t033.cmm",18))+5)+(*_ICA(c,6,"../t033.cmm",18)))+f1())+b),"../t033.cmm",18)))));
}
