#include <pthread.h>
#include <assert.h>

int gtid_mode = 0;
int gtid_mode_counter = 0;

void *writer(void *arg) {
    gtid_mode = 1;   
    gtid_mode_counter = 1;
    return NULL;
}

void *reader(void *arg) {
    int cur_counter = gtid_mode_counter;
    int mode = gtid_mode;
    
    if (cur_counter == 1) {
        assert(mode == 1);
    }
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