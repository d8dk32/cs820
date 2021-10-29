#include <stdio.h>
#include "opHandlers.h"
#include <pthread.h>

#define CONTINUE_PROCESSING 1
//VMX20 error code analogues
#define NORMAL_TERMINATION 0
#define DIVIDE_BY_ZERO -1
#define ADDRESS_OUT_OF_RANGE -2
#define ILLEGAL_INSTRUCTION -3

static int totalNumProcessors = 1; //1 by default

char* regNames[];
char* opcodeNames[];

pthread_mutex_t memMut;
pthread_mutex_t traceMut;

//----op handlers----

int halt(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: halt\n", pid);
    }
    return NORMAL_TERMINATION;
}

int load(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x load\n", instr);
    int reg1 = 0x0000000F & (instr >> 8);
    int addr =  twosComplementer(instr >> 12, 20);

    if( r[pid][PC].ui+addr > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    r[pid][reg1].i = curExec->objCode[ r[pid][PC].ui+addr ];

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: load r%d, %d\n", pid, reg1, addr);
    }

    return CONTINUE_PROCESSING;
}

int store(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x store\n", instr);
    int reg1 = 0x0000000F & (instr >> 8);
    int addr =  twosComplementer(instr >> 12, 20);

    if( r[pid][PC].ui+addr > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    curExec->objCode[ r[pid][PC].ui+addr ] = r[pid][reg1].i;
    
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: store r%d, %d\n", pid, reg1, addr);
    }

    return CONTINUE_PROCESSING;
}

int ldimm(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x ldimm\n", instr);
    int reg1 = 0x0000000F & (instr >> 8);
    int constant = twosComplementer(instr >> 12, 20);

    r[pid][reg1].i = constant;
    
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: ldimm r%d, %d\n", pid, reg1, constant);
    }    

    return CONTINUE_PROCESSING;
}

int ldaddr(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x ldaddr\n", instr);
    int reg1 = 0x0000000F & (instr >> 8);
    int addr =  twosComplementer(instr >> 12, 20);

    r[pid][reg1].i = r[pid][PC].ui+addr;
    
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: ldaddr r%d, %d\n", pid, reg1, addr);
    }

    return CONTINUE_PROCESSING;
}

int ldind(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x ldind\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int offset = twosComplementer(instr >> 16, 16);

    if( r[pid][reg2].i + offset > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    r[pid][reg1].i = curExec->objCode[ r[pid][reg2].i + offset ];
    
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: ldind r%d, %d(%d)\n", pid, reg1, offset, reg2);
    }

    return CONTINUE_PROCESSING;
}

int stind(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x stind\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int offset = twosComplementer(instr >> 16, 16);

    if( r[pid][reg2].i+offset > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    curExec->objCode[ r[pid][reg2].i+offset ] = r[pid][reg1].i;
    
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: stind r%d, %d(%d)\n", pid, reg1, offset, reg2);
    }

    return CONTINUE_PROCESSING;
}

int addf(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x addf\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    r[pid][reg1].f = r[pid][reg1].f + r[pid][reg2].f;
    
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: addf r%d, r%d\n", pid, reg1, reg2);
    }

    return CONTINUE_PROCESSING;
}

int subf(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x subf\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    r[pid][reg1].f = r[pid][reg1].f - r[pid][reg2].f;
    
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: subf r%d, r%d\n", pid, reg1, reg2);
    }

    return CONTINUE_PROCESSING;
}

int divf(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x divf\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    if(r[pid][reg2].f == 0){
        return DIVIDE_BY_ZERO;
    }

    r[pid][reg1].f = r[pid][reg1].f / r[pid][reg2].f;

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: divf r%d, r%d\n", pid, reg1, reg2);
    }

    return CONTINUE_PROCESSING;
}

int mulf(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x mulf\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    r[pid][reg1].f = r[pid][reg1].f * r[pid][reg2].f;
    
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: mulf r%d, r%d\n", pid, reg1, reg2);
    }

    return CONTINUE_PROCESSING;
}

int addi(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x addi\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    r[pid][reg1].i = r[pid][reg1].i + r[pid][reg2].i;
   
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: addi r%d, r%d\n", pid, reg1, reg2);
    }

    return CONTINUE_PROCESSING;
}

int subi(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x subi\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    r[pid][reg1].i = r[pid][reg1].i - r[pid][reg2].i;
    
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: subi r%d, r%d\n", pid, reg1, reg2);
    }

    return CONTINUE_PROCESSING;
}

int divi(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x divi\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    if(r[pid][reg2].i == 0){
        return DIVIDE_BY_ZERO;
    }

    r[pid][reg1].i = r[pid][reg1].i / r[pid][reg2].i;
    
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: divi r%d, r%d\n", pid, reg1, reg2);
    }

    return CONTINUE_PROCESSING;
}

int muli(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x muli\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);

    r[pid][reg1].i = r[pid][reg1].i * r[pid][reg2].i;
    
    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: muli r%d, r%d\n", pid, reg1, reg2);
    };

    return CONTINUE_PROCESSING;
}

int call(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
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

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: call %d\n", pid, addr);
    }

    return CONTINUE_PROCESSING;
}

int ret(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){

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

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: ret\n", pid);
    }

    return CONTINUE_PROCESSING;
}

int blt(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
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

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: blt r%d, r%d, %d\n", pid, reg1, reg2, addr);
    }

    return CONTINUE_PROCESSING;
}

int bgt(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
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

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: bgt r%d, r%d, %d\n", pid, reg1, reg2, addr);
    }

    return CONTINUE_PROCESSING;
}

int beq(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
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

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: beq r%d, r%d, %d\n", pid, reg1, reg2, addr);
    }

    return CONTINUE_PROCESSING;
}

int jmp(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x jmp\n", instr);
    int addr =  twosComplementer(instr >> 12, 20);

    r[pid][PC].ui += addr;

    if(r[pid][PC].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: jmp %d\n", pid, addr);
    }

    return CONTINUE_PROCESSING;
}

int cmpxchg(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
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

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: cmpxchg r%d, r%d, %d\n", pid, reg1, reg2, addr);
    }

    return CONTINUE_PROCESSING;
}

int getpid(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x getpid\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);

    r[pid][reg1].i = pid;

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: getpid r%d\n", pid, reg1);
    }

    return CONTINUE_PROCESSING;
}

int getpn(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x getpn\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);

    r[pid][reg1].i = totalNumProcessors;

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: getpn r%d\n", pid, reg1);
    }

    return CONTINUE_PROCESSING;
}

int push(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
    //printf("%08x push\n", instr);

    int reg1 = 0x0000000F & (instr >> 8);

    if(r[pid][SP].ui > MAX_ADDR){
        return ADDRESS_OUT_OF_RANGE;
    }

    r[pid][SP].ui -= 1;
    curExec->objCode[r[pid][SP].ui] = r[pid][reg1].i;

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: push r%d\n", pid, reg1);
    }

    return CONTINUE_PROCESSING;
}

int pop(Word instr, int pid, union Register** r, ObjFile *curExec, int trace){
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

    if (trace == 1){
        traceRegisters(pid, r);
        printf("<%d>: pop r%d\n", pid, reg1);
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

void traceRegisters(int pid, union Register** r){
    printf("<%d>: %08x %08x %08x %08x %08x %08x %08x %08x\n", pid, r[pid][0].ui, r[pid][1].ui, r[pid][2].ui, r[pid][3].ui, 
                                                                r[pid][4].ui, r[pid][5].ui, r[pid][6].ui, r[pid][7].ui);
    printf("<%d>: %08x %08x %08x %08x %08x %08x %08x %08x\n", pid, r[pid][8].ui, r[pid][9].ui, r[pid][10].ui, r[pid][11].ui, 
                                                                r[pid][12].ui, r[pid][13].ui, r[pid][14].ui, r[pid][15].ui);
}

//-----------------------------------------------------------------

void op(Word instr, int pc, char* buffer) {
    //this op takes no params
    int opcode = 0x000000FF & instr; 
    sprintf(buffer, "%s", opcodeNames[opcode]);
}

void opAddr(Word instr, int pc, char* buffer) {
    int opcode = 0x000000FF & instr; 
    int addr =  twosComplementer(instr >> 12, 20);
    
    sprintf(buffer, "%s %d", opcodeNames[opcode], pc+addr);
}

void opReg(Word instr, int pc, char* buffer) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    sprintf(buffer, "%s %s", opcodeNames[opcode], regNames[reg1]);
}

void opRegConst(Word instr, int pc, char* buffer) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int constant = twosComplementer(instr >> 12, 20);
    sprintf(buffer, "%s %s, %d", opcodeNames[opcode], regNames[reg1], constant);
}

void opRegAddr(Word instr, int pc, char* buffer) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int addr =  twosComplementer(instr >> 12, 20);
    
    sprintf(buffer, "%s %s, %d", opcodeNames[opcode], regNames[reg1], pc+addr);
}

void opRegReg(Word instr, int pc, char* buffer) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    sprintf(buffer, "%s %s, %s", opcodeNames[opcode], regNames[reg1], regNames[reg2]);
}

void opRegOffsetReg(Word instr, int pc, char* buffer) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int offset = twosComplementer(instr >> 16, 16);
    sprintf(buffer, "%s %s, %d, %s", opcodeNames[opcode], regNames[reg1], offset, regNames[reg2]);
}

void opRegRegAddr(Word instr, int pc, char* buffer) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int addr = twosComplementer(instr >> 16, 16);
    
    sprintf(buffer, "%s %s, %s, %d", opcodeNames[opcode], regNames[reg1], regNames[reg2], pc+addr);
}

char* regNames[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "fp", "sp", "pc"};

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