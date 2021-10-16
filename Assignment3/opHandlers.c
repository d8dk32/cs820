#include "opHandlers.h"

#define CONTINUE_PROCESSING 1
//VMX20 error code analogues
#define NORMAL_TERMINATION 0
#define DIVIDE_BY_ZERO -1
#define ADDRESS_OUT_OF_RANGE -2
#define ILLEGAL_INSTRUCTION -3

int halt(Word instr, int pid, union Register** r, ObjFile *curExec){
    return NORMAL_TERMINATION;
}

int load(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int store(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int ldimm(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int ldaddr(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int ldind(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int stind(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int addf(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int subf(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int divf(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int mulf(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int addi(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int subi(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int divi(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int muli(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int call(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int ret(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int blt(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int bgt(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int beq(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int jmp(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int cmpxchg(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int getpid(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int getpn(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int push(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}

int pop(Word instr, int pid, union Register** r, ObjFile *curExec){
    return CONTINUE_PROCESSING;
}