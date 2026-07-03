#include <pthread.h>
#include <assert.h>

int seq_state = 0;
int shared_data = 0;

void *reader(void *arg) {
    int stamp = seq_state;
    int val = shared_data;
    int new_stamp = seq_state;
    
    if (stamp == 0 && new_stamp == 0) {
        assert(val == 0);
    }
    return NULL;
}

void *writer(void *arg) {
    shared_data = 1;
    seq_state = 1;
    return NULL;
}

int main() {
    pthread_t r, w;
    pthread_create(&r, NULL, reader, NULL);
    pthread_create(&w, NULL, writer, NULL);
    pthread_join(r, NULL);
    pthread_join(w, NULL);
    return 0;
}