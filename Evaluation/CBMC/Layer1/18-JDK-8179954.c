#include <pthread.h>
#include <assert.h>

int x = 0;
int y = 0;
int r1 = 0;
int r2 = 0;

void* thread1_func(void* arg) {
    x = 1;  
    r1 = y;  
    return NULL;
}

void* thread2_func(void* arg) {
    y = 1;  
    r2 = x;  
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1_func, NULL);
    pthread_create(&t2, NULL, thread2_func, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    assert(!(r1 == 0 && r2 == 0)); 
    
    return 0;
}