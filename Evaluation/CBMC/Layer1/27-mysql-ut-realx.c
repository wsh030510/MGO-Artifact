#include <pthread.h>
#include <assert.h>

int inno_data = 0;
int inno_data_ready = 0;

void *thread_writer(void *arg) {
    inno_data_ready = 1;
    inno_data = 1;
    return NULL;
}

void *thread_reader(void *arg) {
    if (inno_data_ready == 1) {
        assert(inno_data == 1);
    }
    return NULL;
}

int main(void) {
    pthread_t tw, tr;
    pthread_create(&tw, NULL, thread_writer, NULL);
    pthread_create(&tr, NULL, thread_reader, NULL);
    pthread_join(tw, NULL);
    pthread_join(tr, NULL);
    return 0;
}