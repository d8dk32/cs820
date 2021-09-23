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


int main(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stderr, "Invalid Arguments");
    }

    char* objFile = loadFile(argv[1]);

    

    return 0;
}