# testing the integer math operations

export x1
export x2
export x3
export x4
export pn
export mainx20

mainx20:
	jmp skipData
pn:     word 0
x1:	    word 0
x2:     word 0
x3:     word 0
x4:     word 0
skipData:
    getpn r0
    store r0, pn
    ldaddr r1, x1
    getpid r2
    addi r1, r2
    stind r2, 0(r1)
    halt

