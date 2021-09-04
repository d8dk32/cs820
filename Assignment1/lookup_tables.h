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

//There are more legal values besides these, hopefully that doesn't bite me
char* objFileType[5] = {
    "No Type",
    "Relocatable",
    "Executable",
    "Shared obj. file",
    "Core file"
};