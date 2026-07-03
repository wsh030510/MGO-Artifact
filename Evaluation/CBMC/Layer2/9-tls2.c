#include <pthread.h>

struct sockaddr_in6 { int sin6_family; int sin6_addr[4]; };
int setsockopt(int sockfd, int level, int optname, const void *optval, unsigned int optlen) { return 0; }
int connect(int sockfd, void *addr, unsigned int addrlen) { return 0; }

int sock_fd = 3;

void *thread_repair_connect(void *arg) {
    int val = 1;
    setsockopt(sock_fd, 6, 19, &val, 4); 
    struct sockaddr_in6 addr = { .sin6_family = 10 };
    connect(sock_fd, (void*)&addr, sizeof(addr));
    return NULL;
}

void *thread_tls_md5(void *arg) {
    setsockopt(sock_fd, 6, 31, "tls", 4); 
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_repair_connect, NULL);
    pthread_create(&t2, NULL, thread_tls_md5, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}