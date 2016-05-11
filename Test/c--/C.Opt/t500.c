static FILE *fopen();
static FILE *g(FILE *_cmm_1P);
static FILE *h(FILE *_cmm_1P){
return g(_cmm_1P);
}
static void f(){
static FILE *_cmm_1L;
static FILE *_cmm_2L;
(_cmm_2L=fopen());
(_cmm_1L=_cmm_2L);
(_cmm_1L=g(_cmm_2L));
}
