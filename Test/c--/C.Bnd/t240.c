static _IA _cmm_0T={2,{[1]=0}};
static _IA _cmm_1T={2,{[1]=0}};
static _RA _cmm_2T={2,{&_cmm_0T,&_cmm_1T}};
static _RA *a=&_cmm_2T;
static _IA _cmm_3T={2,{[1]=0}};
static _IA *b=&_cmm_3T;
struct T {
int x;
_IA *a;
struct T *b;
};
static struct T *t;
_IA *f();
int main(){
((*_ICA((*_RCA(a,1,__FILE__,__LINE__)),2,__FILE__,__LINE__))=2);
((*_ICA((((struct T*)_CP(t,__FILE__,__LINE__))->a),1,__FILE__,__LINE__))=2);
((((struct T*)_CP((((struct T*)_CP((((struct T*)_CP(t,__FILE__,__LINE__))->b),__FILE__,__LINE__))->b),__FILE__,__LINE__))->x)=2);
((*_ICA(f(),2,__FILE__,__LINE__))=1);
((*_ICA((b=f()),4,__FILE__,__LINE__))=2);
return 0;
}
