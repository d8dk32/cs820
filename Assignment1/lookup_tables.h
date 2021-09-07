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