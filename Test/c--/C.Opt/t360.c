static void *__AddrAdd(void *_cmm_1P,int _cmm_2P);
static int printf(char *_cmm_1P,...);
struct Reloc {
int addr;
int type_idx;
};
struct Symbol {
int type_sidx;
int val;
};
#define _cmm_0S "reloc: %04x -> %04x (%04x) | %04x\n"
static int relocateSegment(int *_cmm_1P,int _cmm_2P,int *_cmm_3P,int *_cmm_4P){
{
int _cmm_1L;
(_cmm_1L=0);
while((_cmm_1L<_cmm_2P)){
{
struct Reloc *_cmm_2L;
(_cmm_2L=__AddrAdd(_cmm_1P,_cmm_1L));
int _cmm_3L;
(_cmm_3L=((_cmm_2L->type_idx&16383)*8));
struct Symbol *_cmm_4L;
(_cmm_4L=__AddrAdd(_cmm_3P,_cmm_3L));
(_cmm_4P[_cmm_2L->addr]=_cmm_4L->val);
printf(_cmm_0S,_cmm_2L->addr,_cmm_4L->val,_cmm_3L,_cmm_4P[_cmm_2L->addr]);
}
(_cmm_1L=(_cmm_1L+8));
}
}
return 0;
}
