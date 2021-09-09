#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/usr/include/elf.h"

// file containing lookup tables and functions for
// retrieving type codes and such, converting them 
// to meaning ful strings, etc.
//also iumports some .h files here that are used throughout.

char* fileClass[3] = {
    "Invalid", 
    "32-Bit", 
    "64-Bit"
};

char* dataEncoding[3] = {
    "Invalid", 
    "2's Comp. Little Endian", 
    "2's Comp. Big Endian"
};

//There are more legal values besides these, hopefully that doesn't bite me.
//beyond this is OS/Processor specific defined types
static const char* objFileType(int idx) {
    char* objFileTypeArr[5] = {
        "No Type",
        "Relocatable",
        "Executable",
        "Shared obj. file",
        "Core file"
    };
    if (idx < 5) return objFileTypeArr[idx];
    return "OS_or_Processor_Specific";
}

static const char* sectionType(int idx){
    //sh_type value (section type)
    char* sectionTypeArr[20] = {
        "NULL",
        "PROGBITS",
        "SYMTAB",
        "STRTAB",
        "RELA",
        "HASH",
        "DYNAMIC",
        "NOTE",
        "NOBITS",
        "REL",
        "SHLIB",
        "DYNSYM",
        "UNDEFINED",
        "UNDEFINED",
        "INIT_ARRAY",
        "FINI_ARRAY",
        "PREINIT_ARRAY",
        "GROUP",
        "SYMTAB_SHNDX",
        "NUM" //this one doesn't seem to really be a 'type', including it anyway.
    };
    if (idx < 20) return sectionTypeArr[idx];
    return "OS_or_Processor_Specific";

}
// allowed section header flags. Each bit turns on a certain flag.
static const char* printFlags64(Elf64_Xword code) {
    if (code & (1<<0)) printf("WRITE ");
    if (code & (1<<1)) printf("ALLOC ");
    if (code & (1<<2)) printf("EXEC ");
    if (code & (1<<4)) printf("MERGE ");
    if (code & (1<<5)) printf("STRINGS ");
    if (code & (1<<6)) printf("INFO-LINK ");
    if (code & (1<<7)) printf("LINK-ORDER ");
    if (code & (1<<8)) printf("OS-NONCONFORMING ");
    if (code & (1<<9)) printf("GROUP ");
    if (code & (1<<10)) printf("THREAD-LOCAL ");
    if (code & (1<<11)) printf("COMPRESSED ");
    //other values are undefined or os-specific, probably won't see them
}

static const char* printFlags32(Elf32_Word code) {
    if (code & (1<<0)) printf("WRITE ");
    if (code & (1<<1)) printf("ALLOC ");
    if (code & (1<<2)) printf("EXEC ");
    if (code & (1<<4)) printf("MERGE ");
    if (code & (1<<5)) printf("STRINGS ");
    if (code & (1<<6)) printf("INFO ");
    if (code & (1<<7)) printf("LINK-ORDER ");
    if (code & (1<<8)) printf("OS-NONCONFORMING ");
    if (code & (1<<9)) printf("GROUP ");
    if (code & (1<<10)) printf("THREAD-LOCAL ");
    if (code & (1<<11)) printf("COMPRESSED ");
    //other values are undefined or os-specific, probably won't see them
}

static const char* printStType(unsigned char val){
    val = ELF32_ST_TYPE(val);
    char* stTypeArr[8] = {
        "NOTYPE",
        "OBJECT",
        "FUNC",
        "SECTION",
        "FILE",
        "COMMON",
        "TLS",
        "NUM"
    };
    if (val > 7) return "OS/PROC";
    return stTypeArr[val];
}

static const char* printStBind(unsigned char val){
    val = ELF32_ST_BIND(val);
    char* stBindArr[4] = {
        "LOCAL",
        "GLOBAL",
        "WEAK",
        "NUM"
    };
    if (val > 3) return "OS/PROC";
    return stBindArr[val];
}

static const char* printStVis(unsigned char val) {
    val = ELF32_ST_VISIBILITY(val);
    char* visArr[4] = {
        "DEFAULT",
        "INTERNAL",
        "HIDDEN",
        "PROTECTED"
    };
    return visArr[val];
}

typedef struct {
    char* typeStr;
    int valOrPtr;
} TagType;

//this array relates legal (integer) values for d_tag (dynamic entry type) to a 
//string describing the type, and whether they use d_un.d_val (0) or .d_ptr(1)
TagType basicTagArr[34] = {
    {"NULL", 1},
    {"NEEDED", 0},
    {"PLTRELSZ", 0},
    {"PLTGOT", 0},
    {"HASH", 1},
    {"STRTAB", 1},
    {"SYMTAB", 1},
    {"RELA", 1},
    {"RELASZ", 0},
    {"RELAENT", 0},
    {"STRSZ", 0},
    {"SYMENT", 0},
    {"INIT", 1},
    {"FINI", 1},
    {"SONAME", 0},
    {"RPATH", 0},
    {"SYMBOLIC", 0},
    {"REL", 1},
    {"RELSZ", 0},
    {"RELENT", 0},
    {"PLTREL", 0},
    {"DEBUG", 0},
    {"TEXTREL", 0},
    {"JMPREL", 1},
    {"BIND_NOW", 0},
    {"INIT_ARRAY", 1},
    {"FINI_ARRAY", 1},
    {"INIT_ARRAYSZ", 0},
    {"FINI_ARRAYSZ", 0},
    {"RUNPATH", 0},
    {"FLAGS", 0},
    {"ENCODING", 1},
    {"PREINIT_ARRAYSZ", 0}
};

static const char* printDynEntryType64(Elf64_Dyn* entry){

    TagType tagType;
    if (entry->d_tag < 34) {
        tagType = basicTagArr[entry->d_tag];
    } else {
        //unhandled type code
        tagType.typeStr = "XXXXXX";
        tagType.valOrPtr = 1;
    }
    if(tagType.valOrPtr == 0) {
        printf("0x%016lx %16s %16ld\n", entry->d_tag, tagType.typeStr, entry->d_un.d_val);
    } else if (tagType.valOrPtr == 1) {
        printf("0x%016lx %16s 0x%016lx\n", entry->d_tag, tagType.typeStr, entry->d_un.d_ptr);
    }
}