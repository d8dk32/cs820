#include <stdio.h>
#include "vmx20.h"

int main(int argc, char** argv){
    //TEST INIT------------------------------------------
    int numProcessors = 1;
    char* filename = "simple.obj";
    //END TEST INIT--------------------------------------

    int errNum = 666;
    int error = loadExecutableFile(filename, &errNum);

    if (error != 0){
        printf("File '%s' loaded successfully\n", filename);
        printf("Executing-------------------------\n");
        int initSP[] = {1000};
        int terminationStatus[] = {1};
        error = execute(1, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        for(int i = 0; i < numProcessors; i++){
            printf("Processor %d termination status: %d\n", i, terminationStatus[i]);
        }
    }

    unsigned int outAddr = 0;
    getAddress("y1", &outAddr);
    int outWord = 0;
    getWord(outAddr, &outWord);
    printf("y1: %d\n", outWord);


}