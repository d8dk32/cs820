#
# very simple program with some data

export x1
export y1
export poo
export mainx20

mainx20:
	jmp skipData
x1: 	word 1
y1: 	word 3
poo:	word 2
skipData:
	load r1, x1
	ldind r2, 1(r1)
	addi r1, r2
    store r1, y1
	halt
