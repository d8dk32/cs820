alloc:
	gcc -c alloc.c asmAlloc.s
	ar rcs liballoc.a *.o

myTests:
	gcc -g testHarness.c alloc.c asmAlloc.s -o test