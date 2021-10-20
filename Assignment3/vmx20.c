#include <stdio.h>
#include "vmx20FileReaderUtils.h"
#include "vmx20.h"
#include "opHandlers.h"

ObjFile *currentExecutable;
int (*instructionHandlers[26])(Word, int, union Register**, ObjFile *); 

int loadExecutableFile(char *filename, int *errorNumber) {

    char* blob = loadFile(filename);
    if (blob == NULL) {
        if (errorNumber != NULL){
            *errorNumber = VMX20_FILE_NOT_FOUND; 
        }
        return 0;
    }

    currentExecutable = parseObjFile(blob);

    if (currentExecutable->numOutSymbols > 0){
        if (errorNumber != NULL){
            *errorNumber = VMX20_FILE_CONTAINS_OUTSYMBOLS;
        }
        return 0;
    }

    if (currentExecutable->objCodeLen <= 0 || currentExecutable->numInSymbols < 0){
        if (errorNumber != NULL){
            *errorNumber = VMX20_FILE_IS_NOT_VALID;
        }
        return 0;
    }

    unsigned int mainx20 = 0;
    if(getAddress("mainx20", &mainx20) == 0){
        if (errorNumber != NULL){
            *errorNumber = VMX20_FILE_IS_NOT_VALID;
        }
        return 0;
    }
    return 1;
}

int getAddress(char *label, unsigned int *outAddr){
    //search insymbols for one with that name
    for(int i = 0; i < currentExecutable->numInSymbols; i++){
        if(strcmp(label, currentExecutable->inSymbols[i].name) == 0){
            *outAddr = currentExecutable->inSymbols[i].addr;
            return 1;
        }
    }

    //should do a check for VMX20_ADDRESS_OUT_OF_RANGE here?

    // symbol not found
    return 0;
}

int getWord(unsigned int addr, int *outWord) {
    // there's gotta be some error checking here but I don't know what yet
    *outWord = currentExecutable->objCode[addr];
    return 1;
}

int putWord(unsigned int addr, int word){
    // there's gotta be some error checking here but I don't know what yet
    currentExecutable->objCode[addr] = word;
    return 1;
}

int execute(unsigned int numProcessors, unsigned int initialSP[], int terminationStatus[], int trace) {  

    //init registers
    //Registers can be accessed in the form of r[pid][reg#], so each processor has its own set of registers
    //the PC, FP, and SP count as registers. Constants are defined for these for clarity
    union Register **r;
    r = calloc(numProcessors, sizeof(union Register *));
    for(int i = 0; i < 16; i++){
        r[i] = calloc(16, sizeof(union Register));
    }

    for(int i = 0; i < numProcessors; i++){
        //set initial Sp
        r[i][SP].ui = initialSP[i];
        //set initial PC to mainx20(?)
        unsigned int initPC = 0;
        int _a = getAddress("mainx20", &initPC);
        r[i][PC].ui = initPC;
    }

    //begin the fetch/execute loop
    int haltExec = 0;
    while (haltExec < 1) {

        //just using a single processor for now but eventually we'll need to do multiprocessing as well
        Word op;
        int _b = getWord(r[0][PC].ui, &op);
        r[0][PC].ui += 1;
        if(r[0][PC].ui > MAX_ADDR){
            terminationStatus[0] = VMX20_ADDRESS_OUT_OF_RANGE;
            return VMX20_ADDRESS_OUT_OF_RANGE;
        }
        //printf("Word: %08x\n", op);
        //process the opcode
        int opcode = 0x000000FF & op;
        if (opcode > 25) {
            terminationStatus[0] = VMX20_ILLEGAL_INSTRUCTION;
            return VMX20_ILLEGAL_INSTRUCTION;
        }

        int execRetVal = (*instructionHandlers[opcode])(op, 0, r, currentExecutable);

        if (execRetVal < 1){
            //error code returned
            terminationStatus[0] = execRetVal;
            //return execRetVal;
            haltExec = 1;
        }

    }
    
    return 1;
}

int (*instructionHandlers[26])(Word, int, union Register**, ObjFile *) = {
    halt,
    load,
    store,
    ldimm,
    ldaddr,
    ldind,
    stind,
    addf,
    subf,
    divf,
    mulf,
    addi,
    subi,
    divi,
    muli,
    call,
    ret,
    blt,
    bgt,
    beq,
    jmp,
    cmpxchg,
    getpid,
    getpn,
    push,
    pop
};

