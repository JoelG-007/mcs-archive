#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

int main(){
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    int count = 0;
    char filename[1024];

    dir = opendir(".");

    if (dir == NULL)    {
        printf("Unable to open directory.\n");
        return 1;
    }

    printf("\nCurrent Directory Contents\n");
    printf("-----------------------------------------------------------\n");
    printf("%-25s %-10s %-15s\n", "File Name", "Size", "Type");
    printf("-----------------------------------------------------------\n");

    while ((entry = readdir(dir)) != NULL)    {
        strcpy(filename, entry->d_name);

        if (stat(filename, &fileStat) == -1)
            continue;

        printf("%-25s %-10ld ", entry->d_name, fileStat.st_size);

        if (S_ISREG(fileStat.st_mode))
            printf("Regular File");
        else if (S_ISDIR(fileStat.st_mode))
            printf("Directory");
        else if (S_ISCHR(fileStat.st_mode))
            printf("Character Device");
        else if (S_ISBLK(fileStat.st_mode))
            printf("Block Device");
        else if (S_ISFIFO(fileStat.st_mode))
            printf("FIFO");
        else if (S_ISLNK(fileStat.st_mode))
            printf("Symbolic Link");
        else if (S_ISSOCK(fileStat.st_mode))
            printf("Socket");
        else
            printf("Unknown");

        printf("\n");
        count++;
    }

    closedir(dir);

    printf("-----------------------------------------------------------\n");
    printf("Total Number of Files: %d\n", count);

    return 0;
}