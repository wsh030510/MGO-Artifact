#include <pthread.h>

int syscall(int number, ...) { return 0; }
int socket(int domain, int type, int protocol) { return 4; }
int writev(int fd, const void *iov, int iovcnt) { return 0; }
int setsockopt(int sockfd, int level, int optname, const void *optval, unsigned int optlen) { return 0; }

int tcp_sock = 3;
unsigned long ctx = 0;

void *thread_io(void *arg) {
    syscall(206, 2, &ctx); // io_setup
    int c1 = socket(16, 3, 21); // AF_NETLINK
    int dummy_cb[16] = {0};
    void *cbs[1] = { dummy_cb };
    syscall(208, ctx, 1, cbs); // io_submit
    struct { void *base; unsigned long len; } iov = { "data", 4 };
    writev(c1, &iov, 1);
    return NULL;
}

void *thread_tls(void *arg) {
    setsockopt(tcp_sock, 6, 31, "tls", 4); // SOL_TCP, TCP_ULP
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_io, NULL);
    pthread_create(&t2, NULL, thread_tls, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}