#include "lookup_tables.h"

//define some variables that are set upon loading the file and are used repeatedly
//in every other function

int osBits;
int endianness;

Elf32_Ehdr *header32;
Elf64_Ehdr *header64;

//utility function for printg the magic, hex encoded
void printMagic(unsigned char magic[16]){
    for (int i = 0; i < 16; i++){
        printf("%x ", magic[i]);
    }
    printf("\n");
}

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

        //gonna need these headers no mattaer what, so save them off when
        //loading the file
        osBits = blob[EI_CLASS]; 
        endianness = blob[EI_DATA];

        if (osBits == 1) {
            header32 = &blob[0];
        } else {
            header64 = &blob[0];
        }

        return blob;
    } else {
        fprintf(stderr, "File %s could not be opened.\n", filename); 
        exit(-1);
    }
}

// prints the header. identical functionality for 32 vs 64 bits, 
//just split to accomodate differences in data structures defined in elf.h
void h(char *blob) {

    if (osBits == 1) {
        printf("Magic: ");
        printMagic(header32->e_ident);
        printf("Bits: %s\n", fileClass[osBits]);
        printf("Endianness: %s\n", dataEncoding[endianness]); 
        printf("Obj. File Type: %s\n", objFileType(header32->e_type));
        printf("Version: %x\n", header32->e_version);
        printf("Entry Point Addr.: %x\n", header32->e_entry);
        printf("Program Header Offset: %d\n", header32->e_phoff);
        printf("Section Header offset: %d\n", header32->e_shoff);
        printf("Flags: %x\n", header32->e_flags);
        printf("ELF header Size: %d\n", header32->e_ehsize);
        printf("Prog. Hdr. Table Entry Size: %d\n", header32->e_phentsize);
        printf("Prog. Hdr. Table Entry Count: %d\n", header32->e_phnum);
        printf("Section Hdr. Table Entry Size: %d\n", header32->e_shentsize);
        printf("Section Hdr. Table Entry Count: %d\n", header32->e_shnum);
        printf("Section Hdr. String Table Index: %d\n", header32->e_shstrndx);        

    } else {
        printf("Magic: ");
        printMagic(header64->e_ident);
        printf("Bits: %s\n", fileClass[osBits]);
        printf("Endianness: %s\n", dataEncoding[endianness]); 
        printf("Obj. File Type: %s\n", objFileType(header64->e_type));  
        printf("Version: %x\n", header64->e_version);
        printf("Entry Point Addr.: %lx\n", header64->e_entry);
        printf("Program Header Offset: %ld\n", header64->e_phoff);
        printf("Section Header offset: %ld\n", header64->e_shoff);
        printf("Flags: %x\n", header64->e_flags);
        printf("ELF header Size: %d\n", header64->e_ehsize);
        printf("Prog. Hdr. Table Entry Size: %d\n", header64->e_phentsize);
        printf("Prog. Hdr. Table Entry Count: %d\n", header64->e_phnum);
        printf("Section Hdr. Table Entry Size: %d\n", header64->e_shentsize);
        printf("Section Hdr. Table Entry Count: %d\n", header64->e_shnum);
        printf("Section Hdr. String Table Index: %d\n", header64->e_shstrndx);
    }

}

void printSectionNameFromNameOffset(int offset, char *blob) {
    if (osBits == 1) {
        //32 bit
        int shrstrSectionHeaderOffset = header32->e_shoff + (header32->e_shstrndx*sizeof(Elf32_Shdr));
        Elf32_Shdr *secHdr;
        secHdr = &blob[shrstrSectionHeaderOffset]; //the shstrtab section header
        printf("%s", &blob[secHdr->sh_offset+offset]);

    } else {
        //64 bit
        int shrstrSectionHeaderOffset = header64->e_shoff + (header64->e_shstrndx*sizeof(Elf64_Shdr));
        Elf64_Shdr *secHdr;
        secHdr = &blob[shrstrSectionHeaderOffset]; //the shstrtab section header
        printf("%s", &blob[secHdr->sh_offset+offset]);
    }
}

void sectionNames(char *blob) {
    printf(".shrstrtab section names: \n");

    if (osBits == 1){
        //32 bits
        for (int i = 0; i < header32->e_shnum; i++){
            Elf32_Shdr *secHdr;
            secHdr = &blob[header32->e_shoff + i * sizeof(Elf32_Shdr)];
            printSectionNameFromNameOffset(secHdr->sh_name, blob);
            printf("\n");
        }

    } else {
        //64 bits
        for (int i = 0; i < header64->e_shnum; i++){
            Elf64_Shdr *secHdr;
            secHdr = &blob[header64->e_shoff + i * sizeof(Elf64_Shdr)];
            printSectionNameFromNameOffset(secHdr->sh_name, blob);
            printf("\n");
        }
    }
}

void sections(char *blob) {
    
    if (osBits == 1) {
        //32 bits
        printf("There are %d section headers beginning at address 0x%x\n", header32->e_shnum, header32->e_shoff);
        for(int i = 0; i < header32->e_shnum; i++){
            //pull out the correct header
            Elf32_Shdr *secHdr;
            secHdr = &blob[header32->e_shoff + i * sizeof(Elf32_Shdr)];
            printf("[%d] ", i);
            printSectionNameFromNameOffset(secHdr->sh_name, blob); printf(" ");
            printf("%s ", sectionType(secHdr->sh_type));
            printf("%x ", secHdr->sh_addr);
            printf("%x ", secHdr->sh_offset);
            printf("%x", secHdr->sh_size);
            printf("\n");//line break
        }
    } else {
        //64 bits
        printf("There are %d section headers beginning at address 0x%lx\n", header64->e_shnum, header64->e_shoff);
        for(int i = 0; i < header64->e_shnum; i++){
            //pull out the correct header
            Elf64_Shdr *secHdr;
            secHdr = &blob[header64->e_shoff + i * sizeof(Elf64_Shdr)];
            printf("[%d] ", i);
            printSectionNameFromNameOffset(secHdr->sh_name, blob); printf(" ");
            printf("%s ", sectionType(secHdr->sh_type));
            printf("%lx ", secHdr->sh_addr);
            printf("%lx ", secHdr->sh_offset);
            printf("%lx", secHdr->sh_size);
            printf("\n");//line break
        }

    }
    
}

void sectionFlags(char *blob) {
    if (osBits == 1) {
        //32 bits
        for(int i = 0; i < header32->e_shnum; i++){
            //pull out the correct header
            Elf32_Shdr *secHdr;
            secHdr = &blob[header32->e_shoff + i * sizeof(Elf32_Shdr)];
            printSectionNameFromNameOffset(secHdr->sh_name, blob); printf(" ");
            printFlags32(secHdr->sh_flags);
            printf("\n");//line break
        }
    } else {
        //64 bits
        for(int i = 0; i < header64->e_shnum; i++){
            //pull out the correct header
            Elf64_Shdr *secHdr;
            secHdr = &blob[header64->e_shoff + i * sizeof(Elf64_Shdr)];
            printSectionNameFromNameOffset(secHdr->sh_name, blob); printf(" ");
            printFlags64(secHdr->sh_flags);
            printf("\n");//line break
        }

    }
}

//retrieves a section header by name (string literal)
//note: probably will segfault if there is no section with the specified name
Elf32_Shdr* getSectionHeaderByName32(char* name, char* blob){
    Elf32_Shdr* retval;
    //32 bits
    for(int i = 0; i < header64->e_shnum; i++){
        //pull out each section header
        Elf32_Shdr *secHdr;
        secHdr = &blob[header32->e_shoff + i * sizeof(Elf32_Shdr)];
        
        //get that section's name
        int shrstrSectionHeaderOffset = header32->e_shoff + (header32->e_shstrndx*sizeof(Elf32_Shdr));
        Elf32_Shdr *shstrSecHdr;
        shstrSecHdr = &blob[shrstrSectionHeaderOffset]; 
        char* secName = &blob[shstrSecHdr->sh_offset + secHdr->sh_name];
        printf("testing sec name %s\n", secName);

        //comapare the names, return the section if it matches
        if (strcmp(name, secName) == 0) {
            retval = secHdr;
            break;
        }
    }

    return retval;
}

//retrieves a section header by name (string literal)
//note: probably will segfault if there is no section with the specified name
Elf64_Shdr* getSectionHeaderByName64(char* name, char* blob){
    Elf64_Shdr* retval;
    //64 bits
    for(int i = 0; i < header64->e_shnum; i++){
        //pull out each section header
        Elf64_Shdr *secHdr;
        secHdr = &blob[header64->e_shoff + i * sizeof(Elf64_Shdr)];
        
        //get that section's name
        int shrstrSectionHeaderOffset = header64->e_shoff + (header64->e_shstrndx*sizeof(Elf64_Shdr));
        Elf64_Shdr *shstrSecHdr;
        shstrSecHdr = &blob[shrstrSectionHeaderOffset]; 
        char* secName = &blob[shstrSecHdr->sh_offset + secHdr->sh_name];
        printf("testing sec name %s\n", secName);

        //comapare the names, return the section if it matches
        if (strcmp(name, secName) == 0) {
            retval = secHdr;
            break;
        }
    }

    return retval;
}

void symtabNames(char *blob) {
    Elf64_Shdr* symtabHdr = getSectionHeaderByName64(".symtab", blob);
    long numEntries = symtabHdr->sh_size/symtabHdr->sh_entsize;
    printf("Symbol table '.symtab' contains %ld entries:\n", numEntries);
    for(int i = 0; i < numEntries; i++){
        Elf64_Sym* entry;
        entry = &blob[symtabHdr->sh_offset + i*sizeof(Elf64_Sym)];
        printf("Value: %lx, Size: %ld\n", entry->st_value, entry->st_size);
    }
}

void dynsymNames(char *blob) {
    
}

void dynamic(char *blob) {
    
}

int main(int argc, char** argv) {

    //handle incorrect numbers of args
    if (argc < 3) {
        fprintf(stderr, "Program requires a filename and option flag.\n");
        return -1;
    }
    if (argc > 3) {
        fprintf(stderr, "Program received too many arguments.\n");
        return -1;
    }

    //argv[0] is program name, so first 'real' param is at [1]
    char *blob = loadFile(argv[1]);

    //handle option flags
    if (strcmp(argv[2], "-h") == 0) {
        //display header info
        h(blob);
    } else if (strcmp(argv[2], "--section_names") == 0) {
        //dump contents of .shstrtab section 
        sectionNames(blob);
    } else if (strcmp(argv[2], "--sections") == 0) {
        //list section headers
        sections(blob);
    } else if (strcmp(argv[2], "--section_flags") == 0) {
        //list section names and flag value
        sectionFlags(blob);
    } else if (strcmp(argv[2], "--symtab_names") == 0) {
        //dump of .symtab table
        symtabNames(blob);
    } else if (strcmp(argv[2], "--dynsym_names") == 0) {
        //dump of .dynsym symbol table
        dynsymNames(blob);
    } else if (strcmp(argv[2], "--dynamic") == 0) {
        //dump of .dynamic section
        dynamic(blob);
    } else {
        fprintf(stderr, "Invalid option flag.\n");
        return -1;
    }

    return 0;
}