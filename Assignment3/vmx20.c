#include "vmx20FileReaderUtils.h"
#include "vmx20.h"

#define FP 13
#define SP 14
#define PC 15

ObjFile *currentExecutable;

int loadExecutableFile(char *filename, int *errorNumber) {

    char* blob = loadFile(filename);
    if (blob == NULL) {
        if (errorNumber != NULL){
            int errorCode = VMX20_FILE_NOT_FOUND;
            errorNumber = &errorCode; 
        }
        return 0;
    }

    currentExecutable = parseObjFile(blob);

    if (currentExecutable->numOutSymbols > 0){
        if (errorNumber != NULL){
            int errorCode = VMX20_FILE_CONTAINS_OUTSYMBOLS;
            errorNumber = &errorCode; 
        }
        return 0;
    }

    if (currentExecutable->objCodeLen <= 0 || currentExecutable->numInSymbols < 0){
        if (errorNumber != NULL){
            int errorCode = VMX20_FILE_IS_NOT_VALID;
            errorNumber = &errorCode; 
        }
        return 0;
    }

    unsigned int *mainx20;
    if(getAddress("mainx20", mainx20) == 0){
        if (errorNumber != NULL){
            int errorCode = VMX20_FILE_IS_NOT_VALID;
            errorNumber = &errorCode; 
        }
        return 0;
    }

    return 1;
}

int getAddress(char *label, unsigned int *outAddr){
    //search insymbols for one with that name
    for(int i = 0; i < currentExecutable->numInSymbols; i++){
        if(strcmp(label, currentExecutable->inSymbols[i].name) == 0){
            outAddr = &currentExecutable->inSymbols[i].addr;
            return 1;
        }
    }

    //should do a check for VMX20_ADDRESS_OUT_OF_RANGE here

    // symbol not found
    outAddr = NULL;
    return 0;
}

int getWord(unsigned int addr, int *outWord) {
    // there's gotta be some error checking here but I don't know what yet
    outWord = &currentExecutable->objCode[addr];
    return 1;
}

int putWord(unsigned int addr, int word){
    // there's gotta be some error checking here but I don't know what yet
    currentExecutable->objCode[addr] = word;
    return 1;
}

int execute(unsigned int numProcessors, unsigned int initialSP[], int terminationStatus[], int trace) {
    terminationStatus = calloc(numProcessors, sizeof(int));

    unsigned int *entryPoint;
    int _a = getAddress("mainx20", entryPoint);  

    //init registers
    //Registers can be accessed in the form of r[pid][reg#], so each processor has its own set of registers
    //the PC, FP, and SP count as registers. Constants are defined for these for clarity
    union Register **r;
    r = calloc(numProcessors, sizeof(union Register *));
    for(int i = 0; i < numProcessors; i++){
        r[i] = calloc(16, sizeof(union Register));
    }

    for(int i = 0; i < numProcessors; i++){
        //set initial PC - start at 'mainx20' for now, but maybe it shold be 0
        r[i][PC].ui = *entryPoint;
    }

    //begin the fetch/execute loop
    int haltExec = 0;
    while (haltExec < 1) {
        //just using a single processor for now but eventually we'll need to do multiprocessing as well
        Word *op;
        int _b = getWord(r[0][PC].ui, op);
        r[0][PC].ui += 1;
        //process the opcode

    }
    
    return 1;
}

