int f1();
static int a=1;
static int b=2;
static int c=3;
static int _cmm_0T[5];
static int *x=_cmm_0T;
static int _cmm_1T[5];
static int _cmm_2T[5];
static int _cmm_3T[5];
static int _cmm_4T[5];
static int _cmm_5T[5];
static int *_cmm_6T[]={_cmm_1T,_cmm_2T,_cmm_3T,_cmm_4T,_cmm_5T};
static int **y=_cmm_6T;
static void f2(){
(b=y[1][((a*b)+(c*f1()))]);
}
