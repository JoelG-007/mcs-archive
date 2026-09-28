#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig){
    if(sig == SIGINT)
        printf("\nSIGINT received - Ctrl+C pressed\n");
    else if(sig == SIGTERM){
        printf("SIGTERM received - Process terminating\n");
        exit(0);
    }
}

int main(){
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    printf("Process running. PID = %d\n", getpid());
    while(1)
        pause();

    return 0;
}
