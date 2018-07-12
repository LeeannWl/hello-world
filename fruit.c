#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
     
#else
   
    union semun {
        int val;                     
        struct semid_ds *buf;        
        unsigned short int *array;   
        struct seminfo *__buf;      
    };
#endif

int SemInit(int sem,int val);
int P(int semid);
int V(int semid);

int S_empty,S_ful;

int main()
{
    pid_t pid;
    S_empty = semget(ftok("./fruit.c",'a'), 1, 0666 | IPC_CREAT);
    S_ful = semget(ftok("./fruit.c",'b'), 1, 0666 | IPC_CREAT);
    SemInit(S_empty, 1);
    SemInit(S_ful, 0);
    
    pid = fork();
    switch(pid)
    {
    case 0: //SON
        while(1)
        {
            P(S_ful);
            printf("SON: Get an apple\n");
            V(S_empty);
            printf("Son: Eating.....\n");
            sleep(rand()%4);
        }
        break;
    default:  //Father
        while(1)
        {
            printf("FATHER:Prepare an apple\n");
            sleep(rand()%2);
            P(S_empty);
            printf("FATHER:Put the apple to disc\n");
            V(S_ful);
        }
    }//end switch
    return 0;
}

int SemInit(int semid, int val)
{
    union semun sem_union;

    sem_union.val = val;
    if (semctl(semid, 0, SETVAL, sem_union) == -1) return(0);
    return(1);
}


int P(int semid)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    semop(semid, &sem_b, 1);
    return(1);
}

int V(int semid)
{
    struct sembuf sem_b;
    
    sem_b.sem_num = 0;
    sem_b.sem_op = 1; /* V() */
    sem_b.sem_flg = SEM_UNDO;
    semop(semid, &sem_b, 1);
    return(1);
}

