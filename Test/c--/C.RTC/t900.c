static char c;
static int i;
static void f(char c){
;
}
static char g(){
f((0xff&i));
(c=(0xff&(i+1)));
return (0xff&i);
}
