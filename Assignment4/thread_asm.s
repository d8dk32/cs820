.global asm_yield
.global thread_start

.type asm_yield, @function
.type thread_start, @function

asm_yield:
    #asm_yield(*TCB curTcb, *TCB nextTcb)
    pushq %rbp
    movq %rsp, %rbp




    movq (%rsi), %rsp
    popq %rbp
    ret

thread_start:
    pushq %rbp
    movq %rsp, %rbp
