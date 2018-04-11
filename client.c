#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define true 1
#define false 0

#define shared_file "queue"

pthread_t thread_write, thread_read;
sem_t mutex;

void *read() {
    while(true) {
        key_t key = ftok(shared_file, 'R');
        int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
        char *str = (char *)shmat(shmid, (void *)0, 0);
        printf("%d\n", strlen(str));
        if(strlen(str)>0) {
            printf("Data read from memory: %s\n", str);
        }
        shmdt(str);
        shmctl(shmid, IPC_RMID, NULL);
    }
}

void *write() {
    while(true) {
        key_t key = ftok(shared_file, 65);
        int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
        char *str = (char *)shmat(shmid, (void *)0, 0);
        printf("Enter Message: ");
        fgets(str, sizeof(str), stdin);
        shmdt(str);
    }
}

int main() {
    while(true) {
        sem_init(&mutex, 0, 1);
        pthread_create(&thread_write, NULL, write, NULL);
        pthread_create(&thread_read, NULL, read, NULL);
        pthread_join(thread_write, NULL);
        pthread_join(thread_read, NULL);
        sem_destroy(&mutex);
    }

    return 0;
}