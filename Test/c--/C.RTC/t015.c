static int a=1;
int b=2;
static char c=3;
static _CA _cmm_0T={5,{[4]=0}};
_CA *x=&_cmm_0T;
static _CA _cmm_1T={5,{[4]=0}};
static _CA _cmm_2T={5,{[4]=0}};
static _CA _cmm_3T={5,{[4]=0}};
static _CA _cmm_4T={5,{[4]=0}};
static _CA _cmm_5T={5,{[4]=0}};
static _RA _cmm_6T={5,{&_cmm_1T,&_cmm_2T,&_cmm_3T,&_cmm_4T,&_cmm_5T}};
static _RA *y=&_cmm_6T;
static _CA *f1(){
return (*_RCA(y,1,"../t015.cmm",13));
}
static void f2(){
(c=(*_CCA((*_RCA(y,1,"../t015.cmm",17)),((a*b)+(c*(*_CCA(f1(),2,"../t015.cmm",17)))),"../t015.cmm",17)));
}
