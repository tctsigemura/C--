void *__ItoA(int _cmm_1P);
struct TCB {
int pc;
int sp;
};
static struct TCB _cmm_0T={0,20480};
static struct TCB _cmm_1T={0,32768};
static struct TCB *_cmm_2T[]={&_cmm_0T,&_cmm_1T};
static struct TCB **tcbs=_cmm_2T;
static void task1(){
}
static void task2(){
}
static unsigned char boole;
void main(){
int *_cmm_1L;
(_cmm_1L=__ItoA(65024));
(tcbs[0]->pc=((int)&task1));
(tcbs[1]->sp=(((int)&task2)+100));
int _cmm_2L;
(_cmm_2L=(((int)&boole)+10));
}
