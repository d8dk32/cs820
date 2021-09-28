#include "dataTypes.h"

//2's complementer utility function. supports 20 bit and 16 bit (really n-bit where n < 32) lengths

int twosComplementer(int value, int numBits);

//functions for handling each instruction format

void op(Word instr, int pc);
void opAddr(Word instr, int pc);
void opReg(Word instr, int pc);
void opRegConst(Word instr, int pc);
void opRegAddr(Word instr, int pc);
void opRegReg(Word instr, int pc);
void opRegOffsetReg(Word instr, int pc);
void opRegRegAddr(Word instr, int pc);

//arrays to address into based on opcode - handler functions for each type of instruction format, and instruction names
void (*instrFormatHandlers[26])(Word, int);
char* opcodeNames[26]; 