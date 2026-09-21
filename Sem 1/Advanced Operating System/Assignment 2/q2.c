#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    int fd;

    open("sample.txt", O_RDONLY);

    if (fd == -1)    {
        perror("Error while opening file");
        exit(1);
    }

    printf("File opened successfully.\n");
    printf("Process ID: %d\n", getpid());

    printf("Sleeping for 5 seconds...\n");
    sleep(5);

    close(fd);

    printf("File closed. Program terminated.\n");

    return 0;
}