# testing the integer math operations

export add
export sub
export mul
export div
export mainx20

mainx20:
	jmp skipData
add: 	word 0 #should end up as 5
sub: 	word 0 #should end up as 2
mul:	word 0 #should end up as 6
div:	word 0 #should end up as 2

skipData:
	ldimm r1, 2
    ldimm r2, 3
	addi r1, r2
	store r1, add
	subi r1, r2
	store r1, sub
	muli r1, r2
	store r1, mul
	divi r1, r2
	store r1, div
	halt
