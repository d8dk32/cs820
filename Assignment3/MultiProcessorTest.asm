# testing the integer math operations

export x
export y
export mainx20

mainx20:
	jmp skipData
x:	word 0
y:  word 0
skipData:
	ldimm r2, 2
    push r2
    call func
    pop r3
    store r3, y
    halt
func:
    ldimm r1, 1
    store r1, x
    ret
