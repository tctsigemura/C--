struct Node {
struct Node *next;
struct Node *prev;
int val;
};
static struct Node _cmm_0T={0,0,10};
static struct Node *n=&_cmm_0T;
static struct Node *m;
int main(){
((((struct Node*)_CP(n,__FILE__,__LINE__))->next)=n);
((((struct Node*)_CP(n,__FILE__,__LINE__))->prev)=n);
(m=(((struct Node*)_CP((((struct Node*)_CP(n,__FILE__,__LINE__))->next),__FILE__,__LINE__))->prev));
struct Node *_cmm_1L;
(_cmm_1L=m);
((((struct Node*)_CP(_cmm_1L,__FILE__,__LINE__))->next)=(((struct Node*)_CP((((struct Node*)_CP(_cmm_1L,__FILE__,__LINE__))->prev),__FILE__,__LINE__))->prev));
int _cmm_2L;
(_cmm_2L=(((struct Node*)_CP(_cmm_1L,__FILE__,__LINE__))->val));
return 0;
}
