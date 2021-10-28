#ifndef _OP_HANDLERS_
#define _OP_HANDLERS_

#include "dataTypes.h"

#define FP 13
#define SP 14
#define PC 15
#define MAX_ADDR 262144

//----------Utilities-----------------------

int twosComplementer(int value, int numBits);

void traceRegisters(int pid, union Register** r);

// ------------ op handlers -------------------

int halt(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int load(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int store(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int ldimm(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int ldaddr(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int ldind(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int stind(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int addf(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int subf(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int divf(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int mulf(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int addi(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int subi(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int divi(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int muli(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int call(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int ret(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int blt(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int bgt(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int beq(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int jmp(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int cmpxchg(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int getpid(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int getpn(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int push(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

int pop(Word instr, int pid, union Register** r, ObjFile *curExec, int trace);

//---------------Disassembler instr format handlers--------------------------

void op(Word instr, int pc, char*); 

void opAddr(Word instr, int pc, char*);

void opReg(Word instr, int pc, char*); 

void opRegConst(Word instr, int pc, char*);

void opRegAddr(Word instr, int pc, char*);

void opRegReg(Word instr, int pc, char*);

void opRegOffsetReg(Word instr, int pc, char*);

void opRegRegAddr(Word instr, int pc, char*);

#endif