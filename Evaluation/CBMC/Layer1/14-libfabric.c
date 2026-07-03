#include <pthread.h>
#include <assert.h>

int cmd_data = 0;
int resp_status = 0;

void* thread_progress(void* arg) {
    cmd_data = 2;
    __sync_synchronize(); 
    resp_status = -1;
    return NULL;
}

void* thread_check(void* arg) {
    if (resp_status == -1) {
        assert(cmd_data == 2);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_progress, NULL);
    pthread_create(&t2, NULL, thread_check, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}