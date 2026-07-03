// mysql_rwlock_deadlock_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int writer_thread_id = ATOMIC_VAR_INIT(0);
atomic_int recursive_flag = ATOMIC_VAR_INIT(0);

void* thread_writer(void* arg) {
    atomic_store_explicit(&writer_thread_id, 1001, memory_order_relaxed);
    atomic_store_explicit(&recursive_flag, 1, memory_order_relaxed);
    return NULL;
}

void* thread_reader(void* arg) {
    // Reader first reads recursive_flag == 1
    if (atomic_load_explicit(&recursive_flag, memory_order_relaxed) == 1) {
        // Due to reordering, it may read old writer_thread_id (0)
        assert(atomic_load_explicit(&writer_thread_id, memory_order_relaxed) == 1001);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_writer, NULL);
    pthread_create(&t2, NULL, thread_reader, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}