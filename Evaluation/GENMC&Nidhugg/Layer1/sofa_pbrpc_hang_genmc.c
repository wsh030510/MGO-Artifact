// sofa_pbrpc_hang_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int pending_count = ATOMIC_VAR_INIT(0);
atomic_int send_token = ATOMIC_VAR_INIT(1); // 1=busy

atomic_int r1 = ATOMIC_VAR_INIT(0);
atomic_int r2 = ATOMIC_VAR_INIT(0);

void* thread_try_send(void* arg) {
    // Write count
    atomic_store_explicit(&pending_count, 1, memory_order_relaxed);
    // Read token
    atomic_store_explicit(&r1, atomic_load_explicit(&send_token, memory_order_relaxed), memory_order_relaxed);
    return NULL;
}

void* thread_get_queue(void* arg) {
    // Write token (release)
    atomic_store_explicit(&send_token, 0, memory_order_relaxed);
    // Read count
    atomic_store_explicit(&r2, atomic_load_explicit(&pending_count, memory_order_relaxed), memory_order_relaxed);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_try_send, NULL);
    pthread_create(&t2, NULL, thread_get_queue, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    // SB pattern core assertion: if store-load reordering occurs,
    // T1 sees token=1 (old), T2 sees count=0 (old), causing message hang
    assert(!(atomic_load_explicit(&r1, memory_order_relaxed) == 1 && 
             atomic_load_explicit(&r2, memory_order_relaxed) == 0));
    return 0;
}