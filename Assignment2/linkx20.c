#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataTypes.h"

//it would have been better to construct some header files such that these utility functions were more reuseable,
// but at this point it would take longer to refactor than to copy and paste, so here we are...

int mainx20Exists = 0;

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

void readSymbolSection(char* blob, Word numSymbols, Word secStartAddr, Symbol* symList, int offset, int isInSymbolSection) {
    for(int i = 0; i < numSymbols; i++){
        char* symName = calloc(16, sizeof(char));
        for(int b = 0; b < 16; b++) {
            //16 chars (4 words) permitted for symbol names
            char c = blob[secStartAddr+(20*i)+b];
            symName[b] = c;
        }
        Word symAddr = readWord(blob, secStartAddr+(20*i)+16);
        symList[i].addr = symAddr;
        if (isInSymbolSection == 1) {
            symList[i].addr += offset;
        }
        symList[i].name = symName;
        if (strcmp(symName, "mainx20") == 0 && isInSymbolSection == 1) {
            mainx20Exists = 1;
        }
    }
}

//TODO: the real version of this function, with reverse 2s complementing and shit
Word updateCodeWordAddr(Word codeWord, int pcRelativeAddr){

    int mask = 0x00000FFF;
    int shift = 12;
    int opcode = 0x000000FF & codeWord; 

    //these are all the opcodes that could use a smaller-sized addr field
    if (opcode == 5 || opcode == 6 || opcode == 17 || opcode == 18 || opcode == 19 || opcode == 21) {
        //blt, bgt, beq opcodes
        mask = 0x0000FFFF;
        shift = 16;
    }

    //don't need to to mess around with 2's complement or anything
    // just clean out the address chunk (i think it should be already?) and paste in the new address
    codeWord = (mask & codeWord) | (pcRelativeAddr << shift);

    return codeWord;
}

void writeWord(Word outWord, FILE* outFile){
    unsigned char byte1 = 0x000000FF & outWord;
    unsigned char byte2 = (0x0000FF00 & outWord) >> 8;
    unsigned char byte3 = (0x00FF0000 & outWord) >> 16;
    unsigned char byte4 = (0xFF000000 & outWord) >> 24;

    unsigned char wordArr[4] = {byte1, byte2, byte3, byte4};
    fwrite(wordArr, sizeof(unsigned char), sizeof(wordArr), outFile);
}

void writeSymbol(Symbol sym, FILE* outFile){
    //write the symbol name
    for(int i = 0; i < 16; i++){
        fwrite(&sym.name[i], sizeof(char), sizeof(char), outFile);
    }
    writeWord(sym.addr, outFile);
}

int main(int argc, char** argv) {

    if (argc == 1) {
        fprintf(stderr, "Incorrect number of arguments.\n");
        exit(1);
    }

    int dashOFlag = strcmp(argv[argc-2], "-o");

    char* exeFileName = dashOFlag == 0 ? strcat(argv[argc-1], ".exe") : "vmx20.exe";

    int numInputFiles = dashOFlag == 0 ? argc-3 : argc-1;

    //printf("num. input files: %d\n", numInputFiles);

    if (numInputFiles == 0) {
        fprintf(stderr, "At least 1 object file must be specified.\n");
        exit(1);
    }

    //load each obj file
    ObjFile objFiles[numInputFiles];
    int runningTotalOffset = 0;
    for(int i = 0; i < numInputFiles; i++) {
        char* objFile = loadFile(argv[i+1]);
        objFiles[i].offset = runningTotalOffset;
        objFiles[i].numInSymbols = readWord(objFile, 0)/5;
        objFiles[i].numOutSymbols = readWord(objFile, 4)/5;
        objFiles[i].objCodeLen = readWord(objFile, 8);
        runningTotalOffset += objFiles[i].objCodeLen;
        objFiles[i].inSymbols = malloc(sizeof(Symbol)*objFiles[i].numInSymbols);
        objFiles[i].outSymbols = malloc(sizeof(Symbol)*objFiles[i].numOutSymbols);
        objFiles[i].objCode = getObjCodeAsWordArray(objFile, 12 + (objFiles[i].numInSymbols*20) + (objFiles[i].numOutSymbols*20), objFiles[i].objCodeLen);

        if(objFiles[i].numInSymbols > 0){
            readSymbolSection(objFile, objFiles[i].numInSymbols, 12, objFiles[i].inSymbols, objFiles[i].offset, 1);
        }
        if(objFiles[i].numOutSymbols > 0){
            readSymbolSection(objFile, objFiles[i].numOutSymbols, 12+(objFiles[i].numInSymbols*20), objFiles[i].outSymbols, objFiles[i].offset, 0);
        }
        
        //uncomment to dump file info------------------------------------------------------
        // printf("%s -----------\n", argv[i+1]);
        // printf("offset: %d, numInSym: %d, numOutSym: %d, objCodeLen: %d \n", objFiles[i].offset, objFiles[i].numInSymbols, objFiles[i].numOutSymbols, objFiles[i].objCodeLen);
        // printf("insymbols: \n");
        // for(int j = 0; j < objFiles[i].numInSymbols; j++) {
        //     printf("%s: %d\n", objFiles[i].inSymbols[j].name, objFiles[i].inSymbols[j].addr);
        // }
        // printf("outsymbols:\n");
        // for(int j = 0; j < objFiles[i].numOutSymbols; j++) {
        //     printf("%s: %d\n", objFiles[i].outSymbols[j].name, objFiles[i].outSymbols[j].addr);
        // }
    }

    if (mainx20Exists == 0) {
        fprintf(stderr, "Required entry point label 'mainx20' not found.\n");
        exit(1);
    }

    //check for duplicate in-symbols. this code will be kind of horrible
    for(int i = 0; i < numInputFiles; i++){
        for(int j = 0; j < objFiles[i].numInSymbols; j++) {
            for(int ii = 0; ii < numInputFiles; ii++){
                for(int jj = 0; jj < objFiles[ii].numInSymbols; jj++){
                    int strCmp = strcmp(objFiles[i].inSymbols[j].name, objFiles[ii].inSymbols[jj].name);
                    if( strCmp == 0 && i != ii && j != jj) {
                        fprintf(stderr, "Duplicate insymbols detected; unable to complete linking.\n");
                        exit(1);
                    }
                }
            }
        }
    }

    //resolve outsymbols
    for (int i = 0; i < numInputFiles; i++){ //iterate over input files
        for(; objFiles[i].numOutSymbols > 0; objFiles[i].numOutSymbols--){ //iterate over all the outsymbols, 'removing' them once resolved 
            int outsymResolved = 0;
            for(int j = 0; j < numInputFiles; j++){
                for(int k = 0; k < objFiles[j].numInSymbols; k++){
                    int curOutsymIdx = objFiles[i].numOutSymbols-1;
                    int inSymMatch = strcmp(objFiles[i].outSymbols[curOutsymIdx].name, objFiles[j].inSymbols[k].name);
                    if (inSymMatch == 0) {
                        //a matching insymbol was found, this outsymbol has been resolved
                        outsymResolved = 1;
                        //update address portion of the code word at the outsymbol's location
                        int insymAddr = objFiles[j].inSymbols[k].addr;
                        int curOutsymAddr = objFiles[i].outSymbols[curOutsymIdx].addr; //really the pc at the outsymbol's reference in this file
                        Word updatedCodeWord = updateCodeWordAddr(objFiles[i].objCode[curOutsymAddr], insymAddr - (objFiles[i].offset+curOutsymAddr+1));
                        objFiles[i].objCode[curOutsymAddr] = updatedCodeWord;
                        //and then bail
                        break;
                    }
                }
                if(outsymResolved == 1){
                        break;
                }
            }
            if (outsymResolved == 0){
                //outsymbol could not be resolved. so error out
                fprintf(stderr, "Could not resolve outsymbol '%s'. Linking Failed.\n", objFiles[i].outSymbols[objFiles[i].numOutSymbols-1].name);
                exit(1);
            }
        }
    }

    //uncomment to dump each file's code, for checking correctness of modified addresses
    // for(int i = 0; i < numInputFiles; i++){
    //     printf("%s -----------\n", argv[i+1]);
    //     for(int j = 0; j < objFiles[i].objCodeLen; j++){
    //         printf("0x%8x\n", objFiles[i].objCode[j]);
    //     }
    // }

    //now assemble the whole deal into an output executable file
    //open the file
    FILE *outFile = fopen(exeFileName, "wb+"); // write only
    if (outFile == NULL) {
        printf("Failed to open output file '%s'.\n", exeFileName);
        exit(1);
    }
    //output header
    Word sumInSyms = 0;
    Word sumObjCodeLen = 0;
    for(int i = 0; i < numInputFiles; i++){
        sumInSyms += objFiles[i].numInSymbols;
        sumObjCodeLen += objFiles[i].objCodeLen;
    }
    writeWord(sumInSyms*5, outFile);
    writeWord(0, outFile);
    writeWord(sumObjCodeLen, outFile);
    
    //output in-symbols. No need for out-symbols, cause if we've made it this far there aren't any more.
    for(int i = 0; i < numInputFiles; i++){
        for(int j = 0; j < objFiles[i].numInSymbols; j++){
            writeSymbol(objFiles[i].inSymbols[j], outFile);
        }
    }
    
    //output code
    for(int i = 0; i < numInputFiles; i++){
        for(int j = 0; j < objFiles[i].objCodeLen; j++){
            writeWord(objFiles[i].objCode[j], outFile);
        }
    }

    //close the file
    fclose(outFile);



}