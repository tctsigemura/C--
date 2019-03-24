static int a;
static char b=1;
static _CA _cmm_0T={2,{1,0}};
static _CA *e=&_cmm_0T;
typedef struct _X X;
struct _X {
int a;
char b;
_CA *c;
};
static _CA _cmm_0S={4,{"abc"}};
static X _cmm_1T={1,1,&_cmm_0S};
static X *h=&_cmm_1T;
static _CA _cmm_1S={4,{"abc"}};
static _CA _cmm_2S={4,{"def"}};
static X _cmm_2T={1,1,&_cmm_1S};
static X _cmm_3T={2,0,&_cmm_2S};
static _RA _cmm_4T={2,{&_cmm_2T,&_cmm_3T}};
static _RA *i=&_cmm_4T;
static char f1(char _cmm_1P){
(b=((a<0)&&_cmm_1P));
(b=(_cmm_1P&&(a<0)));
(b=0);
(b=1);
for(;(b==1);)
(b=(!b));
{
int _cmm_1L;
(_cmm_1L=0);
for(;(_cmm_1L==10);(_cmm_1L=(_cmm_1L+1)))
;
}
return (b==0);
}
