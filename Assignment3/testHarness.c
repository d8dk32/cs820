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
        printf("\nExecuting '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {0};
        error = execute(numProcessors, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        printf("Termination Statuses: [ ");
        for(int i = 0; i < numProcessors; i++){
            printf("%d ", terminationStatus[i]);
        } printf("]\n");
    }

    assertEquals("y1", 1);
    assertEquals("z1", 1);
    assertEquals("w1", 1);
    assertEquals("u1", 1);

    //-------Test 2: Integer Math--------------------------- 
    filename = "IntegerMath.exe";
    error = loadExecutableFile(filename, &errNum);

    if (error != 0){
        printf("\nExecuting '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {0};
        error = execute(numProcessors, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        printf("Termination Statuses: [ ");
        for(int i = 0; i < numProcessors; i++){
            printf("%d ", terminationStatus[i]);
        } printf("]\n");
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
        printf("\nExecuting '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {0};
        error = execute(numProcessors, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        printf("Termination Statuses: [ ");
        for(int i = 0; i < numProcessors; i++){
            printf("%d ", terminationStatus[i]);
        } printf("]\n");
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
        printf("\nExecuting '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {0};
        error = execute(numProcessors, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        printf("Termination Statuses: [ ");
        for(int i = 0; i < numProcessors; i++){
            printf("%d ", terminationStatus[i]);
        } printf("]\n");
    }

    assertEquals("z", 1);

    putWord(x, 2);

    if (error != 0){
        printf("\nExecuting '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {0};
        error = execute(numProcessors, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        printf("Termination Statuses: [ ");
        for(int i = 0; i < numProcessors; i++){
            printf("%d ", terminationStatus[i]);
        } printf("]\n");
    }

    assertEquals("z", 2);

    putWord(x, 1);

    if (error != 0){
        printf("\nExecuting '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {0};
        error = execute(numProcessors, initSP, terminationStatus, 0);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        printf("Termination Statuses: [ ");
        for(int i = 0; i < numProcessors; i++){
            printf("%d ", terminationStatus[i]);
        } printf("]\n");
    }

    assertEquals("z", 3);

    //-------Test 5: Function Call, Push and Pop---------------------------
    filename = "TestCallPushAndPop.exe";
    error = loadExecutableFile(filename, &errNum);

    if (error != 0){
        printf("\nExecuting '%s'------------------------\n", filename);
        int initSP[] = {1000};
        int terminationStatus[] = {0};
        error = execute(numProcessors, initSP, terminationStatus, 1);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        printf("Termination Statuses: [ ");
        for(int i = 0; i < numProcessors; i++){
            printf("%d ", terminationStatus[i]);
        } printf("]\n");
    }

    assertEquals("result", 84);
    assertEquals("popResult", 1);

    //-------Test 6: Multiprocessing Basics--------------------
    numProcessors = 4;
    filename = "MultiProcessorTest.exe";
    error = loadExecutableFile(filename, &errNum);

    if (error != 0){
        printf("\nExecuting '%s'------------------------\n", filename);
        int initSP[] = {1000, 2000, 3000, 4000};
        int terminationStatus[] = {0, 0, 0, 0};
        error = execute(numProcessors, initSP, terminationStatus, 1);
        printf("Execution ended--------------------------\n");
        printf("Execute() output: %d\n", error);
        printf("Termination Statuses: [ ");
        for(int i = 0; i < numProcessors; i++){
            printf("%d ", terminationStatus[i]);
        } printf("]\n");
    }

    assertEquals("x1", 0);
    assertEquals("x2", 1);
    assertEquals("x3", 2);
    assertEquals("x4", 3);
    assertEquals("pn", 4);
 
}