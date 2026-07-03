// 6-ompi_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int trk_active = ATOMIC_VAR_INIT(1);
atomic_int trk_status = ATOMIC_VAR_INIT(-1); // -1: ERROR, 0: SUCCESS

void* model_registration_callback(void* arg) {
    atomic_store_explicit(&trk_status, 0, memory_order_relaxed);
    // Vulnerability: no barrier, active=0 may become visible to other thread early
    atomic_store_explicit(&trk_active, 0, memory_order_relaxed);
    return NULL;
}

void* ompi_interlib_declare(void* arg) {
    int active = atomic_load_explicit(&trk_active, memory_order_relaxed);
    if (active == 0) {
        int status = atomic_load_explicit(&trk_status, memory_order_relaxed);
        // Checkpoint: since active is 0 (registration complete), status must be SUCCESS(0)
        assert(status == 0);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, model_registration_callback, NULL);
    pthread_create(&t2, NULL, ompi_interlib_declare, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}