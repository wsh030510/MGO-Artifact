// 17-ceph_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int locked_by = ATOMIC_VAR_INIT(0);
atomic_int nlock = ATOMIC_VAR_INIT(0);

void* thread_lock(void* arg) {
    // Corresponds to _post_lock()
    atomic_store_explicit(&locked_by, 1, memory_order_relaxed);
    atomic_store_explicit(&nlock, 1, memory_order_relaxed);
    return NULL;
}

void* thread_check(void* arg) {
    // Corresponds to is_locked_by_me()
    if (atomic_load_explicit(&nlock, memory_order_relaxed) > 0) {
        // If nlock > 0, lock is held, locked_by must equal 1
        assert(atomic_load_explicit(&locked_by, memory_order_relaxed) == 1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_lock, NULL);
    pthread_create(&t2, NULL, thread_check, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}