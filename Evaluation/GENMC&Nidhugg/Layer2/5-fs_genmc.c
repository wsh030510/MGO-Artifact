// 5-fs_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int fd_valid = 1;
atomic_int error = 0;

void *thread_race_close(void *arg) {
    atomic_store_explicit(&fd_valid, 0, memory_order_relaxed);
    return NULL;
}

void *thread_race_fcntl(void *arg) {
    int v = atomic_load_explicit(&fd_valid, memory_order_relaxed);
    if (v == 0) {
        atomic_store_explicit(&error, 1, memory_order_relaxed);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_race_close, NULL);
    pthread_create(&t2, NULL, thread_race_fcntl, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    if (atomic_load_explicit(&error, memory_order_relaxed)) assert(0);
    return 0;
}