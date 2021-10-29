#include <stdio.h>
//#include "vmx20FileReaderUtils.h"
#include "vmx20.h"
//#include "opHandlers.h"

//some hariy stuff
#include "opHandlers.c"
#include "vmx20FileReaderUtils.c"

#define ONE_MB 1024*1024

ObjFile *currentExecutable;
union Register **r;
unsigned int* initialSP; 
int* terminationStatus; 
int trace;

int (*instructionHandlers[26])(Word, int, union Register**, ObjFile *, int); 
void (*instrFormatHandlers[26])(Word, int, char*);

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

    Word* workingMemory = calloc(ONE_MB, sizeof(char));
    memcpy(workingMemory, currentExecutable->objCode, sizeof(Word)*currentExecutable->objCodeLen);
    currentExecutable->objCode = workingMemory;

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


void * threadWorker(void * _pid){
    int pid = (int)(long) _pid; //all this horrible casting to make warnings go away

    //begin the fetch/execute loop
    while (1) {

        //just using a single processor for now but eventually we'll need to do multiprocessing as well
        Word op;
        int _b = getWord(r[pid][PC].ui, &op);

        r[pid][PC].ui += 1;
        if(r[pid][PC].ui > MAX_ADDR){
            terminationStatus[pid] = VMX20_ADDRESS_OUT_OF_RANGE;
            return (void *)(long) VMX20_ADDRESS_OUT_OF_RANGE;
        }

        //printf("Word: %08x\n", op);
        //process the opcode
        int opcode = 0x000000FF & op;
        if (opcode > 25) {
            terminationStatus[pid] = VMX20_ILLEGAL_INSTRUCTION;
            return (void *)(long) VMX20_ILLEGAL_INSTRUCTION;
        }

        int execRetVal = (*instructionHandlers[opcode])(op, pid, r, currentExecutable, trace);

        if (execRetVal < 1){
            //error code returned
            terminationStatus[pid] = execRetVal;
            return (void *)(long) execRetVal;
        }

    }
}

int execute(unsigned int numProcessors, unsigned int _initialSP[], int _terminationStatus[], int _trace) {  

    totalNumProcessors = numProcessors;
    initialSP = _initialSP;
    terminationStatus = _terminationStatus;
    trace = _trace;


    pthread_t pt[VMX20_MAX_PROCESSORS];

    //init registers
    //Registers can be accessed in the form of r[pid][reg#], so each processor has its own set of registers
    //the PC, FP, and SP count as registers. Constants are defined for these for clarity
    
    r = calloc(numProcessors, sizeof(union Register*));
    for(int i = 0; i < 16; i++){
        r[i] = calloc(16, sizeof(union Register));
    }

    //can't figure out why calloc doesn't zero them out, maybe because of the union type?
    //anyway, zeroing the registers out manually first
    for(int i = 0; i < numProcessors; i++){
        for(int j = 0; j < 16; j++){
            r[i][j].ui = 0;
        }
    }

    //there's some minor speed improvements that could be made here
    //to not loop so many times but its not worth it to me right now
    for(int i = 0; i < numProcessors; i++){
        //set initial Sp
        r[i][SP].ui = initialSP[i];
        //set initial PC to mainx20(?)
        unsigned int initPC = 0;
        int _a = getAddress("mainx20", &initPC);
        r[i][PC].ui = initPC;
    }

    for(int i = 0; i < numProcessors && i < VMX20_MAX_PROCESSORS; i++){
        if (pthread_create(&pt[i], NULL, threadWorker, (void *)(long) i) != 0){
            fprintf(stderr, "Error creating thread %d\n", i);
        }
    }

    for(int i = 0; i < numProcessors && i < VMX20_MAX_PROCESSORS; i++){
        if (pthread_join(pt[i], NULL))
        {
            fprintf(stderr, "Error joining thread %d\n", i);
        }
    }

    return 1;
}

int disassemble(unsigned int address, char *buffer, int *errorNumber){
    if (address > MAX_ADDR) {
        *errorNumber = VMX20_ADDRESS_OUT_OF_RANGE;
        return 0;
    }

    Word instr = currentExecutable->objCode[address];
    int opcode = 0x000000FF & instr;

    if (opcode > 25) {
        *errorNumber = VMX20_ILLEGAL_INSTRUCTION;
        return 0;
    }

    (*instrFormatHandlers[opcode])(instr, address+1, buffer);

    return 1;

}

int (*instructionHandlers[26])(Word, int, union Register**, ObjFile *, int) = {
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

//index in by opcode, get the function that can parse the whole word properly
void (*instrFormatHandlers[26])(Word, int, char*) = {
    op,
    opRegAddr,
    opRegAddr,
    opRegConst,
    opRegAddr,
    opRegOffsetReg,
    opRegOffsetReg,
    opRegReg,
    opRegReg,
    opRegReg,
    opRegReg,
    opRegReg,
    opRegReg,
    opRegReg,
    opRegReg,
    opAddr,
    op,
    opRegRegAddr,
    opRegRegAddr,
    opRegRegAddr,
    opAddr,
    opRegRegAddr,
    opReg,
    opReg,
    opReg,
    opReg
};