#
# very simple program with some data

export x1
export y1
export poo
export mainx20

mainx20:
	jmp skipData
x1: 	word 0
y1: 	word 0
poo:	word 0
skipData:
	load r1, x1
	ldimm r2, 50
	addi r2, r1
    store r2, y1
	halt
