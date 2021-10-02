#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataTypes.h"

/*
a simple disassembler for the vmx20 toy vm assembler
*/

int *insymbolAddrs;
int *outsymbolAddrs;

int numInSymbols;
int numOutSymbols;

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

char* regNames[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "fp", "sp", "pc"};

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

int checkInsymbols(int addr, int pc) {
    for(int i = 0; i < numOutSymbols; i++ ){
        if (addr == 0 && pc-1 == outsymbolAddrs[i]){
            return 1;
        }
    }
    return 0;
}

void op(Word instr, int pc) {
    //this op takes no params
    int opcode = 0x000000FF & instr; 
    printf("%07d   %08x %8s\n", pc-1, instr, opcodeNames[opcode]);
}

void opAddr(Word instr, int pc) {
    int opcode = 0x000000FF & instr; 
    int addr =  twosComplementer(instr >> 12, 20);
    if (checkInsymbols(addr, pc) == 1) {
        printf("%07d   %08x %8s %s\n", pc-1, instr, opcodeNames[opcode], "[UNDEF]");
    } else {
        printf("%07d   %08x %8s %d\n", pc-1, instr, opcodeNames[opcode], pc+addr);
    }
}

void opReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    printf("%07d   %08x %8s %s\n", pc-1, instr, opcodeNames[opcode], regNames[reg1]);
}

void opRegConst(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int constant = twosComplementer(instr >> 12, 20);
    printf("%07d   %08x %8s %s, %d\n", pc-1, instr, opcodeNames[opcode], regNames[reg1], constant);
}

void opRegAddr(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int addr =  twosComplementer(instr >> 12, 20);
    if (checkInsymbols(addr, pc) == 1) {
        printf("%07d   %08x %8s %s, %s\n", pc-1, instr, opcodeNames[opcode], regNames[reg1], "[UNDEF]");
    } else {
        printf("%07d   %08x %8s %s, %d\n", pc-1, instr, opcodeNames[opcode], regNames[reg1], pc+addr);
    }
}

void opRegReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    printf("%07d   %08x %8s %s, %s\n", pc-1, instr, opcodeNames[opcode], regNames[reg1], regNames[reg2]);
}

void opRegOffsetReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int offset = twosComplementer(instr >> 16, 16);
    printf("%07d   %08x %8s %s, %d(%s)\n", pc-1, instr, opcodeNames[opcode], regNames[reg1], offset, regNames[reg2]);
}

void opRegRegAddr(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int addr = twosComplementer(instr >> 16, 16);
    if(checkInsymbols(addr,pc) == 1){
        printf("%07d   %08x %8s %s, %s, %s\n", pc-1, instr, opcodeNames[opcode], regNames[reg1], regNames[reg2], "[UNDEF]");
    } else {
        printf("%07d   %08x %8s %s, %s, %d\n", pc-1, instr, opcodeNames[opcode], regNames[reg1], regNames[reg2], pc+addr);
    }
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

// load the file into a blob in memory
char* loadFile(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (f) {
        printf("File: %s\n", filename);
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        char *blob = malloc(size);
        fseek(f, 0, SEEK_SET);
        fread(blob, size, 1, f);
        fclose(f);

        return blob;
    } else {
        fprintf(stderr, "File '%s' could not be opened.\n", filename); 
        exit(-1);
    }
}

//returns the next word  starting from the given address.
Word readWord(char* blob, int addr) {

    Word w = 0;
    unsigned char byte1 = blob[addr];
    w = w | byte1;
    unsigned char byte2 = blob[addr+1];
    w = w | (byte2 << 8);
    unsigned char byte3 = blob[addr+2];
    w = w | (byte3 << 16);
    unsigned char byte4 = blob[addr+3];
    w = w | (byte4 << 24);
    return w;

}

Word* getObjCodeAsWordArray(char* blob, Word codeStartAddr, Word numCodeWords) {
    Word *objCode = malloc(sizeof(Word)*numCodeWords);
    
    for(int i = 0; i < numCodeWords; i++) {
        objCode[i] = readWord(blob, codeStartAddr + (4*i));
    }

    return objCode;
}

void dumpEveryByte(char* blob, int numBytes) {
    for (int i = 0; i < numBytes; i +=4){
        printf("%x %x %x %x\n", blob[i], blob[i+1], blob[i+2], blob[i+3]);
    }
}

void printSymbolSection(char* blob, Word symSectionLen, Word secStartAddr, int inOrOut) {
    int numSymbols = symSectionLen/5;
    for(int i = 0; i < numSymbols; i++){
        char symName[] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
        for(int b = 0; b < 16; b++) {
            //16 chars (4 words) permitted for symbol names
            char c = blob[secStartAddr+(20*i)+b];
            //printf("%x ", c);
            symName[b] = c;
            //printf("%c", c);
        }
        Word symAddr = readWord(blob, secStartAddr+(20*i)+16);
        if(inOrOut == 0) {
            insymbolAddrs[i] = symAddr;
        } else {
            outsymbolAddrs[i] = symAddr;
        }
        printf("%s %d\n", symName, symAddr);//don't need to 2s comp this cause they're always positive?
    }
}

void printInSymbolSection(char* blob, Word symSectionLen, Word secStartAddr) {
    printf("\nInsymbol section (%d entries): \n\n", symSectionLen/5);
    printSymbolSection(blob, symSectionLen, secStartAddr, 0);
}

void printOutSymbolSection(char* blob, Word symSectionLen, Word secStartAddr) {
    printf("\nOutsymbol section (%d entries): \n\n", symSectionLen/5);
    printSymbolSection(blob, symSectionLen, secStartAddr, 1);
}

void recursiveDisassembler(Word *objCode, int *dataOrCode, int pc, int objCodeLen) {
    while (pc < objCodeLen) {
        Word curInstr = objCode[pc];
        dataOrCode[pc] = 1;
        pc++;
        int opcode = 0x000000FF & curInstr;
        if (opcode == 20) {
            //jmp
            int addr = twosComplementer(curInstr >> 12, 20);
            if (addr > 0) {
                //an unconditional forward jump
                pc = pc+addr;
            }
        } else if (opcode == 15) {
            //call
            int addr = twosComplementer(curInstr >> 12, 20);
            if (checkInsymbols(addr, pc) == 0) {
                // a call to a function in this file
                recursiveDisassembler(objCode, dataOrCode, pc+addr, objCodeLen);
            }
        } else if (opcode == 0 || opcode == 16) {
            //halt or ret means we're done.
            return;
        }

    }
}

int main(int argc, char** argv) {

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Invalid Arguments");
    }

    char* objFile;

    //use the right arg as the filename if the recursive option is there
    if (strcmp(argv[1], "-r") == 0) {
        objFile = loadFile(argv[2]);
    } else {
        objFile = loadFile(argv[1]);
    }

    Word inSymLen = readWord(objFile, 0);
    Word outSymLen = readWord(objFile, 4);
    Word objCodeLen = readWord(objFile, 8);

    numInSymbols = inSymLen/5;
    numOutSymbols = outSymLen/5;
   
    insymbolAddrs = (int *) malloc(sizeof(int)*(inSymLen/5));
    outsymbolAddrs = (int *) malloc(sizeof(int)*(outSymLen/5));

    //dumpEveryByte(objFile, 12+(inSymLen*4)+(outSymLen*4)+(objCodeLen*4));

    //dumpEveryByte(objFile, 12);

    printInSymbolSection(objFile, inSymLen, 12);
    printOutSymbolSection(objFile, outSymLen, 12+(inSymLen*4));

    printf("\nObject Code (%d words): \n\n", objCodeLen);
    //start of obj code is at 12 bytses (to account for the 3 section-length Words that start the file) 
    //   + lengths of in- and out-symbol sections
    Word *objCode = getObjCodeAsWordArray(objFile, 12 + (inSymLen*4) + (outSymLen*4), objCodeLen );

    //linear disassembly
    if (strcmp(argv[1], "-r") != 0) {
        for(int i = 0; i < objCodeLen; i++) {
            int opcode = 0x000000FF & objCode[i];
            if(opcode < 26) {
                (*instrFormatHandlers[opcode])(objCode[i], i+1); //+1 because the program counter will have incremented
            } else {
                //the opcode is invalid so this must be data
                printf("%07d   %08x UNKNOWN\n", i, objCode[i]);
            }
        }
    } else {
        int *dataOrCode = (int *) calloc(objCodeLen, sizeof(int));
        // first pass marks each word as data or code
        recursiveDisassembler(objCode, dataOrCode, 0, objCodeLen);
        //2nd pass prints out each disassembled word
        for(int i = 0; i < objCodeLen; i++) {
            int opcode = 0x000000FF & objCode[i];
            if (dataOrCode[i] == 1) {
                (*instrFormatHandlers[opcode])(objCode[i], i+1); //+1 because the program counter will have incremented
            } else {
                //the opcode is invalid
                printf("%07d   %08x [DATA]\n", i, objCode[i]);
            }
        }

    }

    return 0;
}