// #include <sys/types.h>
// #include <sys/ipc.h>
// #include <sys/shm.h>
// #include <stdio.h>

// #define true 1
// #define false 0

// #define shared_file "queue"

// int main()
// {
//     int shmid, i;
//     key_t key;
//     char *shm, *s;

//     key = ftok(shared_file, 65);

//     if ((shmid = shmget(key, 1024, IPC_CREAT | 0666)) < 0)
//     {
//         perror("shmget");
//         return 0;
//     }
//     if ((shm = shmat(shmid, NULL, 0)) == (char *)-1)
//     {
//         perror("shmat");
//         return 0;
//     }
//     *shm = '!';
//     while (*shm != '*')
//     {
//         char a[100];
//         printf("Enter Message: ");
//         gets(a);
//         if (a[0] != '*')
//         {
//             s = shm + 1;
//             for (i = 0; i < strlen(a); i++)
//                 *s++ = a[i];
//             *s = NULL;
//             *shm = '@';

//             while (*shm != '#' && *shm != '*')
//                 sleep(1);

//             if (*shm != '*')
//             {
//                 printf("client:");
//                 for (s = shm + 1; *s != NULL; s++)
//                     putchar(*s);
//                 putchar('\n');
//             }
//             else
//                 printf("connection closed by client");
//         }
//         else
//             *shm = '*';
//     }
//     shmid = shmdt(shm);

//     return 0;
// }