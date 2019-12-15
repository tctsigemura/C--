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
((((Node*)_CP(n,__FILE__,__LINE__))->next)=n);
((((Node*)_CP(n,__FILE__,__LINE__))->prev)=n);
(m=(((Node*)_CP((((Node*)_CP(n,__FILE__,__LINE__))->next),__FILE__,__LINE__))->prev));
Node *_cmm_1L;
(_cmm_1L=m);
((((Node*)_CP(_cmm_1L,__FILE__,__LINE__))->next)=(((Node*)_CP((((Node*)_CP(_cmm_1L,__FILE__,__LINE__))->prev),__FILE__,__LINE__))->prev));
int _cmm_2L;
(_cmm_2L=(((Node*)_CP(_cmm_1L,__FILE__,__LINE__))->val));
return 0;
}
