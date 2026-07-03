#include <pthread.h>

int socket(int domain, int type, int protocol) { return 3; }
int bind(int sockfd, const void *addr, unsigned int addrlen) { return 0; }
int listen(int sockfd, int backlog) { return 0; }
int ioctl(int fd, unsigned long request, ...) { return 0; }

int u_sock;

void *thread_setup(void *arg) {
    char dummy_addr[110] = {0}; 
    bind(u_sock, dummy_addr, 16);
    listen(u_sock, 0);
    return NULL;
}

void *thread_weird_ioctl(void *arg) {
    ioctl(u_sock, 0x89e0, 0); // SIOCX25GSUBSCRIP
    return NULL;
}

int main() {
    u_sock = socket(1, 5, 0); // AF_UNIX, SOCK_SEQPACKET
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_setup, NULL);
    pthread_create(&t2, NULL, thread_weird_ioctl, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}