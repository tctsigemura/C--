#include <stdlib.h>
static int a;
static char b=48;
static int d;
static int e;
static int f=10;
static int g=20;
static int *h;
static int _cmm_0T[]={1000,2000,3000};
static int *i=_cmm_0T;
static int *j;
static int **k;
static int _cmm_1T[]={4000,5000};
static int _cmm_2T[]={6000,7000,8000};
static int *_cmm_3T[]={_cmm_1T,_cmm_2T};
static int **l=_cmm_3T;
#define _cmm_0S "aaaa"
static char *n=_cmm_0S;
#define _cmm_1S "abc"
static char _cmm_4T[]={97,98,0};
static char *_cmm_5T[]={_cmm_1S,_cmm_4T};
static char **o=_cmm_5T;
static int *_cmm_6T[10];
static int _cmm_7T[]={6};
static int _cmm_8T[]={7,8};
static int _cmm_9T[]={9};
static int *_cmm_10T[]={_cmm_7T,_cmm_8T,_cmm_9T};
static int **_cmm_11T[]={_cmm_6T,_cmm_10T};
static int ***p=_cmm_11T;
static int _cmm_12T[]={6};
static int _cmm_13T[]={7,8};
static int _cmm_14T[]={9};
static int *_cmm_15T[]={_cmm_12T,_cmm_13T,_cmm_14T};
static int **_cmm_16T[]={_cmm_15T};
static int ***_cmm_17T[]={_cmm_16T,0};
static int ****_cmm_18T[]={_cmm_17T,0};
static int *****pq=_cmm_18T;
#define _cmm_2S "abc"
static void f0(){
int n;
(n=1);
int ****m;
char *a;
(a=_cmm_2S);
(h[1]=1);
(m[0][1][1][3]=2);
}
#define _cmm_3S "ab"
#define _cmm_4S "cd"
static char *_cmm_19T[]={_cmm_3S,_cmm_4S};
static char *_cmm_20T[3];
static char **_cmm_21T[]={_cmm_19T,_cmm_20T};
static char ***q=_cmm_21T;
static int _cmm_22T[]={1,2};
static int _cmm_23T[]={3,4,5};
static int *_cmm_24T[]={_cmm_22T,_cmm_23T};
static int _cmm_25T[]={6};
static int _cmm_26T[]={7,8};
static int _cmm_27T[]={9};
static int *_cmm_28T[]={_cmm_25T,_cmm_26T,_cmm_27T};
static int **_cmm_29T[]={_cmm_24T,_cmm_28T};
static int ***r=_cmm_29T;
static char _cmm_30T[3];
static char *s=_cmm_30T;
static char _cmm_31T[2];
static char _cmm_32T[2];
static char _cmm_33T[2];
static char *_cmm_34T[]={_cmm_31T,_cmm_32T,_cmm_33T};
static char **t=_cmm_34T;
static char _cmm_35T[2];
static char _cmm_36T[2];
static char _cmm_37T[2];
static char _cmm_38T[2];
static char *_cmm_39T[]={_cmm_35T,_cmm_36T};
static char *_cmm_40T[]={_cmm_37T,_cmm_38T};
static char **_cmm_41T[]={_cmm_39T,_cmm_40T};
static char ***u=_cmm_41T;
#define _cmm_5S "ab"
#define _cmm_6S "cd"
static char *_cmm_42T[]={_cmm_5S,_cmm_6S};
static char _cmm_43T[2];
static char _cmm_44T[2];
static char *_cmm_45T[]={_cmm_43T,_cmm_44T};
static char **_cmm_46T[]={_cmm_42T,_cmm_45T};
static char ***v=_cmm_46T;
static int f1(char x,int y){
int a;
(a=1);
int b;
int c;
int *q;
(q=_mAlloc((sizeof(int )*10)));
if((q[0]==0))(q[1]=1);
else (q[1]=0);
{if((q[0]==1))(q[1]=1);
}{if((a==1))(a=2);
}if((a!=0))(a=3);
else (a=4);
if((a!=1)){
(a=5);
(b=6);
}
else {
(a=7);
(b=8);
}
for(;(a==0);)
{
int d;
(d=1);
(d=(d+1));
}
{
(a=0);
for(;(a<3);(a=(a+1)))
(q[a]=a);
}
for(;(((a==2)||(b==2))||(c==2));)
{
{if((c==1))continue;
}for(;(b==2);)
{if((c==3))break;
}{if((c==4))break;
}}
free(q);
return c;
}
static char _cmm_47T[]={1,2};
static char _cmm_48T[]={3,4};
static char *_cmm_49T[]={_cmm_47T,_cmm_48T};
static char **ab=_cmm_49T;
static char *f2(){
int a;
(a=1);
int b;
(b=2);
f1(((a==1)&&(b==1)),b);
return ab[1];
}
static void f3(){
;
;
{
;
for(;;)
;
}
}
static void f4(){
int i;
(i=0);
int j;
(j=1);
for(;(i<10);(i=(i+1)))
(i=((j+1)*2));
}
static int _cmm_50T[5];
static int *x=_cmm_50T;
static int _cmm_51T[5];
static int _cmm_52T[5];
static int _cmm_53T[5];
static int _cmm_54T[5];
static int _cmm_55T[5];
static int *_cmm_56T[]={_cmm_51T,_cmm_52T,_cmm_53T,_cmm_54T,_cmm_55T};
static int **y=_cmm_56T;
static void f5(int a,int b,char *s){
(x[(i[x[1]]+1)]=(i[a]=(i[5]=1)));
(b=y[1][(((a*b)*f2()[2])*x[3])]);
}
typedef struct _X X;
struct _X {
int a;
int b;
X **c;
};
typedef struct _Y Y;
struct _Y {
int a;
int b;
char *c;
};
static X *z1;
static X _cmm_57T={1,2,0};
static X *z2=&_cmm_57T;
static X _cmm_58T={1,2,0};
static X _cmm_59T={3,4,0};
static X *_cmm_60T[]={&_cmm_58T,&_cmm_59T};
static X **z3=_cmm_60T;
#define _cmm_7S "ab"
static Y _cmm_61T={1,2,_cmm_7S};
static Y *z4=&_cmm_61T;
static Y *z5=0;
#define _cmm_8S "abc"
static Y _cmm_62T={1,2,_cmm_8S};
static Y *_cmm_63T[]={0,&_cmm_62T,0};
static Y **z6=_cmm_63T;
static Y *_cmm_64T[3];
static Y **z7=_cmm_64T;
static Y *_cmm_65T[3];
static Y *_cmm_66T[3];
static Y **_cmm_67T[]={_cmm_65T,_cmm_66T};
static Y ***z8=_cmm_67T;
static Y **_cmm_68T[2];
static Y ***z9=_cmm_68T;
static void f6(){
(z1=z2);
(z1=_mAlloc(sizeof(X )));
free(z1);
(z1=z3[3]);
(a=z3[3]->a);
(z1=z3[3]->c[1]);
(a=z2->c[1]->c[1]->a);
(z8[1][1]=_mAlloc(sizeof(Y )));
(z8[1][1]->a=1);
free(z8);
}
X *f7(int a);
X **f8(X *x);
X **f9(X **x);
void f10(char *s,...);
#define _cmm_9S "aaa"
#define _cmm_10S "aaa"
#define _cmm_11S "bbb"
static void f11(int a,...){
int b;
(z1=f7(1));
(a=f7(1)->b);
(z1=f8(z1)[1]);
(z1=f9(z3)[1]);
(z1=0);
f10(_cmm_9S);
f10(_cmm_10S,2,_cmm_11S);
f11(b,a,b,a,b);
(z6[1]=(z6[2]=z6[3]));
((y[1]=y[2])[1]=1);
{
int i;
(i=0);
int j;
(j=0);
for(;(i==j);(i=(i+1)))
(j=(j+1));
}
}
#define _cmm_12S "aaa"
static char *ss=_cmm_12S;
static void f12(){
int *x1;
(x1=_mAlloc((sizeof(int )*4)));
free(x1);
char b;
(b=1);
}
static void f13(){
void *x;
int a;
(a=10);
int b;
(b=18);
int c;
(c=99);
(x=ss);
(z1=x);
}
