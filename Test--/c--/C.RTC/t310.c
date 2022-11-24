void *__ItoA(int i);
typedef struct _TCB TCB;
struct _TCB {
int pc;
int sp;
};
static TCB _cmm_0T={0,20480};
static TCB _cmm_1T={0,32768};
static _RA _cmm_2T={2,{&_cmm_0T,&_cmm_1T}};
static _RA *tcbs=&_cmm_2T;
static void task1(){
;
}
static void task2(){
;
}
static char boole;
int __main(){
_IA *vect;
(vect=__ItoA(65024));
((((TCB*)_CP((*_RCA(tcbs,0,"../t310.cmm",54)),"../t310.cmm",54))->pc)=((int)&task1));
((((TCB*)_CP((*_RCA(tcbs,1,"../t310.cmm",55)),"../t310.cmm",55))->sp)=(((int)&task2)+100));
int b;
(b=(((int)&boole)+10));
return 0;
}
