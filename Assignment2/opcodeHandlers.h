#include "dataTypes.h"

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