static int _cmm_0T[2];
static int _cmm_1T[2];
static int *_cmm_2T[]={_cmm_0T,_cmm_1T};
static int **a=_cmm_2T;
static int _cmm_3T[2];
static int *b=_cmm_3T;
typedef struct _T T;
struct _T {
int x;
int *a;
T *b;
};
static T *t;
int *f();
int main(){
(a[1][2]=2);
(t->a[1]=2);
(t->b->b->x=2);
(f()[2]=1);
((b=f())[4]=2);
return 0;
}
