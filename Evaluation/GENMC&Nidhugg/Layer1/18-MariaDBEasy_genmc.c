// 18-MariaDBEasy_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int cursor_key = ATOMIC_VAR_INIT(1);
atomic_int cursor_link = ATOMIC_VAR_INIT(1);

void* thread_l_delete(void* arg) {
    // Simulate reordering in add_to_purgatory
    atomic_store_explicit(&cursor_key, 0, memory_order_relaxed);
    atomic_store_explicit(&cursor_link, 0, memory_order_relaxed);
    return NULL;
}

void* thread_l_find(void* arg) {
    // Simulate optimistic read in l_find
    int key = atomic_load_explicit(&cursor_key, memory_order_relaxed);
    int link = atomic_load_explicit(&cursor_link, memory_order_relaxed);
    
    if (link == 1) {
        // If link still valid, key must not be dirty data 0
        assert(key == 1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_l_delete, NULL);
    pthread_create(&t2, NULL, thread_l_find, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}