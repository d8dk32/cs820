#include "dataTypes.h"
#include <stdio.h>

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
    printf("%08x %8s\n", instr, opcodeNames[opcode]);
}

void opAddr(Word instr, int pc) {
    int opcode = 0x000000FF & instr; 
    //int addr = instr >> 12;
    int addr =  (instr >> 12) | 0xFFF00000;
    int twosCompAddr = -1*((0xFFFFFFFF ^ addr)+1);
    //printf("2sCompAddr: %08x ", addr);
    //printf("int addr: %d ", twosCompAddr);
    printf("%08x %8s %d\n", instr, opcodeNames[opcode], pc+twosCompAddr);
}

//TODO
void opReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    printf("%08x %8s r%d\n", instr, opcodeNames[opcode], reg1);
}

//TODO
void opRegConst(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int constant = instr >> 12;
    printf("%08x %8s r%d, %d\n", instr, opcodeNames[opcode], reg1, constant);
}

void opRegAddr(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int addr =  (instr >> 12) | 0xFFF00000;
    int twosCompAddr = -1*((0xFFFFFFFF ^ addr)+1);
    //printf("2sCompAddr: %08x ", addr);
    //printf("int addr: %d ", twosCompAddr);
    printf("%08x %8s r%d, %d\n", instr, opcodeNames[opcode], reg1, pc+twosCompAddr);
}

//TODO
void opRegReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    printf("%08x %8s r%d, r%d\n", instr, opcodeNames[opcode], reg1, reg2);
}

//TODO
void opRegOffsetReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int offset = (instr >> 16) | 0xFFFF0000;
    int twosCompOffset = -1*((0xFFFFFFFF ^ offset)+1);
    printf("%08x %8s r%d, %d(r%d)\n", instr, opcodeNames[opcode], reg1, twosCompOffset, reg2);
}

//TODO
void opRegRegAddr(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int addr = (instr >> 16) | 0xFFFF0000;
    int twosCompAddr = -1*((0xFFFFFFFF ^ addr)+1);
    printf("%08x %8s r%d, r%d, %d\n", instr, opcodeNames[opcode], reg1, reg2, pc+twosCompAddr);
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
};