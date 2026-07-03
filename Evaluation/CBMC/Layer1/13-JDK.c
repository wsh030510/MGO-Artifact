#include <pthread.h>
#include <assert.h>

int semaphore = 0;
int w_state = 0; 

void* thread_enter(void* arg) {
    __CPROVER_assume(semaphore == 1);
    assert(w_state == 1);
    return NULL;
}

void* thread_exit(void* arg) {
    semaphore = 1;
    w_state = 1;
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_enter, NULL);
    pthread_create(&t2, NULL, thread_exit, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}