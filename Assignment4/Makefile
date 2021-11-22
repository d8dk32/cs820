#
# Sample makefile for creating threads library
#

CC = gcc
CFLAGS = -g -Wall -std=c99
SRC = thread.c thread_asm.s
OBJ = thread.o thread_asm.o

thread:
	$(CC) -c $(SRC) $(CFLAGS)
	ar -cr libthread.a $(OBJ)

myTests: 
	gcc -g -O0 testHarness.c thread.c thread_asm.s -o test