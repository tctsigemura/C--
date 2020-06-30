void *__AddrAdd(void *_cmm_1P,int _cmm_2P);
#include <stdio.h>
typedef struct _Reloc Reloc;
struct _Reloc {
int addr;
int type_idx;
};
typedef struct _Symbol Symbol;
struct _Symbol {
int type_sidx;
int val;
};
static _CA _cmm_0S={35,{"reloc: %04x -> %04x (%04x) | %04x\n"}};
static int relocateSegment(_IA *_cmm_1P,int _cmm_2P,_IA *_cmm_3P,_IA *_cmm_4P){
{
int _cmm_1L;
(_cmm_1L=0);
for(;(_cmm_1L<_cmm_2P);(_cmm_1L=(_cmm_1L+sizeof(Reloc ))))
{
Reloc *_cmm_2L;
(_cmm_2L=__AddrAdd(_cmm_1P,_cmm_1L));
int _cmm_3L;
(_cmm_3L=(((((Reloc*)_CP(_cmm_2L,__FILE__,__LINE__))->type_idx)&16383)*sizeof(Symbol )));
Symbol *_cmm_4L;
(_cmm_4L=__AddrAdd(_cmm_3P,_cmm_3L));
((*_ICA(_cmm_4P,(((Reloc*)_CP(_cmm_2L,__FILE__,__LINE__))->addr),__FILE__,__LINE__))=(((Symbol*)_CP(_cmm_4L,__FILE__,__LINE__))->val));
_printf(&_cmm_0S,(((Reloc*)_CP(_cmm_2L,__FILE__,__LINE__))->addr),(((Symbol*)_CP(_cmm_4L,__FILE__,__LINE__))->val),_cmm_3L,(*_ICA(_cmm_4P,(((Reloc*)_CP(_cmm_2L,__FILE__,__LINE__))->addr),__FILE__,__LINE__)));
}
}
return 0;
}
