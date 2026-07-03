#include <pthread.h>

union bpf_attr { int map_type; int key_size; int value_size; int max_entries; int map_fd; unsigned long key; unsigned long value; };
int syscall(int number, ...) { return 0; }
int connect(int sockfd, void *addr, unsigned int addrlen) { return 0; }

int u_sock = 5;

void *thread_bpf(void *arg) {
    union bpf_attr attr = { .map_type = 15 };
    int map_fd = syscall(321, 0, &attr, sizeof(attr)); 
    
    union bpf_attr attr_upd = { .map_fd = map_fd };
    syscall(321, 2, &attr_upd, sizeof(attr_upd)); 
    return NULL;
}

void *thread_sock(void *arg) {
    connect(u_sock, NULL, 0);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_bpf, NULL);
    pthread_create(&t2, NULL, thread_sock, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}