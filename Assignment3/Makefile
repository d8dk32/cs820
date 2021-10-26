buildAll: testvm vmx20

testvm:
	gcc testHarness.c vmx20.c -o testvm

vmx20:
	gcc -c vmx20.c

clean:
	find . -type f -iname '*.o' -delete
	find . -type f -iname 'testvm' -delete

