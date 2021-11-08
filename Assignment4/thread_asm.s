.global asm_yield

.type asm_yield, @function

asm_yield:
    #asm_yield(*TCB curTcb, *TCB nextTcb)
    pushq %rbp
    movq %rsp, %rbp

    #save old regs
    movq %rdi, (%rdi)
    movq %rsi, 0x8(%rdi)
    movq %rsp, 0x10(%rdi)
    movq %rbx, 0x18(%rdi)
    movq %r12, 0x20(%rdi)
    movq %r13, 0x28(%rdi)
    movq %r14, 0x30(%rdi)
    movq %r15, 0x38(%rdi)

    # SHOULD THE LOWEST ADDRESS OF THE TCB BE ITS STACK

    #restore new regs
    movq (%rsi), %rdi
    movq 0x8(%rsi), %rsi
    movq 0x10(%rsi), %rsp
    movq 0x18(%rsi), %rbx
    movq 0x20(%rsi), %r12
    movq 0x28(%rsi), %r13
    movq 0x30(%rsi), %r14
    movq 0x38(%rsi), %r15

    #is there additional stack manipulation I need here?

    movq (%rsi), %rsp
    popq %rbp
    ret
