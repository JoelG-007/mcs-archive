#include <stdio.h>
#include <unistd.h>       // fork(), pipe(), read(), write()
#include <signal.h>       // SIGSTOP, SIGCONT
#include <sys/wait.h>     // waitpid()

typedef struct Buffer{
    int block, busy, next;
} Buffer;

Buffer b[3] = {{10,1,-1},{11,1,-1},{12,1,-1}};
int balance = 500;
int freeHead = -1;

void display(){
    int p = freeHead;

    printf("\nFree List: ");
    while(p != -1){
        printf("B%d -> ", p + 1);
        p = b[p].next;
    }
    printf("NULL\n");
}

void emi(int fd){
    if(balance < 800){
        printf("EMI: Sleeping - Scenario 4\n");
        raise(SIGSTOP);            // Stop process
    }
    read(fd, &b[0], sizeof(b[0]));  // Wait for deposit message
    balance -= 800;
    printf("EMI: ₹800 paid\n");
}

void withdraw(int fd){
    if(b[0].busy){
        printf("Withdrawal: B1 busy - Scenario 5\n");
        raise(SIGSTOP);            // Stop process
    }

    read(fd, &b[0], sizeof(b[0]));  // Wait for deposit message
    balance -= 600;
    printf("Withdrawal: ₹600 withdrawn\n");
}

void deposit(int fd){
    balance += 1000;
    b[0].busy = 0;
    freeHead = 0;
    b[0].next = -1;

    printf("Deposit: ₹1000 added\n");

    write(fd, &b[0], sizeof(b[0])); // Send message through pipe
    write(fd, &b[0], sizeof(b[0])); // Wake both processes
}

int main(){
    int pipefd[2];
    pipe(pipefd);                  // Create IPC pipe
    display();

    pid_t e = fork();              // Create EMI process
    if(e == 0){
        close(pipefd[1]);          // Close unused write end
        emi(pipefd[0]);
        return 0;
    }

    pid_t w = fork();              // Create Withdrawal process
    if(w == 0){
        close(pipefd[1]);          // Close unused write end
        withdraw(pipefd[0]);
        return 0;
    }

    int status;
    waitpid(e, &status, WUNTRACED); // Wait for EMI to stop
    waitpid(w, &status, WUNTRACED); // Wait for Withdrawal to stop

    pid_t d = fork();              // Create Deposit process
    if(d == 0){
        close(pipefd[0]);          // Close unused read end
        deposit(pipefd[1]);
        return 0;
    }

    waitpid(d, NULL, 0);           // Wait for Deposit

    kill(e, SIGCONT);              // Wake EMI
    kill(w, SIGCONT);              // Wake Withdrawal

    waitpid(e, NULL, 0);           // Wait for EMI to finish
    waitpid(w, NULL, 0);           // Wait for Withdrawal to finish

    close(pipefd[0]);
    close(pipefd[1]);

    printf("\nFinal Balance: ₹%d\n", balance);
    display();
    
    return 0;
}