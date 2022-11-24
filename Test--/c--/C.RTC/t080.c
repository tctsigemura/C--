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
typedef struct _X X;
struct _X {
int a;
char b;
_CA *c;
};
static _CA _cmm_1S={4,{"abc"}};
static X _cmm_8T={1,1,&_cmm_1S};
static X *h=&_cmm_8T;
static _CA _cmm_2S={4,{"abc"}};
static _CA _cmm_3S={4,{"def"}};
static X _cmm_9T={1,1,&_cmm_2S};
static X _cmm_10T={2,0,&_cmm_3S};
static _RA _cmm_11T={2,{&_cmm_9T,&_cmm_10T}};
static _RA *i=&_cmm_11T;
static void f0();
static char f1(char t){
(b=(t&&(a<0)));
(b=0);
for(;(b==1);)
{
(b=(*_CCA(e,a,"../t080.cmm",29)));
(b=(!b));
}
{
int i;
(i=0);
for(;(i==10);(i=(i+1)))
;
}
return (b==0);
}
