struct Node {
struct Node *next;
struct Node *prev;
int val;
};
static struct Node _cmm_0T={0,0,10};
static struct Node *n=&_cmm_0T;
static struct Node *m;
static void main(){
(n->next=n);
(n->prev=n);
(m=n->next->prev);
static struct Node *_cmm_1L;
(_cmm_1L=m);
(_cmm_1L->next=_cmm_1L->prev->prev);
static int _cmm_2L;
(_cmm_2L=_cmm_1L->val);
}
