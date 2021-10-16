#ifndef _DATA_TYPES_
#define _DATA_TYPES_
#include <stdint.h>

typedef uint32_t Word;

union Register {
        int i;
        float f;
        unsigned int ui; //mostly for the PC, maybe FP and SP as well
};

//a struct with all the different possible fields a vmx20 instruction can have
typedef struct {
    int opcode;
    int reg1;
    int reg2;
    int addr;
    int offset;
    union {
        int asInt;
        float asFloat;
    } constant;
} Instruction;

typedef struct {
    char *name;
    Word addr;
} Symbol;

typedef struct {
    int numInSymbols;
    int numOutSymbols;
    int objCodeLen;
    Symbol* inSymbols;
    Symbol* outSymbols;
    Word* objCode;
    int entryPointAddr;
} ObjFile;

#endif