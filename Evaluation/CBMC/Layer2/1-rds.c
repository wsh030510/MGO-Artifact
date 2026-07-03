#include <pthread.h>
#include <stddef.h>

struct sockaddr { int sa_family; char sa_data[14]; };
struct msghdr { void *msg_name; int msg_namelen; void *msg_iov; int msg_iovlen; void *msg_control; int msg_controllen; int msg_flags; };
int bind(int sockfd, const struct sockaddr *addr, unsigned int addrlen) { return 0; }
int sendmsg(int sockfd, const struct msghdr *msg, int flags) { return 0; }

int rds_sock = 3;

void *thread_bind(void *arg) {
    struct sockaddr addr = { .sa_family = 21 }; 
    bind(rds_sock, &addr, sizeof(addr));
    return NULL;
}

void *thread_send(void *arg) {
    struct msghdr msg = {0};
    sendmsg(rds_sock, &msg, 0);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_bind, NULL);
    pthread_create(&t2, NULL, thread_send, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}