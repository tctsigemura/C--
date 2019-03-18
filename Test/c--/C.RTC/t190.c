static _IA _cmm_0T={10,{[9]=0}};
static _IA *a=&_cmm_0T;
typedef struct _X X;
struct _X {
int a;
int b;
};
int main(){
int _cmm_1L;
int _cmm_2L;
X *_cmm_3L;
(_cmm_1L=_cmm_1L);
(_cmm_1L=(_cmm_1L+1));
(_cmm_2L=_cmm_2L);
(_cmm_2L=(_cmm_2L+-1));
(_cmm_1L=(*_ICA(a,0,__FILE__,__LINE__)));
(_cmm_2L=(*_ICA(a,1,__FILE__,__LINE__)));
(_cmm_1L=(((X*)_CP(_cmm_3L,__FILE__,__LINE__))->a));
(_cmm_2L=(((X*)_CP(_cmm_3L,__FILE__,__LINE__))->b));
return 0;
}
