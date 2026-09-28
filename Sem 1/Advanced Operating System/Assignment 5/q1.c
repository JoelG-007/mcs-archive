#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void handler(int sig){ // This function is called when the child receives one of the signals for which it registered the handler.
    if(sig == SIGHUP)
        printf("Child: SIGHUP received\n");
    else if(sig == SIGINT)
        printf("Child: SIGINT received\n");
    else if(sig == SIGQUIT){
        printf("My DADDY has Killed me!!!\n");
        exit(0);       // terminates the child.
    }
}

int main(){
    pid_t child;
    int i;
    child = fork();
    if(child == 0){
        // In Child
        // The child installs three signal handlers
        signal(SIGHUP, handler);
        signal(SIGINT, handler);
        signal(SIGQUIT, handler);
        while(1)
            pause();    // causes the process to sleep until a signal is delivered.
    }else{  // child > 0
        // In Parent
        for(i = 3; i <= 30; i += 3){
            sleep(3);
            if(i == 30)
                kill(child, SIGQUIT);
            else if((i / 3) % 2 == 1)
                kill(child, SIGHUP);
            else
                kill(child, SIGINT);
        }
        wait(NULL);
    }
    return 0;
}

/*
i	i/3	(i/3)%2	Signal
3	1	1	    SIGHUP
6	2	0	    SIGINT
9	3	1	    SIGHUP
12	4	0	    SIGINT
15	5	1	    SIGHUP
18	6	0	    SIGINT
21	7	1	    SIGHUP
24	8	0	    SIGINT
27	9	1	    SIGHUP
30	10	—	    SIGQUIT
*/

/*
Signal = A signal is an asynchronous notification sent to a process to inform it that some event has occurred.
Signal Handler = A signal handler is a function that executes when a particular signal is received.
SIGINT is traditionally generated when the user interrupts a process, usally with ctrl + c
SIGHUP It can be generated in situations involving a terminal/session disconnect, and programs can also send it explicitly, as this program does.
SIGQUIT is a signal with a default termination behavior.
signal() = Registers what the process should do when it receives a signal.
kill() = Sends a signal to another process.
*/