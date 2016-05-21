static int a=1;
int b=2;
static unsigned char c=3;
static unsigned char _cmm_0T[5];
unsigned char *x=_cmm_0T;
static unsigned char _cmm_1T[5];
static unsigned char _cmm_2T[5];
static unsigned char _cmm_3T[5];
static unsigned char _cmm_4T[5];
static unsigned char _cmm_5T[5];
static unsigned char *_cmm_6T[]={_cmm_1T,_cmm_2T,_cmm_3T,_cmm_4T,_cmm_5T};
static unsigned char **y=_cmm_6T;
static unsigned char *f1(){
return y[1];
}
static void f2(){
(c=y[1][((a*b)+(c*f1()[2]))]);
}
