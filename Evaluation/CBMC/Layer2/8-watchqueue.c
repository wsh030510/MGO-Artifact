#include <pthread.h>

int ioctl(int fd, unsigned long request, ...) { return 0; }
int read(int fd, void *buf, unsigned int count) { return 0; }
int syscall(int number, ...) { return 0; }

int pipe_fds[2] = {3, 4};

void *thread_ioctl(void *arg) {
    int size = 16;
    ioctl(pipe_fds[0], 0x40045760, size);
    return NULL;
}

void *thread_read(void *arg) {
    char buf[100];
    read(pipe_fds[0], buf, sizeof(buf));
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_ioctl, NULL);
    pthread_create(&t2, NULL, thread_read, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}