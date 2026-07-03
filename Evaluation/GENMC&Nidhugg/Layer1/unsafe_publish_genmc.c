// unsafe_publish_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

// Normal non-atomic variables, representing payload inside a struct
int p_x = 0;
int p_y = 0;

// Atomic pointer publication signal
_Atomic(int) current_pos = ATOMIC_VAR_INIT(0);

void* thread_writer(void* arg) {
    p_x = 42;
    p_y = 43;
    // Pointer publication lacking release barrier
    atomic_store_explicit(&current_pos, 1, memory_order_relaxed);
    return NULL;
}

void* thread_reader(void* arg) {
    if (atomic_load_explicit(&current_pos, memory_order_relaxed) == 1) {
        // Under weak memory, reading normal variables here triggers strict non-atomic data race
        int read_x = p_x;
        int read_y = p_y;
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