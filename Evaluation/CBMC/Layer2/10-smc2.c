#include <pthread.h>

struct iovec { void *iov_base; unsigned int iov_len; };
struct msghdr { void *msg_name; int msg_namelen; struct iovec *msg_iov; int msg_iovlen; void *msg_control; int msg_controllen; int msg_flags; };
struct mmsghdr { struct msghdr msg_hdr; unsigned int msg_len; };
int setsockopt(int sockfd, int level, int optname, const void *optval, unsigned int optlen) { return 0; }
int sendmmsg(int sockfd, struct mmsghdr *msgvec, unsigned int vlen, int flags) { return 0; }

int sock_fd = 3;

void *thread_opt(void *arg) {
    int val = 1;
    setsockopt(sock_fd, 6, 0x22, &val, 4);
    return NULL;
}

void *thread_send(void *arg) {
    struct mmsghdr msgs[1] = {0};
    sendmmsg(sock_fd, msgs, 1, 0);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_opt, NULL);
    pthread_create(&t2, NULL, thread_send, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}