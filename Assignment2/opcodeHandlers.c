#include "dataTypes.h"

char* opcodeNames[] = {
    "halt",
    "load",
    "store",
    "ldimm",
    "ldaddr",
    "ldind",
    "stind",
    "addf",
    "subf",
    "divf",
    "mulf",
    "addi",
    "subi",
    "divi",
    "muli",
    "call",
    "ret",
    "blt",
    "bgt",
    "beq",
    "jmp",
    "cmpxchg",
    "getpid",
    "getpn",
    "push",
    "pop"
};

void op(Word instr, int pc) {
    //this op takes no params
    int opcode = 0x000000FF & instr; 
    printf("%s\n", opcodeNames[opcode]);
}

void opAddr(Word instr, int pc) {
    int opcode = 0x000000FF & instr; 
}

void opReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
}

void opRegConst(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
}

void opRegAddr(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
}

void opRegReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
}

void opRegOffsetReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
}

void opRegRegAddr(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
}

//index in by opcode, get the function that can parse the whole word properly
void (*instrFormatHandlers[26])(Word, int) = {
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
}