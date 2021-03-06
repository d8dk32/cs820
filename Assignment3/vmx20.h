//
// vmx20.h
//
// public interface to the vmx20 implementation
//

// maximum number of processors
#define VMX20_MAX_PROCESSORS 16

// error codes for loadExecutableFile
#define VMX20_FILE_NOT_FOUND -1
#define VMX20_FILE_CONTAINS_OUTSYMBOLS -2
#define VMX20_FILE_IS_NOT_VALID -3

// error codes for execute
//   VMX20_ADDRESS_OUT_OF_RANGE and VMX20_ILLEGAL_INSTRUCTION are also
//     used by disassemble
#define VMX20_NORMAL_TERMINATION 0
#define VMX20_DIVIDE_BY_ZERO -1
#define VMX20_ADDRESS_OUT_OF_RANGE -2
#define VMX20_ILLEGAL_INSTRUCTION -3

// load an executable file
//   only one executable file may be loaded at a time
//   the function returns 1 if successful and 0 otherwise
//   if 0 is returned then an error number is returned through the second
//     parameter if it is not NULL
//   the following error numbers are supported:
//     VMX20_FILE_NOT_FOUND
//     VMX20_FILE_CONTAINS_OUTSYMBOLS
//     VMX20_FILE_IS_NOT_VALID
//
int loadExecutableFile(char *filename, int *errorNumber);

// get the address of a symbol in the current executable file
//   the label must be a symbol in the insymbol section of the executable file
//   the address is returned through the second parameter
//   the function returns 1 if successful and 0 otherwise
int getAddress(char *label, unsigned int *outAddr);

// read a word from memory
//   the word is returned through the second parameter
//   the function returns 1 if successful and 0 otherwise
int getWord(unsigned int addr, int *outWord);

// write a word to memory
//   the function returns 1 if successful and 0 otherwise
int putWord(unsigned int addr, int word);

// execute the current loaded executable file
//   the function returns 1 if all processors are successfully started and
//     0 otherwise
//   the first parameter specifies the number of processors to use
//   the second parameter provides an initial SP value for each processor
//   the third parameter is used to return the termination status for
//     each processor
//   the following termination statuses are supported:
//     VMX20_NORMAL_TERMINATION
//     VMX20_DIVIDE_BY_ZERO
//     VMX20_ADDRESS_OUT_OF_RANGE
//     VMX20_ILLEGAL_INSTRUCTION
//   the fourth parameter is a Boolean indicating whether an instruction
//     trace should be be printed to stderr
//   Note: that all other registers will be initialized to 0, including
//     the PC and the FP.
//
int execute(unsigned int numProcessors, unsigned int initialSP[],
      int terminationStatus[], int trace);

// disassemble the word at the given address
//   return 1 if successful and 0 otherwise
//   the first parameter contains the address of the word to disassemble
//   the second parameter is a pointer to a buffer where the output should be
//     placed
//   the output will be the opcode followed by a space, followed by the
//     comma separated operands (if any). each comma will be followed by
//     a space. PC-relative addresses are converted to absolute addresses
//     and displayed in decimal. offsets and immediate constants are displayed
//     in decimal.
//   the caller can rely that the output will certainly not consume more than
//     100 characters
//   the third parameter is used to return an error code if an error is
//     encountered
//   the following error codes are supported:
//     VMX20_ADDRESS_OUT_OF_RANGE
//     VMX20_ILLEGAL_INSTRUCTION
int disassemble(unsigned int address, char *buffer, int *errorNumber);
