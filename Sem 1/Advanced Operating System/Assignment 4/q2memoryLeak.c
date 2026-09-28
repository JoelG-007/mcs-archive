#include <stdio.h>
#include <stdlib.h>

int main(){
    int *ptr;
    ptr = (int *)malloc(5 * sizeof(int));

    if(ptr == NULL){
        printf("Memory allocation failed.\n");
        return 1;
    }

    for(int i = 0; i < 5; i++){
        ptr[i] = i + 1;
    }

    printf("Memory allocated successfully.\n");
    // Memory is not released using free(). This creates a memory leak.
    // A memory leak occurs when dynamically allocated memory is no longer needed but the program has failed to release it.
    return 0;
}