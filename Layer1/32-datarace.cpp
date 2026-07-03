#include <stdio.h>
#include <pthread.h>
#include <string.h> // Include library for string manipulation functions

int data = 0; // Global variable representing shared data
int flag = 0; // Global variable acting as a signal indicating whether data has been written
char str[50]; // Declare a sufficiently large character array to store the generated string

// Thread 1 function
void *writer_thread(void *args) {
    data = 42; // Write data
    flag = 1;  // Set flag to 1, indicating data has been written
    return NULL;
}

// Thread 2 function
void *reader_thread(void *args) {
    // This loop checks whether flag is 1, but if instruction reordering occurs,
    // flag might be set to 1 first while data hasn't been assigned the correct value yet
    if (flag == 1) {
        sprintf(str, "Data happen %d", data);
    }
    return NULL;
}

int main() {
    pthread_t writer, reader;
    int num = 0;

    // Create two threads
    pthread_create(&writer, NULL, writer_thread, NULL);
    pthread_create(&reader, NULL, reader_thread, NULL);

    // Wait for threads to finish
    pthread_join(writer, NULL);
    pthread_join(reader, NULL);

    if (strcmp(str, "Data happen 0") == 0) {
        num++;
        printf("%s\n", str);
    }
    data = 0;
    memset(str, 0, sizeof(str));
    return 0;
}