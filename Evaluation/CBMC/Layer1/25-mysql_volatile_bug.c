#include <pthread.h>
#include <assert.h>

int log_state = 0;
int log_file_data = 0;

void *thread_open_binlog(void *arg) {
    log_file_data = 999;
    log_state = 1;
    return NULL;
}

void *thread_use_binlog(void *arg) {
    if (log_state == 1) {
        assert(log_file_data == 999);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_open_binlog, NULL);
    pthread_create(&t2, NULL, thread_use_binlog, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}