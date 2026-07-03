#include <pthread.h>

int syscall(int number, ...) { return 0; }
int setsockopt(int sockfd, int level, int optname, const void *optval, unsigned int optlen) { return 0; }
int poll(void *fds, unsigned int nfds, int timeout) { return 0; }

int tun_fd = 3, xdp_fd = 4;

void *thread_bpf(void *arg) {
    int dummy_attr[15] = {0}; 
    syscall(321, 5, dummy_attr, 120); // BPF_PROG_LOAD
    syscall(321, 17, dummy_attr, 120); // BPF_RAW_TRACEPOINT_OPEN
    return NULL;
}

void *thread_poll(void *arg) {
    int size = 2;
    setsockopt(xdp_fd, 283, 3, &size, 4);
    struct { int fd; short events; short revents; } fds[2] = {{tun_fd, 1, 0}, {xdp_fd, 1, 0}};
    poll(&fds, 2, 1);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_bpf, NULL);
    pthread_create(&t2, NULL, thread_poll, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}