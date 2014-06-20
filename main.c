#include "mtrace.h"
#include "timing.h"
         
int main(int argc, char** argv) {
    println("Hola mundo!");
    void *a = x_malloc(sizeof(double));
    void *b = x_malloc(1204);
    void *c = x_malloc(148);
    //x_free(c);
    x_free(b);
    //x_free(a);
    a = x_realloc(a, 14);
    void *d = x_malloc(58);
    //x_free(d);
    
    memory_state();
    memory_clean();
    //memory_state();
    return 0;
}

