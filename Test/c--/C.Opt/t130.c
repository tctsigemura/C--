static int x=0;
static int y;
struct X {
int a;
int b;
};
static char f(){
int _cmm_1L;
int _cmm_2L;
struct X *_cmm_3L;
{if(((_cmm_1L==_cmm_2L)==(x==y)))return (0xff&_cmm_1L);
}{if((_cmm_3L==_cmm_3L))return (0xff&_cmm_1L);
}(_cmm_1L=(_cmm_2L=(_cmm_1L*2)));
{
((_cmm_1L=0),(_cmm_2L=0));
while((_cmm_1L<10)){
(_cmm_2L=(_cmm_2L*2));
(_cmm_1L=(_cmm_1L+1));
}
}
{
(_cmm_1L=(_cmm_2L=0));
while((_cmm_1L<10)){
(_cmm_2L=(_cmm_2L*2));
(_cmm_1L=(_cmm_1L+1));
}
}
return (0xff&_cmm_1L);
}
