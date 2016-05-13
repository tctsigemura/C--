static int a;
static unsigned char b=1;
static unsigned char _cmm_0T[]={1,0};
static unsigned char *e=_cmm_0T;
struct X {
int a;
unsigned char b;
unsigned char *c;
};
#define _cmm_0S ((unsigned char*)"abc")
static struct X _cmm_1T={1,1,_cmm_0S};
static struct X *h=&_cmm_1T;
#define _cmm_1S ((unsigned char*)"abc")
#define _cmm_2S ((unsigned char*)"def")
static struct X _cmm_2T={1,1,_cmm_1S};
static struct X _cmm_3T={2,0,_cmm_2S};
static struct X *_cmm_4T[]={&_cmm_2T,&_cmm_3T};
static struct X **i=_cmm_4T;
static unsigned char f1(unsigned char _cmm_1P){
(b=((a<0)&&_cmm_1P));
(b=(_cmm_1P&&(a<0)));
(b=0);
(b=1);
while((b==1)){
(b=(!b));
}
{
int _cmm_1L;
(_cmm_1L=0);
while((_cmm_1L==10)){
(_cmm_1L=(_cmm_1L+1));
}
}
return (b==0);
}
