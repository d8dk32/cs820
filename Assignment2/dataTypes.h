#include <stdint.h>

typedef uint32_t Word;

//a struct with all the different possible fields a vmx20 instruction can have
typedef struct {
    int opcode;
    int reg1;
    int reg2;
    int addr;
    int offset;
    int constant;
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
} ObjFile;