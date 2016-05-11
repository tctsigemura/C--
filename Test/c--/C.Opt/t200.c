static int f();
static void main(){
static int _cmm_1L;
static int _cmm_2L;
static int _cmm_3L;
(_cmm_1L=(_cmm_2L*_cmm_3L));
(_cmm_1L=(_cmm_2L/_cmm_3L));
(_cmm_1L=(_cmm_2L%_cmm_3L));
(_cmm_1L=(f()*_cmm_3L));
(_cmm_1L=(f()/_cmm_3L));
(_cmm_1L=(f()%_cmm_3L));
(_cmm_1L=(_cmm_2L*f()));
(_cmm_1L=(_cmm_2L/f()));
(_cmm_1L=(_cmm_2L%f()));
}
