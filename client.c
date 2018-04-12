#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define true 1
#define false 0

#define max_queue_elements 100
#define num_test_cases 50

pthread_t thread[num_test_cases];
volatile int index_written[max_queue_elements];
key_t key = 2344234;
int shmid;

int getRandom(int range) 
{
    return rand() % range;
}

void *read_func(void *id)
{
    int *data;
    int *shmid = (int *)id;
    if ((data = shmat(*shmid, (void *)0, 0)) == (int *)(-1))
    {
        perror("shmat");
        exit(1);
    }
    int index_to_read = getRandom(max_queue_elements);
    while(index_written[index_to_read] != 1) {
        index_to_read = getRandom(max_queue_elements);
    }

    printf("Data read at Index %d: %d\n", index_to_read, data[index_to_read]);
    if (shmdt(data) == -1)
    {
        perror("shmdt");
        exit(1);
    }
}

void *write_func(void* id)
{
    int *data;
    int *shmid = (int *)id;

    int index_to_insert = getRandom(max_queue_elements);
    int data_to_insert = getRandom(100);
    

    if ((data = shmat(*shmid, (void *)0, 0)) == (int *)(-1))
    {
        perror("shmat");
        exit(1);
    }

    printf("Inserting %d at %d\n", data_to_insert, index_to_insert);

    data[index_to_insert] = data_to_insert;
    index_written[index_to_insert] = 1;

    if (shmdt(data) == -1)
    {
        perror("shmdt");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int id;

    if ((id = shmget(key, max_queue_elements * sizeof(int), 0644 | IPC_CREAT)) == -1)
    {
        perror("shmget");
        exit(1);
    }

    int temp_cases = num_test_cases;

    while (temp_cases > 0)
    {
        int choice = getRandom(2);
        if (choice == 0)
        {
            pthread_create(&thread[temp_cases], NULL, write_func, &id);
        }
        else if (choice == 1)
        {
            pthread_create(&thread[temp_cases], NULL, read_func, &id);
        }
        else if (choice == 2)
        {
            return 0;
        }
        temp_cases -= 1;
    }

    temp_cases = num_test_cases;

    while (temp_cases-- > 0) {
        pthread_join(thread[temp_cases], NULL);
    }
    return 0;
}
