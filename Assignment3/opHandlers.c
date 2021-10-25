#include <stdio.h>
#include "opHandlers.h"

#define CONTINUE_PROCESSING 1
//VMX20 error code analogues
#define NORMAL_TERMINATION 0
#define DIVIDE_BY_ZERO -1
#define ADDRESS_OUT_OF_RANGE -2
#define ILLEGAL_INSTRUCTION -3

static int totalNumProcessors = 1; //1 by default

//----op handlers----

int halt(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x halt\n", instr);
    return NORMAL_TERMINATION;
}

int load(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x load\n", instr);
    int reg1 = 0x0000000F & (instr >> 8);
    int addr =  twosComplementer(instr >> 12, 20);

    if( r[pid][PC].ui+addr > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    r[pid][reg1].i = curExec->objCode[ r[pid][PC].ui+addr ];
    //printf("    %d loaded into r%d\n", r[pid][reg1].i, reg1);

    return CONTINUE_PROCESSING;
}

int store(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x store\n", instr);
    int reg1 = 0x0000000F & (instr >> 8);
    int addr =  twosComplementer(instr >> 12, 20);

    if( r[pid][PC].ui+addr > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    curExec->objCode[ r[pid][PC].ui+addr ] = r[pid][reg1].i;
    //printf("    %d stored at %d\n", r[pid][reg1].i, r[pid][PC].ui+addr);

    return CONTINUE_PROCESSING;
}

int ldimm(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x ldimm\n", instr);
    int reg1 = 0x0000000F & (instr >> 8);
    int constant = twosComplementer(instr >> 12, 20);

    r[pid][reg1].i = constant;
    //printf("    %d loaded into r%d\n", constant, reg1);    

    return CONTINUE_PROCESSING;
}

int ldaddr(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x ldaddr\n", instr);
    int reg1 = 0x0000000F & (instr >> 8);
    int addr =  twosComplementer(instr >> 12, 20);

    r[pid][reg1].i = r[pid][PC].ui+addr;
    //printf("    addr %d loaded into r%d\n", r[pid][PC].ui+addr, reg1);

    return CONTINUE_PROCESSING;
}

int ldind(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x ldind\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int offset = twosComplementer(instr >> 16, 16);

    if( r[pid][reg2].i + offset > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    r[pid][reg1].i = curExec->objCode[ r[pid][reg2].i + offset ];
    //printf("    value from addr %d loaded into r%d\n", r[pid][reg2].i + offset, reg1);

    return CONTINUE_PROCESSING;
}

int stind(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x stind\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int offset = twosComplementer(instr >> 16, 16);

    if( r[pid][reg2].i+offset > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    curExec->objCode[ r[pid][reg2].i+offset ] = r[pid][reg1].i;
    //printf("    %d stored at %d\n", r[pid][reg1].i, r[pid][reg2].i+offset);

    return CONTINUE_PROCESSING;
}

int addf(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x addf\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    r[pid][reg1].f = r[pid][reg1].f + r[pid][reg2].f;
    //printf("    adding r%d to r%d\n", reg2, reg1);

    return CONTINUE_PROCESSING;
}

int subf(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x subf\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    r[pid][reg1].f = r[pid][reg1].f - r[pid][reg2].f;
    //printf("    subtracting r%d from r%d\n", reg2, reg1);

    return CONTINUE_PROCESSING;
}

int divf(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x divf\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    if(r[pid][reg2].f == 0){
        return DIVIDE_BY_ZERO;
    }

    r[pid][reg1].f = r[pid][reg1].f / r[pid][reg2].f;
    //printf("    dividing r%d by r%d\n", reg1, reg2);

    return CONTINUE_PROCESSING;
}

int mulf(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x mulf\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    r[pid][reg1].f = r[pid][reg1].f * r[pid][reg2].f;
    //printf("    multiplying r%d by r%d\n", reg1, reg2);

    return CONTINUE_PROCESSING;
}

int addi(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x addi\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    r[pid][reg1].i = r[pid][reg1].i + r[pid][reg2].i;
    //printf("    adding r%d to r%d\n", reg2, reg1);

    return CONTINUE_PROCESSING;
}

int subi(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x subi\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    r[pid][reg1].i = r[pid][reg1].i - r[pid][reg2].i;
    //printf("    subtracting r%d from r%d\n", reg2, reg1);

    return CONTINUE_PROCESSING;
}

int divi(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x divi\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    if(r[pid][reg2].i == 0){
        return DIVIDE_BY_ZERO;
    }

    r[pid][reg1].i = r[pid][reg1].i / r[pid][reg2].i;
    //printf("    dividing r%d by r%d\n", reg1, reg2);

    return CONTINUE_PROCESSING;
}

int muli(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x muli\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    r[pid][reg1].i = r[pid][reg1].i * r[pid][reg2].i;
    //printf("    multiplying r%d by r%d\n", reg1, reg2);

    return CONTINUE_PROCESSING;
}

int call(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x call\n", instr);

    int addr =  twosComplementer(instr >> 12, 20);

    if(r[pid][SP].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    r[pid][SP].ui -= 1;
    curExec->objCode[r[pid][SP].ui] = r[pid][PC].ui;
    r[pid][PC].ui += addr;
    r[pid][SP].ui -= 1;
    curExec->objCode[r[pid][SP].ui] = r[pid][FP].ui;    
    r[pid][FP].ui = r[pid][SP].ui;
    r[pid][SP].ui -= 1;
    curExec->objCode[r[pid][SP].ui] = 0;

    if(r[pid][SP].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    return CONTINUE_PROCESSING;
}

int ret(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x ret\n", instr);

    if(r[pid][SP].ui > MAX_ADDR || r[pid][FP].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    int returnVal = curExec->objCode[r[pid][SP].ui];
    r[pid][SP].ui += 1;
    int savedFP = curExec->objCode[r[pid][SP].ui];
    r[pid][SP].ui += 1;
    unsigned int returnAddr = curExec->objCode[r[pid][SP].ui];
    r[pid][SP].ui += 1;
    r[pid][FP].ui = savedFP;
    r[pid][PC].ui = returnAddr;
    curExec->objCode[r[pid][FP].ui-1] = returnVal;

    if(r[pid][SP].ui > MAX_ADDR || r[pid][FP].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    return CONTINUE_PROCESSING;
}

int blt(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x blt\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int addr = twosComplementer(instr >> 16, 16);

    if (r[pid][reg1].i < r[pid][reg2].i){
        r[pid][PC].ui += addr;
    }

    if(r[pid][PC].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    return CONTINUE_PROCESSING;
}

int bgt(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x bgt\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int addr = twosComplementer(instr >> 16, 16);

    if (r[pid][reg1].i > r[pid][reg2].i){
        r[pid][PC].ui += addr;
    }

    if(r[pid][PC].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    return CONTINUE_PROCESSING;
}

int beq(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x beq\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int addr = twosComplementer(instr >> 16, 16);

    if (r[pid][reg1].i == r[pid][reg2].i){
        r[pid][PC].ui += addr;
    }

    if(r[pid][PC].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    return CONTINUE_PROCESSING;
}

int jmp(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x jmp\n", instr);
    int addr =  twosComplementer(instr >> 12, 20);

    r[pid][PC].ui += addr;

    if(r[pid][PC].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    return CONTINUE_PROCESSING;
}

int cmpxchg(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x cmpxchg\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int addr = twosComplementer(instr >> 16, 16);

    if (r[pid][PC].ui+addr > MAX_ADDR) {
        return ADDRESS_OUT_OF_RANGE;
    }

    if(r[pid][reg1].i == curExec->objCode[r[pid][PC].ui+addr]){
        curExec->objCode[r[pid][PC].ui+addr] = r[pid][reg2].i;
    } else {
        r[pid][reg1].i = curExec->objCode[r[pid][PC].ui+addr];
    }

    return CONTINUE_PROCESSING;
}

int getpid(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x getpid\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);

    r[pid][reg1].i = pid;

    return CONTINUE_PROCESSING;
}

int getpn(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x getpn\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);

    r[pid][reg1].i = totalNumProcessors;

    return CONTINUE_PROCESSING;
}

int push(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x push\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);

    if(r[pid][SP].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    r[pid][SP].ui -= 1;
    curExec->objCode[r[pid][SP].ui] = r[pid][reg1].i;

    return CONTINUE_PROCESSING;
}

int pop(Word instr, int pid, union Register** r, ObjFile *curExec){
    //printf("%08x pop\n", instr);

    if(r[pid][SP].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    int reg1 = 0x0000000F & (instr >> 8);

    r[pid][reg1].i = curExec->objCode[r[pid][SP].ui];
    r[pid][SP].ui += 1;

    if(r[pid][SP].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    return CONTINUE_PROCESSING;
}

//----utilities----

int twosComplementer(int value, int numBits){
    if(value >> (numBits-1) == 0) {
        //this is a positive number
        return value;
    }

    //x + 2sComp(x) = 2^N where N is the number of bits in x
    // so 2^N - X = 2sComp(x)
    int twoToTheN = 1 << numBits;
    int twosComp = value-twoToTheN; 
    return twosComp;
}