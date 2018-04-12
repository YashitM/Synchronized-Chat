#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define true 1
#define false 0

#define max_queue_size 1024

key_t key = 2344234;

void read_func()
{
    int shmid;
    char *data;

    if ((shmid = shmget(key, max_queue_size, 0644 | IPC_CREAT)) == -1)
    {
        perror("shmget");
        exit(1);
    }

    if ((data = shmat(shmid, (void *)0, 0)) == (char *)(-1))
    {
        perror("shmat");
        exit(1);
    }

    printf("Data read: %s\n", data);

    if (shmdt(data) == -1)
    {
        perror("shmdt");
        exit(1);
    }
}

void write_func()
{
    int shmid;
    char *data;

    if ((shmid = shmget(key, max_queue_size, 0644 | IPC_CREAT)) == -1)
    {
        perror("shmget");
        exit(1);
    }

    if ((data = shmat(shmid, (void *)0, 0)) == (char *)(-1))
    {
        perror("shmat");
        exit(1);
    }

    printf("Enter your message: ");

    fgets(data, sizeof(data), stdin);

    if (shmdt(data) == -1)
    {
        perror("shmdt");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    while (true)
    {
        int choice;
        printf("Select an option:\n");
        printf("1. Write\n");
        printf("2. Read\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        while ((getchar()) != '\n')
            ;
        if (choice == 1)
        {
            write_func();
        }
        else if (choice == 2)
        {
            read_func();
        }
        else if (choice == 3)
        {
            return 0;
        }

        // sem_init(&mutex, 1, 0);
        // pthread_create(&thread_write, NULL, write, NULL);
        // pthread_create(&thread_read, NULL, read, NULL);
        // pthread_join(thread_write, NULL);
        // pthread_join(thread_read, NULL);
        // sem_destroy(&mutex);
    }

    return 0;
}
