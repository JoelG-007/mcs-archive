// Semaphore + SIGSTOP/SIGCONT
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>

typedef struct Buffer {
    int block,busy,next;
}Buffer;

typedef struct Shared {
    int balance;
    Buffer b[3];
    int freeHead;
    sem_t lock;
}Shared;

void display(Shared *s){
    int p=s->freeHead;
    printf("\nFree List: ");
    while(p!=-1){
        printf("B%d -> ",p+1);
        p=s->b[p].next;
    }
    printf("NULL\n");
}

void emi(Shared *s){
    if(s->balance<800){
        printf("EMI: (Buffer or block NOT found)Sleeping - Scenario 4\n");
        raise(SIGSTOP);         // Putting EMI to sleep
    }
    sem_wait(&s->lock);
    s->balance-=800;
    printf("EMI: ₹800 paid\n");
    sem_post(&s->lock);
    return;
}

void withdraw(Shared *s){
    if(s->b[1].busy){
        printf("Withdrawal: (buffer busy or insufficiant)Sleeping - Scenario 5\n");
        raise(SIGSTOP);         // Putting withdraw to sleep
    }
    sem_wait(&s->lock);
    s->balance-=600;
    printf("Withdrawal: ₹600 withdrawn\n");
    sem_post(&s->lock);
    return;
}

void deposit(Shared *s,pid_t e,pid_t w){
    sem_wait(&s->lock);
    s->balance+=1000;
    s->b[0].busy=0;             // Releasing B1
    s->freeHead=0;
    printf("Deposit: ₹1000 added\n");
    sem_post(&s->lock);
    kill(e,SIGCONT);            // Continuing a stopped process
    kill(w,SIGCONT);
}

int main(){
    Shared *s=mmap(NULL,sizeof(Shared),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    s->balance=500;
    s->freeHead=-1;
    sem_init(&s->lock,1,1);
    for(int i=0;i<3;i++){
        s->b[i].block=10+i;
        s->b[i].busy=1;
        s->b[i].next=-1;
    }
    printf("\nInitial Balance: 500\nEMI: 800\nWithdraw: 600\n");
    display(s);  
    pid_t e=fork();
    if(e==0){           // Success
        emi(s);
        return 0;
    }
    pid_t w=fork();
    if(w==0){           // Success
        withdraw(s);
        return 0;
    }
    int status;
    waitpid(e,&status,WUNTRACED);       // Child process over, waiting for parent
    waitpid(w,&status,WUNTRACED);
    pid_t d=fork();
    if(d==0){
        deposit(s,e,w);
        return 0;
    }
    waitpid(d,NULL,0);
    waitpid(e,&status,WUNTRACED);
    waitpid(w,&status,WUNTRACED);
    kill(e,SIGCONT);
    kill(w,SIGCONT);
    waitpid(e,NULL,0);
    waitpid(w,NULL,0);
    printf("Final Balance: ₹%d\n",s->balance);
    display(s);
    sem_destroy(&s->lock);
    munmap(s,sizeof(Shared));
    return 0;
}

/*
               Initial
                ₹500
                  │
        ┌─────────┴─────────┐
        |                   |
        V                   V
       EMI              Withdrawal
    needs ₹800            B2 busy
        |                   |
        V                   V
     SIGSTOP             SIGSTOP
        ↓                   ↓
        └─────────┬─────────┘
                  |
                  V
               Deposit
                  │
              + ₹1000
                  |
                  V
                ₹1500
                  │
              B1 -> FREE
                  │
               SIGCONT
               /     \
              |       |
              V       V
       Withdrawal   EMI
          -₹600     -₹800
            │         │
            └────┬────┘
                 |
                 V
               ₹100
                 │
        Free List: B1 -> NULL
*/