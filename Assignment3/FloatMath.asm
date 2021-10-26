# testing the integer math operations

export add
export sub
export mul
export div
export float1
export float2
export mainx20

mainx20:
	jmp skipData
float1: word 0 #modified by the test harness
float2: word 0 #modified by the test harness
add: 	word 0 
sub: 	word 0 
mul:	word 0 
div:	word 0 

skipData:
	load r1, float1
    load r2, float2
	addf r1, r2
	store r1, add
	subf r1, r2
	store r1, sub
	mulf r1, r2
	store r1, mul
	divf r1, r2
	store r1, div
	halt
