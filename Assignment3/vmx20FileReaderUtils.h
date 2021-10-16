#ifndef VMX20_FILE_READER_UTILS
#define VMX20_FILE_READER_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataTypes.h"

// These 3 functions are used primarily by other functions in this little module.
// however they can certainly be used a la carte with an executable/obj file 
// you've loaded into memory (see loadFile()).
Word readWord(char* blob, int addr);

Word* getObjCodeAsWordArray(char* blob, Word codeStartAddr, Word numCodeWords);

void readSymbolSection(char* blob, Word numSymbols, Word secStartAddr, Symbol* symList);

// These 2 functions will be the ones primarily called by the user. With these 2, a file can be loaded into
// memory, and then parsed into a useful data structure
char* loadFile(const char *filename);

ObjFile* parseObjFile(char* objFile);

#endif /*VMX20_FILE_READER_UTILS*/