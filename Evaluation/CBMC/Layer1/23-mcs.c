#include <pthread.h>
#include <assert.h>

int me_locked = 0;
int prev_next = 0;

void *thread_successor_lock(void *arg) {
    me_locked = 1; 
    prev_next = 1;
    return NULL;
}

void *thread_predecessor_unlock(void *arg) {
    if (prev_next == 1) {
        assert(me_locked == 1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_successor_lock, NULL);
    pthread_create(&t2, NULL, thread_predecessor_unlock, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}