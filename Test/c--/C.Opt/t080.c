static int a=1;
static char b=1;
#define _cmm_0S "abc"
static char *c=_cmm_0S;
static int _cmm_0T[]={1,2,3};
static int *d=_cmm_0T;
static char _cmm_1T[]={1,0};
static char *e=_cmm_1T;
static int _cmm_2T[]={1,2};
static int _cmm_3T[]={3,4};
static int *_cmm_4T[]={_cmm_2T,_cmm_3T};
static int **f=_cmm_4T;
static char _cmm_5T[]={1,0};
static char _cmm_6T[]={1,0};
static char *_cmm_7T[]={_cmm_5T,_cmm_6T};
static char **g=_cmm_7T;
typedef struct _X X;
struct _X {
int a;
char b;
char *c;
};
#define _cmm_1S "abc"
static X _cmm_8T={1,1,_cmm_1S};
static X *h=&_cmm_8T;
#define _cmm_2S "abc"
#define _cmm_3S "def"
static X _cmm_9T={1,1,_cmm_2S};
static X _cmm_10T={2,0,_cmm_3S};
static X *_cmm_11T[]={&_cmm_9T,&_cmm_10T};
static X **i=_cmm_11T;
static void f0();
static char f1(char _cmm_1P){
(b=(_cmm_1P&&(a<0)));
(b=0);
for(;(b==1);)
{
(b=e[a]);
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
