# simple laoding and storing

export x1
export y1
export z1
export w1
export u1
export mainx20

mainx20:
	jmp skipData
x1: 	word 1
y1: 	word 0
z1:		word 0
w1:		word 0
u1:		word 0
skipData:
	load r1, x1
    store r1, y1
	ldaddr r2, x1
	stind r1, 2(r2)
	ldimm r3, 1
	store r3, w1
	ldind r4, 2(r2)
	store r4, u1
	halt
