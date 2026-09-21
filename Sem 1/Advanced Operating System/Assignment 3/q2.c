#include <stdio.h>
#include <stdlib.h>
void end(){
    printf("Program Terminated\n");
}
int main(){
    atexit(end);    // register a function to be called at normal process termination
    printf("Inside Main\n");
    return 0;
}
/*
Program Flow: 
main() -> atexit(end) -> return 0 -> exit() -> end()-> Program terminates
*/