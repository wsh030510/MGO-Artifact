#include <pthread.h>

int setsockopt(int sockfd, int level, int optname, const void *optval, unsigned int optlen) { return 0; }
int write(int fd, const void *buf, unsigned int count) { return 0; }
int epoll_ctl(int epfd, int op, int fd, void *event) { return 0; }

int sock_fd;
int epoll_fd = 4;

void *thread_tls_config(void *arg) {
    setsockopt(sock_fd, 6, 31, "tls", 4);
    char key_material[60] = {0}; 
    setsockopt(sock_fd, 282, 1, key_material, 56);
    return NULL;
}

void *thread_io(void *arg) {
    write(sock_fd, "test_data", 9);
    
    int ev[2] = {0}; 
    epoll_ctl(epoll_fd, 1, sock_fd, ev); 
    return NULL;
}

int main() {
    sock_fd = 3; 
    
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_tls_config, NULL);
    pthread_create(&t2, NULL, thread_io, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}