// Using Semaphores as Flags/Signals
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>
union semun{ int val; };
int main(){
    int shmid, semid;
    char *shm;
    shmid = shmget(IPC_PRIVATE, 100, IPC_CREAT | 0666);
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    union semun arg;
    arg.val = 0;
    semctl(semid, 0, SETVAL, arg);
    if(fork() == 0){
        shm = shmat(shmid, NULL, 0);
        strcpy(shm, "Hello from Child");
        struct sembuf signal = {0, 1, 0};
        semop(semid, &signal, 1);
        shmdt(shm);
        exit(0);
    }else{
        struct sembuf wait_sem = {0, -1, 0};
        semop(semid, &wait_sem, 1);
        shm = shmat(shmid, NULL, 0);
        printf("Parent received: %s\n", shm);
        wait(NULL);
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);}
    return 0;
}