#
# very simple program with some data

export x1
export y1
export poo
x1: 	word 0
y1: 	word 0
poo:	word 0
	load r1, x1
	ldimm r2, 50
	addi r2, r1
    store r2, y1
	halt
