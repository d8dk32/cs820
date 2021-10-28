#include <stdio.h>
#include <stdlib.h>
#include "vmx20.h"

void assertEquals(char* exportedSymbolName, int expectedValue){
    unsigned int outAddr = 0;
    int outWord = 0;

    int addrFound = getAddress(exportedSymbolName, &outAddr);
    if (addrFound == 0){
        printf("Fail: Symbol %s not found.\n", exportedSymbolName);
    }
    getWord(outAddr, &outWord);

    char* passFail = outWord == expectedValue ? "Pass" : "Fail";
    printf("%s: %s - Expected: %d, Actual: %d\n", passFail, exportedSymbolName, expectedValue, outWord);
}

void assertEqualsFloat(char* exportedSymbolName, float expectedValue){
    unsigned int outAddr = 0;
    int outWord = 0;

    int addrFound = getAddress(exportedSymbolName, &outAddr);
    if (addrFound == 0){
        printf("Fail: Symbol %s not found.\n", exportedSymbolName);
    }
    
    getWord(outAddr, &outWord);

    float outFloat = *((float*) &outWord);

    char* passFail = outFloat == expectedValue ? "Pass" : "Fail";
    printf("%s: %s - Expected: %f, Actual: %f\n", passFail, exportedSymbolName, expectedValue, outFloat);
}

int main(int argc, char** argv){
    int numProcessors = 1;
    char *filename = "";
    int errNum = 666;
    int error = 0;

    //-------Test 1: Loading and Storing---------------------------
    filename = "testLoadStore.exe";
    error = loadExecutableFile(filename, &errNum);

    if (error != 0){
        printf("Executing '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {1};
        error = execute(1, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        for(int i = 0; i < numProcessors; i++){
            printf("Processor %d termination status: %d\n", i, terminationStatus[i]);
        }
    }

    assertEquals("y1", 1);
    assertEquals("z1", 1);
    assertEquals("w1", 1);
    assertEquals("u1", 1);

    //-------Test 2: Integer Math--------------------------- 
    filename = "IntegerMath.exe";
    error = loadExecutableFile(filename, &errNum);

    if (error != 0){
        printf("Executing '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {1};
        error = execute(1, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        for(int i = 0; i < numProcessors; i++){
            printf("Processor %d termination status: %d\n", i, terminationStatus[i]);
        }
    }

    assertEquals("add", 5);
    assertEquals("sub", 2);
    assertEquals("mul", 6);
    assertEquals("div", 2);

    //-------Test 3: Floating Point Math--------------------------- 
    filename = "FloatMath.exe";
    error = loadExecutableFile(filename, &errNum);

    unsigned int float1 = 0;
    unsigned int float2 = 0;
    getAddress("float1", &float1);
    getAddress("float2", &float2);

    float f1 = 2.0f;
    float f2 = 3.0f;

    putWord(float1, *((int*) &f1));
    putWord(float2, *((int*) &f2));

    if (error != 0){
        printf("Executing '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {1};
        error = execute(1, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        for(int i = 0; i < numProcessors; i++){
            printf("Processor %d termination status: %d\n", i, terminationStatus[i]);
        }
    }

    assertEqualsFloat("add", 5.0f);
    assertEqualsFloat("sub", 2.0f);
    assertEqualsFloat("mul", 6.0f);
    assertEqualsFloat("div", 2.0f);

    //-------Test 4: Branching---------------------------

    filename = "BranchingTest.exe";
    error = loadExecutableFile(filename, &errNum);

    unsigned int x = 0;
    getAddress("x", &x);

    if (error != 0){
        printf("Executing '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {1};
        error = execute(1, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        for(int i = 0; i < numProcessors; i++){
            printf("Processor %d termination status: %d\n", i, terminationStatus[i]);
        }
    }

    assertEquals("z", 1);

    putWord(x, 2);

    if (error != 0){
        printf("Executing '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {1};
        error = execute(1, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        for(int i = 0; i < numProcessors; i++){
            printf("Processor %d termination status: %d\n", i, terminationStatus[i]);
        }
    }

    assertEquals("z", 2);

    putWord(x, 1);

    if (error != 0){
        printf("Executing '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {1};
        error = execute(1, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        for(int i = 0; i < numProcessors; i++){
            printf("Processor %d termination status: %d\n", i, terminationStatus[i]);
        }
    }

    assertEquals("z", 3);

    //-------Test 5: Function Call, Push and Pop---------------------------
    filename = "TestCallPushAndPop.exe";
    error = loadExecutableFile(filename, &errNum);

    if (error != 0){
        printf("Executing '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {1};
        error = execute(1, initSP, terminationStatus, 1);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        for(int i = 0; i < numProcessors; i++){
            printf("Processor %d termination status: %d\n", i, terminationStatus[i]);
        }
    }

    assertEquals("result", 84);
    assertEquals("popResult", 1);

    //-------Test 6: Multiprocessing/The Leftover Opcodes - getpid, getpn, cmpxchg------------

    // filename = "TestCallPushAndPop.exe";
    // error = loadExecutableFile(filename, &errNum);

    // if (error != 0){
    //     printf("Executing '%s'------------------------\n", filename);
    //     int initSP[] = {1000};
    //     int terminationStatus[] = {1};
    //     error = execute(1, initSP, terminationStatus, 1);
    //     printf("Execution ended--------------------------\n");
    //     printf("Execute() output: %d\n", error);
    //     for(int i = 0; i < numProcessors; i++){
    //         printf("Processor %d termination status: %d\n", i, terminationStatus[i]);
    //     }
    // }

    // assertEquals("result", 84);
    // assertEquals("popResult", 1);

 
}