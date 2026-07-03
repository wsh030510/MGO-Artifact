// 7-nbd_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int msg_count = 0;
#define EXPECTED_MSGS 10

void *thread_netlink(void *arg) {
    for (int i = 0; i < EXPECTED_MSGS; i++) {
        atomic_fetch_add_explicit(&msg_count, 1, memory_order_relaxed);
    }
    return NULL;
}

void *thread_open_dev(void *arg) {
    int cnt = atomic_load_explicit(&msg_count, memory_order_relaxed);
    if (cnt < EXPECTED_MSGS) {
        assert(0);  // open before all config messages
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_netlink, NULL);
    pthread_create(&t2, NULL, thread_open_dev, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}