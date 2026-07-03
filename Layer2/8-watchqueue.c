/*
commit_hash:2ed147f015af2b48f41c6f0b6746aa9ea85c19f3
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <keyutils.h> 

#define IOC_WATCH_QUEUE_SET_SIZE _IOW('W', 0x60, int)

int pipe_fds[2];
key_serial_t key;

void *thread_ioctl(void *arg) {
    int size = 16; // 0x10
    ioctl(pipe_fds[0], IOC_WATCH_QUEUE_SET_SIZE, size);
    return NULL;
}

void *thread_read(void *arg) {
    char buf[100];
    read(pipe_fds[0], buf, sizeof(buf));
    return NULL;
}

int main() {
    key = add_key("user", "syz", "data", 4, KEY_SPEC_THREAD_KEYRING);
    pipe2(pipe_fds, O_CLOEXEC | O_NONBLOCK);

    syscall(__NR_keyctl, 17 /* KEYCTL_WATCH_KEY */, key, pipe_fds[0], 0);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_ioctl, NULL);
    pthread_create(&t2, NULL, thread_read, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}