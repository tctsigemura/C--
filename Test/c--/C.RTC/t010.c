static int a=1;
static int b=2;
static int c=3;
static _IA _cmm_0T={5,{[4]=0}};
static _IA *x=&_cmm_0T;
static _IA _cmm_1T={5,{[4]=0}};
static _IA _cmm_2T={5,{[4]=0}};
static _IA _cmm_3T={5,{[4]=0}};
static _IA _cmm_4T={5,{[4]=0}};
static _IA _cmm_5T={5,{[4]=0}};
static _RA _cmm_6T={5,{&_cmm_1T,&_cmm_2T,&_cmm_3T,&_cmm_4T,&_cmm_5T}};
static _RA *y=&_cmm_6T;
static _IA *f1(){
return (*_RCA(y,1,"../t010.cmm",13));
}
static void f2(){
(b=(*_ICA((*_RCA(y,1,"../t010.cmm",17)),((a*b)+(c*(*_ICA(f1(),2,"../t010.cmm",17)))),"../t010.cmm",17)));
}
