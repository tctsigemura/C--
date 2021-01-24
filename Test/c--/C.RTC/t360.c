void *__AddrAdd(void *a,int i);
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
static int relocateSegment(_IA *relocTable,int tblSize,_IA *symTable,_IA *segment){
{
int i;
(i=0);
for(;(i<tblSize);(i=(i+sizeof(Reloc ))))
{
Reloc *reloc;
(reloc=__AddrAdd(relocTable,i));
int index;
(index=(((((Reloc*)_CP(reloc,"../t360.cmm",26))->type_idx)&16383)*sizeof(Symbol )));
Symbol *symbol;
(symbol=__AddrAdd(symTable,index));
((*_ICA(segment,(((Reloc*)_CP(reloc,"../t360.cmm",28))->addr),"../t360.cmm",28))=(((Symbol*)_CP(symbol,"../t360.cmm",28))->val));
_printf(&_cmm_0S,(((Reloc*)_CP(reloc,"../t360.cmm",29))->addr),(((Symbol*)_CP(symbol,"../t360.cmm",29))->val),index,(*_ICA(segment,(((Reloc*)_CP(reloc,"../t360.cmm",30))->addr),"../t360.cmm",30)));
}
}
return 0;
}
