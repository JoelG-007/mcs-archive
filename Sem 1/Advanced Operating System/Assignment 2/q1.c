#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    int fd;
    fd = open("holefile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if(fd < 0){
        perror("Error while opening file");
        exit(1);
    }

    write(fd, "BEGIN", 15);

    // Create a hole of 100 bytes
    lseek(fd, 1000, SEEK_CUR);

    write(fd, "END", 3);

    close(fd);

    printf("File with hole created successfully.\n");

    return 0;
}