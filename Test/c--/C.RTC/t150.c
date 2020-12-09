typedef struct _Node Node;
struct _Node {
Node *next;
Node *prev;
int val;
};
static Node _cmm_0T={0,0,10};
static Node *n=&_cmm_0T;
static Node *m;
int __main(){
((((Node*)_CP(n,"../t150.cmm",12))->next)=n);
((((Node*)_CP(n,"../t150.cmm",13))->prev)=n);
(m=(((Node*)_CP((((Node*)_CP(n,"../t150.cmm",14))->next),"../t150.cmm",14))->prev));
Node *_cmm_1L;
(_cmm_1L=m);
((((Node*)_CP(_cmm_1L,"../t150.cmm",17))->next)=(((Node*)_CP((((Node*)_CP(_cmm_1L,"../t150.cmm",17))->prev),"../t150.cmm",17))->prev));
int _cmm_2L;
(_cmm_2L=(((Node*)_CP(_cmm_1L,"../t150.cmm",18))->val));
return 0;
}
