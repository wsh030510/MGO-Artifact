/*
 * Bug #99556: Weak memory ordering vulnerability
 *https://bugs.mysql.com/bug.php?id=99556
 */

#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>

// Global configuration value (non-atomic, ordinary variable)
int gtid_mode = 0;

// Buggy counter: using relaxed memory ordering
atomic_int gtid_mode_counter = 0;

// Reader thread local cache
int cached_gtid_mode = -1;
int cached_counter = -1;

volatile int running = 1;

void *reader(void *arg) {
    while (running) {
        // Bug: relaxed load, may never see writer's store
        int cur_counter = atomic_load_explicit(&gtid_mode_counter,
                                               memory_order_relaxed);
        if (cur_counter != cached_counter) {
            // Even if cur_counter changes, gtid_mode read may not be synchronized
            cached_gtid_mode = gtid_mode;
            cached_counter = cur_counter;
        }
        usleep(100);
    }
    return NULL;
}

void *writer(void *arg) {
    int new_value = 0;
    for (int i = 0; i < 5; ++i) {
        new_value = (new_value == 0) ? 1 : 0;
        gtid_mode = new_value;   // Ordinary write
        // Bug: relaxed fetch_add, no release barrier
        atomic_fetch_add_explicit(&gtid_mode_counter, 1,
                                  memory_order_relaxed);
        sleep(1);
    }
    running = 0;
    return NULL;
}

int main() {
    pthread_t wt, rt;
    pthread_create(&rt, NULL, reader, NULL);
    pthread_create(&wt, NULL, writer, NULL);
    pthread_join(wt, NULL);
    pthread_join(rt, NULL);
    return 0;
}