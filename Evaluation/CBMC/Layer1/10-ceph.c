#include <pthread.h>
#include <assert.h>

int nlock = 0;
int locked_by = 0;

void* lock_thread(void* arg) {
    locked_by = 1;
    __sync_synchronize(); 
    nlock = 1;
    return NULL;
}

void* check_thread(void* arg) {
    if (nlock > 0) {
        assert(locked_by == 1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, lock_thread, NULL);
    pthread_create(&t2, NULL, check_thread, NULL);
    return 0;
}