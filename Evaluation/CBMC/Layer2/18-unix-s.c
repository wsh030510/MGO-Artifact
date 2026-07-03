#include <pthread.h>

int socket(int domain, int type, int protocol) { return 3; }
int bind(int sockfd, const void *addr, unsigned int addrlen) { return 0; }
int listen(int sockfd, int backlog) { return 0; }
int ioctl(int fd, unsigned long request, ...) { return 0; }

int u_sock;

void *thread_0(void *arg) {
    char dummy_addr[110] = {0}; 
    bind(u_sock, dummy_addr, 13);
    return NULL;
}

void *thread_1(void *arg) {
    listen(u_sock, 0);
    ioctl(u_sock, 0x89e0, 0); // SIOCX25GSUBSCRIP
    return NULL;
}

int main() {
    u_sock = socket(1, 5, 0); 
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_0, NULL);
    pthread_create(&t2, NULL, thread_1, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}