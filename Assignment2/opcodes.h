//a struct representing one of Vmx20's 32bit words.
//not sure if I'll actually get all that much use out of this but
//it could be useful
typedef struct {
    char byte4;
    char byte3;
    char byte2;
    char byet1;
} Word;

//different opcode types. Not sure if I'll use all of these either, might just use the Instruction type below these.
typedef struct {
    int opcode;
} Op;

typedef struct {
    int opcode;
    int addr;
} OpAddr;

typedef struct {
    int opcode;
    int reg1;
} OpReg;

typedef struct {
    int opcode;
    int reg1;
    int constant;
} OpRegConst;

typedef struct {
    int opcode;
    int reg1;
    int addr;
} OpRegAddr;

typedef struct {
    int opcode;
    int reg1;
    int reg2;
} OpRegReg;

typedef struct {
    int opcode;
    int reg1;
    int reg2;
    int offset;
} OpRegOffsetReg;

typedef struct {
    int opcode;
    int reg1;
    int reg2;
    int addr;
} OpRegRegAddr;

//a struct with all the different possible fields a vmx20 instruction can have
typedef struct {
    int opcode;
    int reg1;
    int reg2;
    int addr;
    int offset;
    int constant;
} Instruction;