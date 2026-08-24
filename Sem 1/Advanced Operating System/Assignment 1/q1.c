#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include<semaphore.h>

#define BUFS 3

typedef struct{
    int balance;
}Bank;

typedef struct{
    int block;
    int busy;
} Buffer;

Buffer bufferPool[BUFS] ={
    {101,1},
    {102,1},
    {103,0}
};

int freeList[BUFS]={0,0,1};

sem_t *mutex;
sem_t *sleepEMI;
sem_t *sleepWithdraw;

void printBuffers(){
    printf("\nBuffer Pool\n");

    for(int i=0;i<BUFS;i++){
        printf("Block %d -> %s\n",
               bufferPool[i].block,
               bufferPool[i].busy?"BUSY":"FREE");
    }
}

int allocateBuffer(){
    for(int i=0;i<BUFS;i++){
        if(freeList[i]){    
            freeList[i]=0;
            bufferPool[i].busy=1;
            return i;
        }
    }
    return -1;
}

void releaseBuffer(int index){
    freeList[index]=1;
    bufferPool[index].busy=0;
}

int main(){
    Bank *bank=mmap(NULL,sizeof(Bank),
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED|MAP_ANONYMOUS,-1,0);

    bank->balance=300;         // EMI and Withdrawal processes will go into sleep mode due to insufficient balance.

    mutex=mmap(NULL,sizeof(sem_t),
               PROT_READ|PROT_WRITE,
               MAP_SHARED|MAP_ANONYMOUS,-1,0);


    sleepEMI=mmap(NULL,sizeof(sem_t),
                  PROT_READ|PROT_WRITE,
                  MAP_SHARED|MAP_ANONYMOUS,-1,0);

    sleepWithdraw=mmap(NULL,sizeof(sem_t),
                       PROT_READ|PROT_WRITE,
                       MAP_SHARED|MAP_ANONYMOUS,-1,0);

    sem_init(mutex,1,1);
    sem_init(sleepEMI,1,0);
    sem_init(sleepWithdraw,1,0);

    printBuffers();

    pid_t emi=fork();

    if(emi==0){
        sem_wait(mutex);

        printf("\nEMI Process PID=%d\n",getpid());

        if(bank->balance<1200){
            printf("Insufficient Balance\n");
            printf("EMI Sleeping...\n");

            sem_post(mutex);

            sem_wait(sleepEMI);

            sem_wait(mutex);
        }

        int b=allocateBuffer();

        printf("EMI Allocated Buffer %d\n",bufferPool[b].block);

        bank->balance-=1200;

        printf("EMI Deducted 1200\n");
        printf("Balance=%d\n",bank->balance);

        releaseBuffer(b);

        sem_post(mutex);

        exit(0);
    }

    pid_t withdraw=fork();

    if(withdraw==0){
        sem_wait(mutex);

        printf("\nWithdrawal PID=%d\n",getpid());

        if(bank->balance<500){
            printf("Insufficient Balance\n");
            printf("Withdrawal Sleeping...\n");

            sem_post(mutex);

            sem_wait(sleepWithdraw);

            sem_wait(mutex);
        }

        int b=allocateBuffer();

        printf("Withdrawal Allocated Buffer %d\n",bufferPool[b].block);

        bank->balance-=500;

        printf("Withdrawal Done\n");
        printf("Balance=%d\n",bank->balance);

        releaseBuffer(b);

        sem_post(mutex);

        exit(0);
    }

    pid_t deposit=fork();

    if(deposit==0){
        sleep(3);

        sem_wait(mutex);

        printf("\nDeposit PID=%d\n",getpid());

        int b=allocateBuffer();

        printf("Deposit Allocated Buffer %d\n",bufferPool[b].block);

        bank->balance+=2000;            //

        printf("Deposited 2000\n");
        printf("Balance=%d\n",bank->balance);

        releaseBuffer(b);

        printf("\nWaking EMI and Withdrawal\n");

        sem_post(sleepEMI);
        sem_post(sleepWithdraw);

        sem_post(mutex);

        exit(0);
    }

    wait(NULL);
    wait(NULL);
    wait(NULL);

    printf("\nFinal Balance=%d\n",bank->balance);

    printBuffers();

    sem_destroy(mutex);
    sem_destroy(sleepEMI);
    sem_destroy(sleepWithdraw);

    return 0;
}
/*
Sample Output 1 (EMI executes first after wake-up)
Sample Output 2 (Withdrawal sleeps too) If Balance = 0<x<500 
*/

/*
System calls used - 
fork() → Creates EMI, Withdrawal and Deposit processes.
wait() → Parent waits for all child processes.
mmap() → Shared memory for bank balance and semaphores.
sleep() → Deposit runs later, allowing EMI and Withdrawal to block first.
getpid() → Displays process IDs.
sem_wait() → Blocks processes (sleep state).
sem_post() → Wakes blocked processes, creating the required race condition.
*/