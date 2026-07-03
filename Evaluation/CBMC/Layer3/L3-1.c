#include <pthread.h>

int setsockopt(int sockfd, int level, int optname, const void *optval, unsigned int optlen) { return 0; }
int sendmmsg(int sockfd, void *msgvec, unsigned int vlen, int flags) { return 0; }
int socket(int domain, int type, int protocol) { return 3; }

int sock_fd;

void *thread_ulp(void *arg) {
    setsockopt(sock_fd, 6, 31, "smc", 4);
    return NULL;
}

void *thread_send(void *arg) {
    int msgs[10] = {0}; 
    sendmmsg(sock_fd, msgs, 1, 0);
    return NULL;
}

int main() {
    sock_fd = socket(2, 1, 0);
    
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_ulp, NULL);
    pthread_create(&t2, NULL, thread_send, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}