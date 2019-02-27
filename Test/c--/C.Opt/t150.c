typedef struct _Node Node;
struct _Node {
Node *next;
Node *prev;
int val;
};
static Node _cmm_0T={0,0,10};
static Node *n=&_cmm_0T;
static Node *m;
int main(){
(n->next=n);
(n->prev=n);
(m=n->next->prev);
Node *_cmm_1L;
(_cmm_1L=m);
(_cmm_1L->next=_cmm_1L->prev->prev);
int _cmm_2L;
(_cmm_2L=_cmm_1L->val);
return 0;
}
