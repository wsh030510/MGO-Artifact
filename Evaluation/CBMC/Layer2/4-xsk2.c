#include <pthread.h>

struct sockaddr_xdp { int sxdp_family; int sxdp_ifindex; };
int setsockopt(int sockfd, int level, int optname, const void *optval, unsigned int optlen) { return 0; }
int bind(int sockfd, void *addr, unsigned int addrlen) { return 0; }
int poll(void *fds, unsigned int nfds, int timeout) { return 0; }

int xdp_fd = 3, inet_fd = 4;

void *thread_setup_ring(void *arg) {
    int size = 8;
    setsockopt(xdp_fd, 283, 1, &size, sizeof(size));
    struct sockaddr_xdp sxdp = { .sxdp_family = 44, .sxdp_ifindex = 0 };
    bind(xdp_fd, &sxdp, sizeof(sxdp));
    return NULL;
}

void *thread_poll(void *arg) {
    poll(NULL, 0, 1);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_setup_ring, NULL);
    pthread_create(&t2, NULL, thread_poll, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}