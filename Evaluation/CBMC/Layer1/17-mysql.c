#include <pthread.h>
#include <assert.h>

int data = 0;
int lock_released = 0;

void* thread_exit_lock(void* arg) {
    data = 1;
    lock_released = 1; 
    return NULL;
}

void* thread_enter_lock(void* arg) {
    if (lock_released == 1) {
        assert(data == 1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_exit_lock, NULL);
    pthread_create(&t2, NULL, thread_enter_lock, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}