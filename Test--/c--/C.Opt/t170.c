static int a;
static char b;
static char c;
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
#define _cmm_0S "aaaa"
static char *n=_cmm_0S;
#define _cmm_1S "abc"
static char _cmm_12T[]={97,98,0};
static char *_cmm_13T[]={_cmm_1S,_cmm_12T};
static char **o=_cmm_13T;
static int *_cmm_14T[10];
static int _cmm_15T[]={6};
static int _cmm_16T[]={7,8};
static int _cmm_17T[]={9};
static int *_cmm_18T[]={_cmm_15T,_cmm_16T,_cmm_17T};
static int **_cmm_19T[]={_cmm_14T,_cmm_18T};
static int ***p=_cmm_19T;
static int *pq;
static void f1(){
int n;
(n=1);
}
#define _cmm_2S "ab"
#define _cmm_3S "cd"
static char *_cmm_20T[]={_cmm_2S,_cmm_3S};
static char *_cmm_21T[3];
static char **_cmm_22T[]={_cmm_20T,_cmm_21T};
static char ***q=_cmm_22T;
#include <stdlib.h>
#define _cmm_4S "ABCDE"
static int f2(int x,int y){
int la;
int lb;
int *lc;
(lc=_mAlloc(10));
{
int ld;
(ld=x);
int le;
(le=x);
}
int le;
(le=30);
char *ls;
(ls=_cmm_4S);
(la=10);
(b=(0xff&la));
(lb=b);
(i[la]=lb);
(la=(lb=10));
(i[la]=(lb=10));
(i[la]=(i[lb]=(lb=20)));
(la=l[1][2]);
(la=l[i[5]][i[la]]);
(l[i[5]][i[la]]=(i[la]=20));
return la;
}
static char f3(){
char x;
(x=1);
return x;
}
static void f4(){
int x;
(x=(a|b));
int y;
(y=((a*b)^2));
int z;
(z=(a&b));
char w;
(w=(a==b));
char v;
(v=((0xff&a)!=b));
char u;
(u=(a<b));
char t;
(t=(a<=b));
char s;
(s=(a>b));
char r;
(r=(a>=b));
int q;
(q=(a<<b));
int p;
(p=(a>>b));
int o;
(o=(a+b));
int n;
(n=(a-b));
int m;
(m=(a*b));
int l;
(l=(a/b));
int k;
(k=(a%b));
int j;
(j=(-a));
int i;
(i=a);
char h;
(h=(!w));
int g;
(g=(~a));
int f;
(f=(((a*b)+6)+(c*d)));
char e;
(e=(w&&v));
(pq[2]=2);
}
