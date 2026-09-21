#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main(){
    int shmid;
    char *shared_memory;

    shmid = shmget(IPC_PRIVATE, 1024, IPC_CREAT | 0666);
    if(shmid == -1){
        perror("shmget");
        exit(1);
    }

    shared_memory = (char *)shmat(shmid, NULL, 0);
    if(shared_memory == (char *)-1){
        perror("shmat");
        exit(1);
    }

    if(fork() == 0){
        printf("Child: Reading message from shared memory...\n");
        sleep(1);   // Due to the process running simultaneously, the sleep will make sure the child doesn't access the shared memory prematurely
        printf("Child: %s\n", shared_memory);

        shmdt(shared_memory);
        exit(0);
    }else{
        strcpy(shared_memory, "Hello from Parent using Shared Memory!");
        printf("Parent: Message written to shared memory.\n");
        wait(NULL);

        shmdt(shared_memory);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}

/*
What is used and why:
- shmget() creates a shared memory segment.
- shmat() attaches the shared memory to the process.
- fork() creates a child process.
- The parent writes a message into shared memory.
- The child reads the same message from shared memory.
- shmdt() detaches the shared memory.
- shmctl() removes the shared memory segment.
*/