static int a=1;
static char b=1;
static _CA _cmm_0S={4,{"abc"}};
static _CA *c=&_cmm_0S;
static _IA _cmm_0T={3,{1,2,3}};
static _IA *d=&_cmm_0T;
static _CA _cmm_1T={2,{1,0}};
static _CA *e=&_cmm_1T;
static _IA _cmm_2T={2,{1,2}};
static _IA _cmm_3T={2,{3,4}};
static _RA _cmm_4T={2,{&_cmm_2T,&_cmm_3T}};
static _RA *f=&_cmm_4T;
static _CA _cmm_5T={2,{1,0}};
static _CA _cmm_6T={2,{1,0}};
static _RA _cmm_7T={2,{&_cmm_5T,&_cmm_6T}};
static _RA *g=&_cmm_7T;
struct X {
int a;
char b;
_CA *c;
};
static _CA _cmm_1S={4,{"abc"}};
static struct X _cmm_8T={1,1,&_cmm_1S};
static struct X *h=&_cmm_8T;
static _CA _cmm_2S={4,{"abc"}};
static _CA _cmm_3S={4,{"def"}};
static struct X _cmm_9T={1,1,&_cmm_2S};
static struct X _cmm_10T={2,0,&_cmm_3S};
static _RA _cmm_11T={2,{&_cmm_9T,&_cmm_10T}};
static _RA *i=&_cmm_11T;
static void f0();
static char f1(char _cmm_1P){
(b=(_cmm_1P&&(a<0)));
(b=0);
for(;(b==1);)
{
(b=(*_CCA(e,a,__FILE__,__LINE__)));
(b=(!b));
}
{
int _cmm_1L;
(_cmm_1L=0);
for(;(_cmm_1L==10);(_cmm_1L=(_cmm_1L+1)))
;
}
return (b==0);
}
