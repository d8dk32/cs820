#define FORMAT_STRING "%s"
#define OUTPUT "Hello, world\n"

#include <stdio.h>

int main(int argc, char* argv[]) {
    printf(FORMAT_STRING, OUTPUT);
    return 0;
}