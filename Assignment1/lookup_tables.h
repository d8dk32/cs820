//file containing lookup tables for header data, etc.
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
char* objFileType[5] = {
    "No Type",
    "Relocatable",
    "Executable",
    "Shared obj. file",
    "Core file"
};

//sh_type value (section type)
char* sectionType[20] = {
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