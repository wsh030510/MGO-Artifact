// 1-vlan_genmc.c - Two-step config vs hwaddr
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int vlan_created = 0;
atomic_int vlan_ready = 0;

void *thread_config(void *arg) {
    atomic_store_explicit(&vlan_created, 1, memory_order_relaxed);
    atomic_store_explicit(&vlan_ready, 1, memory_order_relaxed);
    return NULL;
}

void *thread_transmit(void *arg) {
    int ready = atomic_load_explicit(&vlan_ready, memory_order_relaxed);
    if (ready) {
        int created = atomic_load_explicit(&vlan_created, memory_order_relaxed);
        if (created == 0) assert(0);  // Saw ready but created not completed
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_config, NULL);
    pthread_create(&t2, NULL, thread_transmit, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}