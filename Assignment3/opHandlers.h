#ifndef _OP_HANDLERS_
#define _OP_HANDLERS_

#include "dataTypes.h"

#define FP 13
#define SP 14
#define PC 15
#define MAX_ADDR 262144

int twosComplementer(int value, int numBits);

int halt(Word instr, int pid, union Register** r, ObjFile *curExec);

int load(Word instr, int pid, union Register** r, ObjFile *curExec);

int store(Word instr, int pid, union Register** r, ObjFile *curExec);

int ldimm(Word instr, int pid, union Register** r, ObjFile *curExec);

int ldaddr(Word instr, int pid, union Register** r, ObjFile *curExec);

int ldind(Word instr, int pid, union Register** r, ObjFile *curExec);

int stind(Word instr, int pid, union Register** r, ObjFile *curExec);

int addf(Word instr, int pid, union Register** r, ObjFile *curExec);

int subf(Word instr, int pid, union Register** r, ObjFile *curExec);

int divf(Word instr, int pid, union Register** r, ObjFile *curExec);

int mulf(Word instr, int pid, union Register** r, ObjFile *curExec);

int addi(Word instr, int pid, union Register** r, ObjFile *curExec);

int subi(Word instr, int pid, union Register** r, ObjFile *curExec);

int divi(Word instr, int pid, union Register** r, ObjFile *curExec);

int muli(Word instr, int pid, union Register** r, ObjFile *curExec);

int call(Word instr, int pid, union Register** r, ObjFile *curExec);

int ret(Word instr, int pid, union Register** r, ObjFile *curExec);

int blt(Word instr, int pid, union Register** r, ObjFile *curExec);

int bgt(Word instr, int pid, union Register** r, ObjFile *curExec);

int beq(Word instr, int pid, union Register** r, ObjFile *curExec);

int jmp(Word instr, int pid, union Register** r, ObjFile *curExec);

int cmpxchg(Word instr, int pid, union Register** r, ObjFile *curExec);

int getpid(Word instr, int pid, union Register** r, ObjFile *curExec);

int getpn(Word instr, int pid, union Register** r, ObjFile *curExec);

int push(Word instr, int pid, union Register** r, ObjFile *curExec);

int pop(Word instr, int pid, union Register** r, ObjFile *curExec);

#endif