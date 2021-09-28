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

int twosComplementer(int value, int numBits){
    if(value >> (numBits-1) == 0) {
        //this is a positive number
        return value;
    }
    
    //x + 2sComp(x) = 2^N where N is the number of bits in x
    // so 2^N - X = 2sComp(x)
    int twoToTheN = 1 << numBits;
    int twosComp = value-twoToTheN; 
}

void op(Word instr, int pc) {
    //this op takes no params
    int opcode = 0x000000FF & instr; 
    printf("%07d   %08x %8s\n", pc-1, instr, opcodeNames[opcode]);
}

void opAddr(Word instr, int pc) {
    int opcode = 0x000000FF & instr; 
    //int addr = instr >> 12;
    int addr =  twosComplementer(instr >> 12, 20); //| 0xFFF00000;
    printf("%07d   %08x %8s %d\n", pc-1, instr, opcodeNames[opcode], pc+addr);
}

//TODO
void opReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    printf("%07d   %08x %8s r%d\n", pc-1, instr, opcodeNames[opcode], reg1);
}

//TODO
void opRegConst(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int constant = twosComplementer(instr >> 12, 20);
    printf("%07d   %08x %8s r%d, %d\n", pc-1, instr, opcodeNames[opcode], reg1, constant);
}

void opRegAddr(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int addr =  twosComplementer(instr >> 12, 20); 
    printf("%07d   %08x %8s r%d, %d\n", pc-1, instr, opcodeNames[opcode], reg1, pc+addr);
}

//TODO
void opRegReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    printf("%07d   %08x %8s r%d, r%d\n", pc-1, instr, opcodeNames[opcode], reg1, reg2);
}

//TODO
void opRegOffsetReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int offset = twosComplementer(instr >> 16, 16);
    printf("%07d   %08x %8s r%d, %d(r%d)\n", pc-1, instr, opcodeNames[opcode], reg1, offset, reg2);
}

//TODO
void opRegRegAddr(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int addr = twosComplementer(instr >> 16, 16);
    printf("%07d   %08x %8s r%d, r%d, %d\n", pc-1, instr, opcodeNames[opcode], reg1, reg2, pc+addr);
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