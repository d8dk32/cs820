generate:
# suppressing specific warning type cause I'm doing some interesting
#    things with pointers that gcc finds suspicious.
	gcc -Wno-incompatible-pointer-types binary_info.c -o binary_info

clean:
	rm binary_info
	find . -type f -iname '*.o' -delete
	find . -type f -iname '*.out' -delete
