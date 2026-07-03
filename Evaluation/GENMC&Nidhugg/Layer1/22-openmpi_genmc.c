// 22-openmpi_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int next_ptr = ATOMIC_VAR_INIT(0);
atomic_int item_free = ATOMIC_VAR_INIT(1);

void* thread_push(void* arg) {
    // Corresponds to item->opal_list_next = next;
    atomic_store_explicit(&next_ptr, 1, memory_order_relaxed);
    
    // BUG: missing opal_atomic_wmb()
    // Corresponds to item->item_free = 0;
    atomic_store_explicit(&item_free, 0, memory_order_relaxed);
    return NULL;
}

void* thread_pop(void* arg) {
    // If item is marked as not free (0), it is safe to pop
    if (atomic_load_explicit(&item_free, memory_order_relaxed) == 0) {
        // Then we must see the correct next pointer
        assert(atomic_load_explicit(&next_ptr, memory_order_relaxed) == 1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_push, NULL);
    pthread_create(&t2, NULL, thread_pop, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}