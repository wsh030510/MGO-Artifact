#include <pthread.h>

union bpf_attr { int prog_type; };
int syscall(int number, ...) { return 0; }
int setsockopt(int sockfd, int level, int optname, const void *optval, unsigned int optlen) { return 0; }
struct pollfd { int fd; short events; short revents; };
int poll(struct pollfd *fds, unsigned int nfds, int timeout) { return 0; }

int tun_fd = 3, xdp_fd = 4;

void *thread_bpf(void *arg) {
    union bpf_attr attr = { .prog_type = 17 };
    syscall(321, 5, &attr, sizeof(attr)); 
    return NULL;
}

void *thread_tx_ring(void *arg) {
    int size = 2;
    setsockopt(xdp_fd, 283, 3, &size, 4); 
    
    struct pollfd fds[2] = {{tun_fd, 1, 0}, {xdp_fd, 1, 0}};
    poll(fds, 2, 1);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_bpf, NULL);
    pthread_create(&t2, NULL, thread_tx_ring, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}