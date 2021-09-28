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
    int addr =  twosComplementer(instr >> 12, 20);
    if (opcode == 15) { //call
        for(int i = 0; i < numOutSymbols; i++ ){
            if (addr == 0 && pc-1 == outsymbolAddrs[i]){
                printf("%07d   %08x %8s %s\n", pc-1, instr, opcodeNames[opcode], "[UNDEF]");
                return;
            }
        }
    }
    printf("%07d   %08x %8s %d\n", pc-1, instr, opcodeNames[opcode], pc+addr);
}

void opReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    printf("%07d   %08x %8s r%d\n", pc-1, instr, opcodeNames[opcode], reg1);
}

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

void opRegReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    printf("%07d   %08x %8s r%d, r%d\n", pc-1, instr, opcodeNames[opcode], reg1, reg2);
}

void opRegOffsetReg(Word instr, int pc) {
    int opcode = 0x000000FF & instr;
    int reg1 = 0x0000000F & (instr >> 8);
    int reg2 = 0x0000000F & (instr >> 12);
    int offset = twosComplementer(instr >> 16, 16);
    printf("%07d   %08x %8s r%d, %d(r%d)\n", pc-1, instr, opcodeNames[opcode], reg1, offset, reg2);
}

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
    char byte1 = blob[addr];
    w = w | byte1;
    char byte2 = blob[addr+1];
    w = w | (byte2 << 8);
    char byte3 = blob[addr+2];
    w = w | (byte3 << 16);
    char byte4 = blob[addr+3];
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
        for(int b = 0; b < 16; b++) {
            //16 chars (4 words) permitted for symbol names
            char c = blob[secStartAddr+(20*i)+b];
            //printf("%x ", c);
            printf("%c", c);
        }
        Word symAddr = readWord(blob, secStartAddr+(20*i)+16);
        if(inOrOut == 0) {
            insymbolAddrs[i] = symAddr;
        } else {
            outsymbolAddrs[i] = symAddr;
        }
        printf(" %d\n", symAddr);//don't need to 2s comp this cause they're always positive?
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

int main(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stderr, "Invalid Arguments");
    }

    char* objFile = loadFile(argv[1]);

    Word inSymLen = readWord(objFile, 0);
    Word outSymLen = readWord(objFile, 4);
    Word objCodeLen = readWord(objFile, 8);

    numInSymbols = inSymLen;
    numOutSymbols = outSymLen;

    insymbolAddrs = (int *) malloc(sizeof(int)*inSymLen);
    outsymbolAddrs = (int *) malloc(sizeof(int)*outSymLen);

    //dumpEveryByte(objFile, 12+(inSymLen*4)+(outSymLen*4)+(objCodeLen*4));

    printInSymbolSection(objFile, inSymLen, 12);
    printOutSymbolSection(objFile, outSymLen, 12+(inSymLen*4));

    printf("\nObject Code (%d words): \n\n", objCodeLen);
    //start of obj code is at 12 bytses (to account for the 3 section-length Words that start the file) 
    //   + lengths of in- and out-symbol sections
    Word *objCode = getObjCodeAsWordArray(objFile, 12 + (inSymLen*4) + (outSymLen*4), objCodeLen );

    for(int i = 0; i < objCodeLen; i++) {
        int opcode = 0x000000FF & objCode[i];
        if(opcode < 26) {
            (*instrFormatHandlers[opcode])(objCode[i], i+1); //+1 because the program counter will have incremented
        } else {
            //the opcode is invalid so this must be data
            printf("%07d   %08x UNKNOWN", i, objCode[i]);
        }
    }

    return 0;
}