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
#define _cmm_0S "reloc: %04x -> %04x (%04x) | %04x\n"
static int relocateSegment(int *relocTable,int tblSize,int *symTable,int *segment){
{
int i;
(i=0);
for(;(i<tblSize);(i=(i+sizeof(Reloc ))))
{
Reloc *reloc;
(reloc=__AddrAdd(relocTable,i));
int index;
(index=((reloc->type_idx&16383)*sizeof(Symbol )));
Symbol *symbol;
(symbol=__AddrAdd(symTable,index));
(segment[reloc->addr]=symbol->val);
_printf(_cmm_0S,reloc->addr,symbol->val,index,segment[reloc->addr]);
}
}
return 0;
}
