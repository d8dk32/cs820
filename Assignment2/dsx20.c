#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcodeHandlers.h"

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

int main(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stderr, "Invalid Arguments");
    }

    char* objFile = loadFile(argv[1]);

    Word inSymLen = readWord(objFile, 0);
    Word outSymLen = readWord(objFile, 4);
    Word objCodeLen = readWord(objFile, 8);

    printf("insym len: %d\n", inSymLen);
    printf("outsym len: %d\n", outSymLen);
    printf("objcode len: %d\n", objCodeLen);

    //start of obj code is at 12 bytses (to account for the 3 section-length Words that start the file) 
    //   + lengths of in- and out-symbol sections
    Word *objCode = getObjCodeAsWordArray(objFile, 12 + (inSymLen*4) + (outSymLen*4), objCodeLen );

    for(int i = 0; i < objCodeLen; i++) {
        int opcodeArrIdx = 0x000F & objCode[i];
        printf("code word: %08x, opcode: %s\n", objCode[i], opcodeNames[opcodeArrIdx]);
    }

    return 0;
}