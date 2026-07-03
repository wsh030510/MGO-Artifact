#include <pthread.h>

int ioctl(int fd, unsigned long request, ...) { return 0; }
int open(const char *pathname, int flags) { return 3; }

int fd;

void *thread_ldisc(void *arg) {
    int ldisc = 21; // N_GSM0710
    ioctl(fd, 0x5423, &ldisc); // TIOCSETD
    return NULL;
}

void *thread_term(void *arg) {
    int t[10] = {0}; // Dummy termios structure
    ioctl(fd, 0x5404, t); // TCSETSF
    return NULL;
}

int main() {
    fd = open("/dev/ptmx", 2);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_ldisc, NULL);
    pthread_create(&t2, NULL, thread_term, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}