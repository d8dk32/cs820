#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataTypes.h"

// returns the next word  starting from the given address.
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

// reads a specified number of Words from a byte array, starting at a specified address.
// intended to be used to read all the code words from an obj file, but i could see some off-label uses.
Word* getObjCodeAsWordArray(char* blob, Word codeStartAddr, Word numCodeWords) {
    Word *objCode = malloc(sizeof(Word)*numCodeWords);
    
    for(int i = 0; i < numCodeWords; i++) {
        objCode[i] = readWord(blob, codeStartAddr + (4*i));
    }

    return objCode;
}

// parses a symbol section from an obj file (represented as a byte array, as if returned by loadFile())
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
        return NULL;
    }
}

// take a file loaded into memory as a byte array and parse it into an ObjFile struct
// for easier use
ObjFile* parseObjFile(char* objFile) {

    ObjFile objFileStruct;
    objFileStruct.numInSymbols = readWord(objFile, 0)/5;
    objFileStruct.numOutSymbols = readWord(objFile, 4)/5;
    objFileStruct.objCodeLen = readWord(objFile, 8);
    objFileStruct.inSymbols = malloc(sizeof(Symbol)*objFileStruct.numInSymbols);
    objFileStruct.outSymbols = malloc(sizeof(Symbol)*objFileStruct.numOutSymbols);
    objFileStruct.objCode = getObjCodeAsWordArray(objFile, 12 + (objFileStruct.numInSymbols*20) + (objFileStruct.numOutSymbols*20), objFileStruct.objCodeLen);

    if(objFileStruct.numInSymbols > 0){
        readSymbolSection(objFile, objFileStruct.numInSymbols, 12, objFileStruct.inSymbols);
    }
    if(objFileStruct.numOutSymbols > 0){
        readSymbolSection(objFile, objFileStruct.numOutSymbols, 12+(objFileStruct.numInSymbols*20), objFileStruct.outSymbols);
    }
    
    return *objFileStruct;
}