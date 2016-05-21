static int a;
static unsigned char b;
static unsigned char c;
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
static int _cmm_4T[]={1,2};
static int _cmm_5T[]={3,4,5};
static int *_cmm_6T[]={_cmm_4T,_cmm_5T};
static int _cmm_7T[]={6};
static int _cmm_8T[]={7,8};
static int _cmm_9T[]={9};
static int *_cmm_10T[]={_cmm_7T,_cmm_8T,_cmm_9T};
static int **_cmm_11T[]={_cmm_6T,_cmm_10T};
static int ***m=_cmm_11T;
#define _cmm_0S ((unsigned char*)"aaaa")
static unsigned char *n=_cmm_0S;
#define _cmm_1S ((unsigned char*)"abc")
static unsigned char _cmm_12T[]={97,98,0};
static unsigned char *_cmm_13T[]={_cmm_1S,_cmm_12T};
static unsigned char **o=_cmm_13T;
static int *_cmm_14T[10];
static int _cmm_15T[]={6};
static int _cmm_16T[]={7,8};
static int _cmm_17T[]={9};
static int *_cmm_18T[]={_cmm_15T,_cmm_16T,_cmm_17T};
static int **_cmm_19T[]={_cmm_14T,_cmm_18T};
static int ***p=_cmm_19T;
static int *pq;
static void f1(){
int _cmm_1L;
(_cmm_1L=1);
}
#define _cmm_2S ((unsigned char*)"ab")
#define _cmm_3S ((unsigned char*)"cd")
static unsigned char *_cmm_20T[]={_cmm_2S,_cmm_3S};
static unsigned char *_cmm_21T[3];
static unsigned char **_cmm_22T[]={_cmm_20T,_cmm_21T};
static unsigned char ***q=_cmm_22T;
#include <stdlib.h>
#define _cmm_4S ((unsigned char*)"ABCDE")
static int f2(int _cmm_1P,int _cmm_2P){
int _cmm_1L;
int _cmm_2L;
int *_cmm_3L;
(_cmm_3L=malloc(10));
{
int _cmm_4L;
(_cmm_4L=_cmm_1P);
int _cmm_5L;
(_cmm_5L=_cmm_1P);
}
int _cmm_4L;
(_cmm_4L=30);
unsigned char *_cmm_5L;
(_cmm_5L=_cmm_4S);
(_cmm_1L=10);
(b=(0xff&_cmm_1L));
(_cmm_2L=b);
(i[_cmm_1L]=_cmm_2L);
(_cmm_1L=(_cmm_2L=10));
(i[_cmm_1L]=(_cmm_2L=10));
(i[_cmm_1L]=(i[_cmm_2L]=(_cmm_2L=20)));
(_cmm_1L=l[1][2]);
(_cmm_1L=l[i[5]][i[_cmm_1L]]);
(l[i[5]][i[_cmm_1L]]=(i[_cmm_1L]=20));
return _cmm_1L;
}
static unsigned char f3(){
unsigned char _cmm_1L;
(_cmm_1L=1);
return _cmm_1L;
}
static void f4(){
int _cmm_1L;
(_cmm_1L=(a|b));
int _cmm_2L;
(_cmm_2L=((a*b)^2));
int _cmm_3L;
(_cmm_3L=(a&b));
unsigned char _cmm_4L;
(_cmm_4L=(a==b));
unsigned char _cmm_5L;
(_cmm_5L=((0xff&a)!=b));
unsigned char _cmm_6L;
(_cmm_6L=(a<b));
unsigned char _cmm_7L;
(_cmm_7L=(a<=b));
unsigned char _cmm_8L;
(_cmm_8L=(a>b));
unsigned char _cmm_9L;
(_cmm_9L=(a>=b));
int _cmm_10L;
(_cmm_10L=(a<<b));
int _cmm_11L;
(_cmm_11L=(a>>b));
int _cmm_12L;
(_cmm_12L=(a+b));
int _cmm_13L;
(_cmm_13L=(a-b));
int _cmm_14L;
(_cmm_14L=(a*b));
int _cmm_15L;
(_cmm_15L=(a/b));
int _cmm_16L;
(_cmm_16L=(a%b));
int _cmm_17L;
(_cmm_17L=(-a));
int _cmm_18L;
(_cmm_18L=a);
unsigned char _cmm_19L;
(_cmm_19L=(!_cmm_4L));
int _cmm_20L;
(_cmm_20L=(~a));
int _cmm_21L;
(_cmm_21L=(((a*b)+6)+(c*d)));
unsigned char _cmm_22L;
(_cmm_22L=(_cmm_4L&&_cmm_5L));
(pq[2]=2);
}
