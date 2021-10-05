#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataTypes.h"

//it would have been better to construct some header files such that these utility functions were more reuseable,
// but at this point it would take longer to refactor than to copy and paste, so here we are...

// load the file into a blob in memory
char* loadFile(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (f) {
        //printf("File: %s\n", filename);
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

void readSymbolSection(char* blob, Word numSymbols, Word secStartAddr, Symbol* symList) {
    for(int i = 0; i < numSymbols; i++){
        char* symName = calloc(16, sizeof(char));
        for(int b = 0; b < 16; b++) {
            //16 chars (4 words) permitted for symbol names
            char c = blob[secStartAddr+(20*i)+b];
            symName[b] = c;
        }
        Word symAddr = readWord(blob, secStartAddr+(20*i)+16);
        symList[i].addr = symAddr;
        symList[i].name = symName;
    }
}

int main(int argc, char** argv) {

    int dashOFlag = strcmp(argv[argc-2], "-o");

    char* exeFileName = dashOFlag == 0 ? strcat(argv[argc-1], ".exe") : "vmx20.exe";

    int numInputFiles = dashOFlag == 0 ? argc-3 : argc-1;

    printf("num. input files: %d\n", numInputFiles);

    if (numInputFiles == 0) {
        fprintf(stderr, "At least 1 object file must be specified.\n");
    }

    //load each obj file
    ObjFile objFiles[numInputFiles];

    for(int i = 0; i < numInputFiles; i++) {
        char* objFile = loadFile(argv[i+1]);
        objFiles[i].numInSymbols = readWord(objFile, 0)/5;
        objFiles[i].numOutSymbols = readWord(objFile, 4)/5;
        objFiles[i].objCodeLen = readWord(objFile, 8);
        objFiles[i].inSymbols = malloc(sizeof(Symbol)*objFiles[i].numInSymbols);
        objFiles[i].outSymbols = malloc(sizeof(Symbol)*objFiles[i].numOutSymbols);
        objFiles[i].objCode = getObjCodeAsWordArray(objFile, 12 + (objFiles[i].numInSymbols*20) + (objFiles[i].numOutSymbols*20), objFiles[i].objCodeLen);
        readSymbolSection(objFile, objFiles[i].numInSymbols, 12, objFiles[i].inSymbols);
        readSymbolSection(objFile, objFiles[i].numOutSymbols, 12+(objFiles[i].numInSymbols*20), objFiles[i].outSymbols);
        printf("%s -----------\n", argv[i+1]);
        printf("numInSym: %d, numOutSym: %d, objCodeLen: %d \n", objFiles[i].numInSymbols, objFiles[i].numOutSymbols, objFiles[i].objCodeLen);
        printf("insymbols: \n");
        for(int j = 0; j < objFiles[i].numInSymbols; j++) {
            printf("%s: %d\n", objFiles[i].inSymbols[j].name, objFiles[i].inSymbols[j].addr);
        }
        printf("outsymbols:\n");
        for(int j = 0; j < objFiles[i].numOutSymbols; j++) {
            printf("%s: %d\n", objFiles[i].outSymbols[j].name, objFiles[i].outSymbols[j].addr);
        }
    }



}