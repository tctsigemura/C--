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
Node *l;
(l=m);
(l->next=l->prev->prev);
int x;
(x=l->val);
return 0;
}
