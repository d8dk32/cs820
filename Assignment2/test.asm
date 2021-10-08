export mainx20
export x2
jmp mainx20
x1: 
    word 0
x2: 
    word 0
x3: 
    word 5
mainx20:
    load r1, x1
    load r2, x2
    call func1
    load r3, x3
    jmp mainx20
    jmp skip2

x4:
    word 0
skip2:
    store r3, x4
    halt

word 0
word 0

func1:
    load r1, x1
    load r2, x2
    call func2
    ret

func2:
    store r2, x1
    jmp skip3
    word 0
    skip3:
        ret

word 1
word 2





