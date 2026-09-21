#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 1024

int main(){
    int shmid;
    char *shared_memory;
    pid_t pid;

    shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | 0666);    // Create shared memory
    if(shmid == -1){
        perror("shmget");
        exit(1);
    }

    shared_memory = (char *)shmat(shmid, NULL, 0);          // Attach shared memory
    if(shared_memory == (char *)-1){
        perror("shmat");
        exit(1);
    }

    pid = fork();                                           // Creating a child 
    if(pid < 0){
        perror("fork");
        exit(1);
    }

    if(pid > 0){                                            // In Parent
        printf("Parent Process (Writer)\n");
        strcpy(shared_memory, "Hello from Parent Process!");// Write message to shared memory
        printf("Parent wrote: %s\n", shared_memory);
        wait(NULL);                                         // Wait for child to complete

        if(shmdt(shared_memory) == -1){                     // Detach shared memory
            perror("shmdt");
            exit(1);
        }

        if(shmctl(shmid, IPC_RMID, NULL) == -1){            // Remove shared memory
            perror("shmctl");
            exit(1);
        }
        printf("Parent: Shared memory removed.\n");
    }else{                                                  // In Child
        printf("Child Process (Reader)\n");
        sleep(1);                                           // Give parent time to write
        printf("Child read: %s\n", shared_memory);          // Read message from shared memory

        if(shmdt(shared_memory) == -1){                     // Detach shared memory
            perror("shmdt");
            exit(1);
        }
        exit(0);
    }
    return 0;
}