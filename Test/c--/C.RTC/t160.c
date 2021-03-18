#include <stdlib.h>
static int a;
static char b=48;
static int d;
static int e;
static int f=10;
static int g=20;
static _IA *h;
static _IA _cmm_0T={3,{1000,2000,3000}};
static _IA *i=&_cmm_0T;
static _IA *j;
static _RA *k;
static _IA _cmm_1T={2,{4000,5000}};
static _IA _cmm_2T={3,{6000,7000,8000}};
static _RA _cmm_3T={2,{&_cmm_1T,&_cmm_2T}};
static _RA *l=&_cmm_3T;
static _CA _cmm_0S={5,{"aaaa"}};
static _CA *n=&_cmm_0S;
static _CA _cmm_1S={4,{"abc"}};
static _CA _cmm_4T={3,{97,98,0}};
static _RA _cmm_5T={2,{&_cmm_1S,&_cmm_4T}};
static _RA *o=&_cmm_5T;
static _RA _cmm_6T={10,{[9]=0}};
static _IA _cmm_7T={1,{6}};
static _IA _cmm_8T={2,{7,8}};
static _IA _cmm_9T={1,{9}};
static _RA _cmm_10T={3,{&_cmm_7T,&_cmm_8T,&_cmm_9T}};
static _RA _cmm_11T={2,{&_cmm_6T,&_cmm_10T}};
static _RA *p=&_cmm_11T;
static _IA _cmm_12T={1,{6}};
static _IA _cmm_13T={2,{7,8}};
static _IA _cmm_14T={1,{9}};
static _RA _cmm_15T={3,{&_cmm_12T,&_cmm_13T,&_cmm_14T}};
static _RA _cmm_16T={1,{&_cmm_15T}};
static _RA _cmm_17T={2,{&_cmm_16T,0}};
static _RA _cmm_18T={2,{&_cmm_17T,0}};
static _RA *pq=&_cmm_18T;
static _CA _cmm_2S={4,{"abc"}};
static void f0(){
int n;
(n=1);
_RA *m;
_CA *a;
(a=&_cmm_2S);
((*_ICA(h,1,"../t160.cmm",26))=1);
((*_ICA((*_RCA((*_RCA((*_RCA(m,0,"../t160.cmm",27)),1,"../t160.cmm",27)),1,"../t160.cmm",27)),3,"../t160.cmm",27))=2);
}
static _CA _cmm_3S={3,{"ab"}};
static _CA _cmm_4S={3,{"cd"}};
static _RA _cmm_19T={2,{&_cmm_3S,&_cmm_4S}};
static _RA _cmm_20T={3,{[2]=0}};
static _RA _cmm_21T={2,{&_cmm_19T,&_cmm_20T}};
static _RA *q=&_cmm_21T;
static _IA _cmm_22T={2,{1,2}};
static _IA _cmm_23T={3,{3,4,5}};
static _RA _cmm_24T={2,{&_cmm_22T,&_cmm_23T}};
static _IA _cmm_25T={1,{6}};
static _IA _cmm_26T={2,{7,8}};
static _IA _cmm_27T={1,{9}};
static _RA _cmm_28T={3,{&_cmm_25T,&_cmm_26T,&_cmm_27T}};
static _RA _cmm_29T={2,{&_cmm_24T,&_cmm_28T}};
static _RA *r=&_cmm_29T;
static _CA _cmm_30T={3,{[2]=0}};
static _CA *s=&_cmm_30T;
static _CA _cmm_31T={2,{[1]=0}};
static _CA _cmm_32T={2,{[1]=0}};
static _CA _cmm_33T={2,{[1]=0}};
static _RA _cmm_34T={3,{&_cmm_31T,&_cmm_32T,&_cmm_33T}};
static _RA *t=&_cmm_34T;
static _CA _cmm_35T={2,{[1]=0}};
static _CA _cmm_36T={2,{[1]=0}};
static _CA _cmm_37T={2,{[1]=0}};
static _CA _cmm_38T={2,{[1]=0}};
static _RA _cmm_39T={2,{&_cmm_35T,&_cmm_36T}};
static _RA _cmm_40T={2,{&_cmm_37T,&_cmm_38T}};
static _RA _cmm_41T={2,{&_cmm_39T,&_cmm_40T}};
static _RA *u=&_cmm_41T;
static _CA _cmm_5S={3,{"ab"}};
static _CA _cmm_6S={3,{"cd"}};
static _RA _cmm_42T={2,{&_cmm_5S,&_cmm_6S}};
static _CA _cmm_43T={2,{[1]=0}};
static _CA _cmm_44T={2,{[1]=0}};
static _RA _cmm_45T={2,{&_cmm_43T,&_cmm_44T}};
static _RA _cmm_46T={2,{&_cmm_42T,&_cmm_45T}};
static _RA *v=&_cmm_46T;
static int f1(char x,int y){
int a;
(a=1);
int b;
int c;
_IA *q;
(q=_mAlloc((sizeof(int )*10)));
if(((*_ICA(q,0,"../t160.cmm",43))==0))((*_ICA(q,1,"../t160.cmm",43))=1);
else ((*_ICA(q,1,"../t160.cmm",43))=0);
{if(((*_ICA(q,0,"../t160.cmm",44))==1))((*_ICA(q,1,"../t160.cmm",44))=1);
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
((*_ICA(q,a,"../t160.cmm",59))=a);
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
static _CA _cmm_47T={2,{1,2}};
static _CA _cmm_48T={2,{3,4}};
static _RA _cmm_49T={2,{&_cmm_47T,&_cmm_48T}};
static _RA *ab=&_cmm_49T;
static _CA *f2(){
int a;
(a=1);
int b;
(b=2);
f1(((a==1)&&(b==1)),b);
return (*_RCA(ab,1,"../t160.cmm",78));
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
static _IA _cmm_50T={5,{[4]=0}};
static _IA *x=&_cmm_50T;
static _IA _cmm_51T={5,{[4]=0}};
static _IA _cmm_52T={5,{[4]=0}};
static _IA _cmm_53T={5,{[4]=0}};
static _IA _cmm_54T={5,{[4]=0}};
static _IA _cmm_55T={5,{[4]=0}};
static _RA _cmm_56T={5,{&_cmm_51T,&_cmm_52T,&_cmm_53T,&_cmm_54T,&_cmm_55T}};
static _RA *y=&_cmm_56T;
static void f5(int a,int b,_CA *s){
((*_ICA(x,((*_ICA(i,(*_ICA(x,1,"../t160.cmm",95)),"../t160.cmm",95))+1),"../t160.cmm",95))=((*_ICA(i,a,"../t160.cmm",95))=((*_ICA(i,5,"../t160.cmm",95))=1)));
(b=(*_ICA((*_RCA(y,1,"../t160.cmm",96)),(((a*b)*(*_CCA(f2(),2,"../t160.cmm",96)))*(*_ICA(x,3,"../t160.cmm",96))),"../t160.cmm",96)));
}
typedef struct _X X;
struct _X {
int a;
int b;
_RA *c;
};
typedef struct _Y Y;
struct _Y {
int a;
int b;
_CA *c;
};
static X *z1;
static X _cmm_57T={1,2,0};
static X *z2=&_cmm_57T;
static X _cmm_58T={1,2,0};
static X _cmm_59T={3,4,0};
static _RA _cmm_60T={2,{&_cmm_58T,&_cmm_59T}};
static _RA *z3=&_cmm_60T;
static _CA _cmm_7S={3,{"ab"}};
static Y _cmm_61T={1,2,&_cmm_7S};
static Y *z4=&_cmm_61T;
static Y *z5=0;
static _CA _cmm_8S={4,{"abc"}};
static Y _cmm_62T={1,2,&_cmm_8S};
static _RA _cmm_63T={3,{0,&_cmm_62T,0}};
static _RA *z6=&_cmm_63T;
static _RA _cmm_64T={3,{[2]=0}};
static _RA *z7=&_cmm_64T;
static _RA _cmm_65T={3,{[2]=0}};
static _RA _cmm_66T={3,{[2]=0}};
static _RA _cmm_67T={2,{&_cmm_65T,&_cmm_66T}};
static _RA *z8=&_cmm_67T;
static _RA _cmm_68T={2,{[1]=0}};
static _RA *z9=&_cmm_68T;
static void f6(){
(z1=z2);
(z1=_mAlloc(sizeof(X )));
free(z1);
(z1=(*_RCA(z3,3,"../t160.cmm",123)));
(a=(((X*)_CP((*_RCA(z3,3,"../t160.cmm",124)),"../t160.cmm",124))->a));
(z1=(*_RCA((((X*)_CP((*_RCA(z3,3,"../t160.cmm",125)),"../t160.cmm",125))->c),1,"../t160.cmm",125)));
(a=(((X*)_CP((*_RCA((((X*)_CP((*_RCA((((X*)_CP(z2,"../t160.cmm",126))->c),1,"../t160.cmm",126)),"../t160.cmm",126))->c),1,"../t160.cmm",126)),"../t160.cmm",126))->a));
((*_RCA((*_RCA(z8,1,"../t160.cmm",127)),1,"../t160.cmm",127))=_mAlloc(sizeof(Y )));
((((Y*)_CP((*_RCA((*_RCA(z8,1,"../t160.cmm",128)),1,"../t160.cmm",128)),"../t160.cmm",128))->a)=1);
free(z8);
}
X *f7(int a);
_RA *f8(X *x);
_RA *f9(_RA *x);
void f10(_CA *s,...);
static _CA _cmm_9S={4,{"aaa"}};
static _CA _cmm_10S={4,{"aaa"}};
static _CA _cmm_11S={4,{"bbb"}};
static void f11(int a,...){
int b;
(z1=f7(1));
(a=(((X*)_CP(f7(1),"../t160.cmm",141))->b));
(z1=(*_RCA(f8(z1),1,"../t160.cmm",142)));
(z1=(*_RCA(f9(z3),1,"../t160.cmm",143)));
(z1=0);
f10(&_cmm_9S);
f10(&_cmm_10S,2,&_cmm_11S);
f11(b,a,b,a,b);
((*_RCA(z6,1,"../t160.cmm",148))=((*_RCA(z6,2,"../t160.cmm",148))=(*_RCA(z6,3,"../t160.cmm",148))));
((*_ICA(((*_RCA(y,1,"../t160.cmm",149))=(*_RCA(y,2,"../t160.cmm",149))),1,"../t160.cmm",149))=1);
{
int i;
(i=0);
int j;
(j=0);
for(;(i==j);(i=(i+1)))
(j=(j+1));
}
}
static _CA _cmm_12S={4,{"aaa"}};
static _CA *ss=&_cmm_12S;
static void f12(){
_IA *x1;
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
