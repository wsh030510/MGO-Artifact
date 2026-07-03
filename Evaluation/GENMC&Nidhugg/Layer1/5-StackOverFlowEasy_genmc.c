// 5-StackOverFlowEasy_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct {
    int x;
    int y;
} Point;

_Atomic(Point*) currentPos = ATOMIC_VAR_INIT(NULL);
Point shared_p; // Use global variable instead of malloc to avoid system library barriers

void* writer_thread(void* arg) {
    shared_p.x = 1;
    shared_p.y = 2;
    // Vulnerability: unsynchronized publication without any lock or barrier
    atomic_store_explicit(&currentPos, &shared_p, memory_order_relaxed);
    return NULL;
}

void* reader_thread(void* arg) {
    Point* p = atomic_load_explicit(&currentPos, memory_order_relaxed);
    if (p != NULL) {
        // Under weak memory, this leads to non-atomic race with writer thread
        int val_x = p->x;
        int val_y = p->y;
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, writer_thread, NULL);
    pthread_create(&t2, NULL, reader_thread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}