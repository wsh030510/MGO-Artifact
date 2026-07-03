#include <pthread.h>
#include <assert.h>

int pending_message_count = 0;
int send_token = 1;

int t1_token = -1;
int t2_count = -1;

void *thread_try_start_send(void *arg) {
    pending_message_count = 1;
    t1_token = send_token;
    return NULL;
}

void *thread_get_from_queue(void *arg) {
    send_token = 0;
    t2_count = pending_message_count;
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_try_start_send, NULL);
    pthread_create(&t2, NULL, thread_get_from_queue, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    assert(!(t1_token == 1 && t2_count == 0));
    return 0;
}