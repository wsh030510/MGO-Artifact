#include <pthread.h>
#include <stddef.h>

int setsockopt(int sockfd, int level, int optname, const void *optval, unsigned int optlen) { return 0; }
void *mmap(void *addr, unsigned long length, int prot, int flags, int fd, unsigned long offset) { return NULL; }
int close(int fd) { return 0; }
int socket(int domain, int type, int protocol) { return 3; }

int xdp_fd;

void *thread_mmap(void *arg) {
    int size = 0x4000;
    setsockopt(xdp_fd, 283, 5, &size, 4);
    mmap(NULL, 0x4000, 3, 1, xdp_fd, 0x100000000ULL);
    return NULL;
}

void *thread_close(void *arg) {
    close(xdp_fd);
    return NULL;
}

int main() {
    xdp_fd = socket(44, 3, 0); // AF_XDP, SOCK_RAW
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_mmap, NULL);
    pthread_create(&t2, NULL, thread_close, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}