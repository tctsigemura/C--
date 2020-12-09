static _IA _cmm_0T={2,{[1]=0}};
static _IA _cmm_1T={2,{[1]=0}};
static _RA _cmm_2T={2,{&_cmm_0T,&_cmm_1T}};
static _RA *a=&_cmm_2T;
static _IA _cmm_3T={2,{[1]=0}};
static _IA *b=&_cmm_3T;
typedef struct _T T;
struct _T {
int x;
_IA *a;
T *b;
};
static T *t;
_IA *f();
int __main(){
((*_ICA((*_RCA(a,1,"../t240.cmm",12)),2,"../t240.cmm",12))=2);
((*_ICA((((T*)_CP(t,"../t240.cmm",13))->a),1,"../t240.cmm",13))=2);
((((T*)_CP((((T*)_CP((((T*)_CP(t,"../t240.cmm",14))->b),"../t240.cmm",14))->b),"../t240.cmm",14))->x)=2);
((*_ICA(f(),2,"../t240.cmm",15))=1);
((*_ICA((b=f()),4,"../t240.cmm",16))=2);
return 0;
}
