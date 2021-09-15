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
        if (size < 4) {
            //file is way too small to possibly be an ELF file so just bail here
            //to avoid seg faults later
            fprintf(stderr, "File '%s' does not match ELF specification.\n", filename); 
            exit(-1);
        }
        char *blob = malloc(size);
        fseek(f, 0, SEEK_SET);
        fread(blob, size, 1, f);
        fclose(f);

        //check to make sure file is *probably* a valid ELF file
        //ensure it has at least 4 bits, and those first 4 bits are '0x7FELF'
        //this is obviously not foolproof but probably is good enough for this assignment
        if (size < 4 || blob[0] != 0x7F || blob[1] != 0x45 || blob[2] != 0x4c || blob[3] != 0x46) {
            fprintf(stderr, "File '%s' does not match ELF specification.\n", filename); 
            exit(-1);
        }
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
        fprintf(stderr, "File '%s' could not be opened.\n", filename); 
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
Elf32_Shdr* getSectionHeaderByName32(char* name, char* blob){
    Elf32_Shdr* retval;
    //32 bits
    for(int i = 0; i < header32->e_shnum; i++){
        //pull out each section header
        Elf32_Shdr *secHdr;
        secHdr = &blob[header32->e_shoff + i * sizeof(Elf32_Shdr)];
        
        //get that section's name
        int shrstrSectionHeaderOffset = header32->e_shoff + (header32->e_shstrndx*sizeof(Elf32_Shdr));
        Elf32_Shdr *shstrSecHdr;
        shstrSecHdr = &blob[shrstrSectionHeaderOffset]; 
        char* secName = &blob[shstrSecHdr->sh_offset + secHdr->sh_name];

        //comapare the names, return the section if it matches
        if (strcmp(name, secName) == 0) {
            retval = secHdr;
            return retval;
        }
    }
    //if the section isn't found, we're just gonna bail.
    exit(1);
    return retval;
}

//retrieves a section header by name (string literal)
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

        //comapare the names, return the section if it matches
        if (strcmp(name, secName) == 0) {
            retval = secHdr;
            return retval;
        }
    }
    //if the section isn't found, we're just gonna bail.
    exit(1);
    return retval;
}

void symtabNames(char *blob, char* symTabName, char* strTabName) {
    if(osBits == 1) {
        //32 bits
        Elf32_Shdr* symtabHdr = getSectionHeaderByName32(symTabName, blob);
        Elf32_Shdr* strtabHdr = getSectionHeaderByName32(strTabName, blob);
        long numEntries = symtabHdr->sh_size/symtabHdr->sh_entsize;
        printf("Symbol table '%s' contains %ld entries:\n", symTabName, numEntries);
        printf("Num  %10s %10s %10s %10s %10s %10s %10s\n", "Value", "Size", "Type", "Bind", "Vis", "Ndx", "Name");
        for(int i = 0; i < numEntries; i++){
            Elf32_Sym* entry;
            entry = &blob[symtabHdr->sh_offset + i*sizeof(Elf32_Sym)];
            printf("%4d %10x %10d, %10s %10s %10s %10d %10s\n", i, entry->st_value, entry->st_size, printStType(entry->st_info), printStBind(entry->st_info), printStVis(entry->st_other), entry->st_shndx,  &blob[strtabHdr->sh_offset + entry->st_name]);
        }
    } else {
        //64 bits
        Elf64_Shdr* symtabHdr = getSectionHeaderByName64(symTabName, blob);
        Elf64_Shdr* strtabHdr = getSectionHeaderByName64(strTabName, blob);
        long numEntries = symtabHdr->sh_size/symtabHdr->sh_entsize;
        printf("Symbol table '%s' contains %ld entries:\n", symTabName, numEntries);
        printf("Num  %10s %10s %10s %10s %10s %10s %10s\n", "Value", "Size", "Type", "Bind", "Vis", "Ndx", "Name");
        for(int i = 0; i < numEntries; i++){
            Elf64_Sym* entry;
            entry = &blob[symtabHdr->sh_offset + i*sizeof(Elf64_Sym)];
            printf("%4d %10lx %10ld, %10s %10s %10s %10d %10s\n", i, entry->st_value, entry->st_size, printStType(entry->st_info), printStBind(entry->st_info), printStVis(entry->st_other), entry->st_shndx,  &blob[strtabHdr->sh_offset + entry->st_name]);
        }
    }
}

void dynamic(char *blob) {
    if (osBits == 1){
        //32 bit
        Elf32_Shdr* dynamicHdr = getSectionHeaderByName32(".dynamic", blob);
        Elf32_Shdr* dynstrHdr = getSectionHeaderByName32(".dynstr", blob);
        long numEntries = dynamicHdr->sh_size/dynamicHdr->sh_entsize;
        printf(".dynamic section at offset %x contains %ld entries:\n", dynamicHdr->sh_offset, numEntries);
        printf("%16s %16s %16s", "Tag", "Type", "Name/Value\n");
        for(int i = 0; i < numEntries; i++){
            Elf32_Dyn* entry;
            entry = &blob[dynamicHdr->sh_offset + i*sizeof(Elf32_Dyn)];
            printDynEntryType32(entry, dynstrHdr, blob);
        }
    } else {
        //64 bit
        Elf64_Shdr* dynamicHdr = getSectionHeaderByName64(".dynamic", blob);
        Elf64_Shdr* dynstrHdr = getSectionHeaderByName64(".dynstr", blob);
        long numEntries = dynamicHdr->sh_size/dynamicHdr->sh_entsize;
        printf(".dynamic section at offset %lx contains %ld entries:\n", dynamicHdr->sh_offset, numEntries);
        printf("%16s %16s %16s", "Tag", "Type", "Name/Value\n");
        for(int i = 0; i < numEntries; i++){
            Elf64_Dyn* entry;
            entry = &blob[dynamicHdr->sh_offset + i*sizeof(Elf64_Dyn)];
            printDynEntryType64(entry, dynstrHdr, blob);
        }
    }
    
}

void programHeaders(char* blob) {
    if (osBits == 1) {
        //32 bits
        for(int i = 0; i < header32->e_phnum; i++) {
            //iterate over program headers
            Elf32_Phdr* progHdr;
            progHdr = &blob[header32->e_phoff + i*sizeof(Elf32_Phdr)];
            printf("%16s 0x%016x 0x%016x 0x%016x 0x%016x 0x%016x %16s 0x%016x\n",
            parseProgHdrType32(progHdr->p_type), progHdr->p_offset, progHdr->p_vaddr,
            progHdr->p_paddr, progHdr->p_filesz, progHdr->p_memsz, 
            parseProgHdrFlags32(progHdr->p_flags), progHdr->p_align);
        }
    } else {
        //64 bits
        printf("Entry point 0x%lx\n", header64->e_entry);
        printf("There are %d program headers starting at offset %ld\n", header64->e_phnum, header64->e_phoff);
        printf("Program headers:\n");
        printf("%16s %16s %16s %16s %16s %16s %16s %16s\n", "Type", "Offset", "Virt.Addr.", "Phys.Addr.", "Filesize", "Memsize", "Flag", "Align");
        for(int i = 0; i < header64->e_phnum; i++) {
            //iterate over program headers
            Elf64_Phdr* progHdr;
            progHdr = &blob[header64->e_phoff + i*sizeof(Elf64_Phdr)];
            printf("%16s 0x%016lx 0x%016lx 0x%016lx 0x%016lx 0x%016lx %16s 0x%016lx\n",
            parseProgHdrType64(progHdr->p_type), progHdr->p_offset, progHdr->p_vaddr,
            progHdr->p_paddr, progHdr->p_filesz, progHdr->p_memsz, 
            parseProgHdrFlags64(progHdr->p_flags), progHdr->p_align);
        }
    }
}

void segmentByName(char* blob, char* segName) {
    if (osBits == 1) {
        //32Bit
        Elf32_Shdr* secHdr = getSectionHeaderByName32(segName, blob);
        printf("Hex dump of section '%s':\n", segName);
        int byteCounter = 0;
        int rowsSoFar = 0;
        while (byteCounter < secHdr->sh_size) {
            //read byte by byte
            printf("0x%016x   ", secHdr->sh_offset + rowsSoFar*16);
            int bytesCollected = 0;
            char asciiRow[] = {46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 0}; //................\0
            while (bytesCollected < 16 && byteCounter < secHdr->sh_size) {
                char curByte = blob[secHdr->sh_offset + byteCounter];
                printf("%02x ", curByte);
                if (curByte > 31 && curByte < 127) {
                    asciiRow[bytesCollected] = curByte;
                }
                bytesCollected++;
                byteCounter++;
            }
            while(bytesCollected < 16) {
                //this section will happen when it didn't read thru a whole 16byte row
                //before it reached the end of the section. print out some blank space
                //to keep everything lined up nice.
                printf("   ");
                bytesCollected++;
            }
            printf("   %s\n", asciiRow);
            rowsSoFar++;
        }

    } else {
        //64 bit
        Elf64_Shdr* secHdr = getSectionHeaderByName64(segName, blob);
        printf("Hex dump of section '%s':\n", segName);
        int byteCounter = 0;
        int rowsSoFar = 0;
        while (byteCounter < secHdr->sh_size) {
            //read byte by byte
            printf("0x%016lx   ", secHdr->sh_offset + rowsSoFar*16);
            int bytesCollected = 0;
            char asciiRow[] = {46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 0}; //................\0
            while (bytesCollected < 16 && byteCounter < secHdr->sh_size) {
                char curByte = blob[secHdr->sh_offset + byteCounter];
                printf("%02x ", curByte);
                if (curByte > 31 && curByte < 127) {
                    asciiRow[bytesCollected] = curByte;
                }
                bytesCollected++;
                byteCounter++;
            }
            while(bytesCollected < 16) {
                //this section will happen when it didn't read thru a whole 16byte row
                //before it reached the end of the section. print out some blank space
                //to keep everything lined up nice.
                printf("   ");
                bytesCollected++;
            }
            printf("   %s\n", asciiRow);
            rowsSoFar++;
        }

    }
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
    char *blob = loadFile(argv[2]);

    //handle option flags
    if (strcmp(argv[1], "-h") == 0) {
        //display header info
        h(blob);
    } else if (strcmp(argv[1], "--section_names") == 0) {
        //dump contents of .shstrtab section 
        sectionNames(blob);
    } else if (strcmp(argv[1], "--sections") == 0) {
        //list section headers
        sections(blob);
    } else if (strcmp(argv[1], "--section_flags") == 0) {
        //list section names and flag value
        sectionFlags(blob);
    } else if (strcmp(argv[1], "--symtab_names") == 0) {
        //dump of .symtab symbol table
        symtabNames(blob, ".symtab", ".strtab");
    } else if (strcmp(argv[1], "--dynsym_names") == 0) {
        //dump of .dynsym symbol table
        symtabNames(blob, ".dynsym", ".dynstr");
    } else if (strcmp(argv[1], "--dynamic") == 0) {
        //dump of .dynamic section
        dynamic(blob);
    } else if (strcmp(argv[1], "--program_headers") == 0){
        programHeaders(blob);
    } else if (memcmp(argv[1], "--segment.", 10) == 0) {
        segmentByName(blob, argv[1]+10);
    } else {
        fprintf(stderr, "Invalid option flag.\n");
        return -1;
    }

    return 0;
}