#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>

int main(){
    int n, i;
    struct rusage usage;

    printf("Enter number of children: ");
    scanf("%d", &n);

    for(i = 0; i < n; i++){
        if(fork() == 0)
            exit(0);
    }

    for(i = 0; i < n; i++)
        wait(NULL);
    
    // returns resource usage measures for who
    getrusage(RUSAGE_CHILDREN, &usage); // Returns resource usage like CPU time

    printf("User Time : %ld.%06ld sec\n", 
        usage.ru_utime.tv_sec /* user CPU time used (in seconds) */, 
        usage.ru_utime.tv_usec /* user CPU time used (in microseconds) */);  

    printf("Kernel Time : %ld.%06ld sec\n", 
        usage.ru_stime.tv_sec /* system CPU time used (in seconds) */, 
        usage.ru_stime.tv_usec /* system CPU time used (in microseconds) */);
    return 0;
}

/* 
RUSAGE_CHILDREN
Return resource usage statistics for all children of the
calling process that have terminated and been waited for.
These statistics will include the resources used by
grandchildren, and further removed descendants, if all of
the intervening descendants waited on their terminated
children.
*/

/*
Program Flow: 
1. Parent creates children.
2. Children terminate.
3. Parent waits for all children.
4. OS stores CPU usage.
5. getrusage() retrieves it.
*/