#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define KEY 123
#define QUEUE_SIZE 10
#define MAX_VALUE 100
#define TOTAL_THREADS 20

sem_t* sem_array;
sem_t* write_access;
volatile int running_threads = 0;
volatile int rdr_num = 0;
volatile int wtr_num = 0;

void *reader(void *_shmid)
{
	int reader_num = ++rdr_num;
	int* shmid = (int *)_shmid;
	int* array = shmat(*shmid, 0, SHM_RDONLY);
	int r = rand() % QUEUE_SIZE;
	sem_wait(&sem_array[r]);
	printf("Reader %d : The value at index %d is %d\n", reader_num, r, array[r]);
	sleep(0.5);
	sem_post(&sem_array[r]);
	running_threads--;
}

void *writer(void* _shmid)
{
	int writer_num = ++wtr_num;
	int* shmid = (int *)_shmid;
	int* array = shmat(*shmid, 0, 0);
	int r = rand() % QUEUE_SIZE;
	
	if (sem_trywait(write_access) == 0 && sem_trywait(&sem_array[r]) == 0)
	{
		printf("Writer %d : Write Access acquired at index %d\n", writer_num, r);
	} 
	else
	{
		printf("Writer %d : Need to wait for write access at index %d\n", writer_num, r);
		sem_wait(write_access);
		sem_wait(&sem_array[r]);
	}

	int new_value = rand()%MAX_VALUE;
	array[r] = new_value;
	printf("Writer %d : The new value at index %d is %d\n", writer_num, r, array[r]);
	sleep(0.5);
	sem_post(&sem_array[r]);
	sem_post(write_access);
	running_threads--;
}

int main(int argc, char const *argv[])
{
	sem_array = (sem_t *)malloc(QUEUE_SIZE * sizeof(sem_t));
	write_access = (sem_t *)malloc(sizeof(sem_t));
	int i;
	for(i = 0; i < QUEUE_SIZE; ++i)
	{
		sem_init(&sem_array[i], 0,1);
	}
	sem_init(write_access, 0, 1);
	key_t key = KEY;
	int shmid = shmget(key, QUEUE_SIZE*sizeof(int), 0666 | IPC_CREAT);
	if(shmid < 0)
	{
		perror("Error getting shared memory");
		exit(1);
	}
	printf("Starting...\n");
	int *array;
	array = malloc(QUEUE_SIZE * sizeof(int));
	array = shmat(shmid, 0, 0);
	pthread_t threads[TOTAL_THREADS];
	i = 0;
	for(i = 0; i < TOTAL_THREADS; ++i)
	{
		int r = rand()%4;
		if(r == 3)
		{
			pthread_create(&threads[i], NULL, writer, &shmid);
		}
		else
		{
			pthread_create(&threads[i], NULL, reader, &shmid);
		}
		running_threads++;
	}

	while (running_threads > 0)
		sleep(1);
	
	i = 0;
	for(i = 0; i<TOTAL_THREADS; ++i)
	{
		pthread_join(&threads[i], NULL);
	}
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}
