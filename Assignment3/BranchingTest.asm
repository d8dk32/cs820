# testing the integer math operations

export x
export y
export z
export mainx20

mainx20:
	jmp skipData
x:	word 0
y:  word 1
z: 	word 0
skipData:
	load r1, x
	load r2, y
	blt r1, r2, z1
	bgt r1, r2, z2
	beq r1, r2, z3
z1:
	ldimm r3, 1
	store r3, z
	jmp end
z2:
	ldimm r3, 2
	store r3, z
	jmp end
z3:
	ldimm r3, 3
	store r3, z 
end:
	halt
