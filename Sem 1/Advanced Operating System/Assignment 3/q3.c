#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
int main(){
    int p1[2], p2[2];
    /*
    pipe[0] -> read end
    pipe[1] -> write end
    */
    char msg[100];
    pipe(p1);
    pipe(p2);
    if(fork() == 0){
        // Child Executing
        close(p1[1]);   // Parent - write    
        close(p2[0]);   // Child won't read Pipe 2.

        read(p1[0], msg, 100);
        printf("Child: %s\n", msg);

        strcpy(msg, "Hello Parent");
        write(p2[1], msg, strlen(msg)+1);

        close(p1[0]);
        close(p2[1]);
    }
    else{
        //Parent Executing
        close(p1[0]);   // Parent won't read from Pipe 1.
        close(p2[1]);   // Child - write

        strcpy(msg, "Hello Child");
        write(p1[1], msg, strlen(msg)+1);   

        read(p2[0], msg, 100);
        printf("Parent: %s\n", msg);

        close(p1[1]);
        close(p2[0]);

        wait(NULL);
    }
    return 0;
}

/*
Program Flow:
Two pipes are created.
fork() gives both processes access to them.
Parent writes to Pipe 1.
Child reads from Pipe 1.
Child writes to Pipe 2.
Parent reads from Pipe 2.
Parent waits for child and exits.
*/

/*
Why?
A pipe is half-duplex, meaning data flows in only one direction.
You cannot send data back through the same pipe.
So another pipe is needed.
*/