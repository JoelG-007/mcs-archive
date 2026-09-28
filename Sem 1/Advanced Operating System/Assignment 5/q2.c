#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
    pid_t child;
    int i;

    child = fork();

    if(child < 0){
        printf("Fork failed\n");
        return 1;
    }
    if(child == 0){
        printf("Child: Running\n");

        for(i = 1; i <= 10; i++){
            printf("Child: Working %d\n", i);
            sleep(1);
        }

        printf("Child: Finished\n");
        exit(0);
    }else{
        sleep(2);

        kill(child, SIGSTOP);
        printf("Parent: Child suspended\n");

        sleep(3);

        kill(child, SIGCONT);
        printf("Parent: Child resumed\n");

        wait(NULL);
    }

    return 0;
}
/*
Time       Child                     Parent
-----------------------------------------------------
0 sec      Working 1                 sleep(2)
1 sec      Working 2                 sleep
2 sec      Working 3                 SIGSTOP
           |
           V
           STOPPED                   "Child suspended"
                                     sleep(3)

3 sec      STOPPED                   sleeping
4 sec      STOPPED                   sleeping
5 sec      STOPPED                   SIGCONT
                                     "Child resumed"

5+ sec     continues working
6 sec      Working ...
7 sec      Working ...
...
eventually
           Working 10
           Finished
           exit(0)

                                     wait() returns
                                     parent exits

*/