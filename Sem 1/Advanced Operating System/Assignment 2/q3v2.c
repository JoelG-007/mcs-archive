#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
void type(struct stat *st){
    if(S_ISREG(st->st_mode))
        printf("Regular File");
    else if(S_ISDIR(st->st_mode))
        printf("Directory");
    else
        printf("Other");
    }
        
int main(){
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    int count = 0;
    dir = opendir(".");
    if(dir == NULL){
        printf("Cannot open directory\n");
        return 1;
    }
    while((entry = readdir(dir)) != NULL){
        if(stat(entry->d_name, &st) == -1)
            continue;
        printf("%-20s %8ld bytes  ",entry->d_name, st.st_size);
        type(&st);
        printf("\n");
        count++;
    }
    printf("\nTotal files: %d\n", count-2);
    closedir(dir);
    return 0;
}