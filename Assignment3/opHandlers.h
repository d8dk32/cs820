#include "dataTypes.h"

//VMX20 error code analogues
#define NORMAL_TERMINATION 0
#define DIVIDE_BY_ZERO -1
#define ADDRESS_OUT_OF_RANGE -2
#define ILLEGAL_INSTRUCTION -3

int halt(Word instr, int pid, union Register** r, ObjFile *curExec){
    return NORMAL_TERMINATION;
}

int 