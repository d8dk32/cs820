.global asm_yield
.global asm_yield_cleanup

.type asm_yield, @function
.type asm_yield_cleanup, @function

asm_yield:
    #asm_yield(*TCB curTcb, *TCB nextTcb)
    pushq %rbp
    movq %rsp, %rbp

    #save old regs
    movq %rsp, (%rdi)
    movq %rdi, 0x8(%rdi)
    movq %rsi, 0x10(%rdi)
    movq %rbx, 0x18(%rdi)
    movq %r12, 0x20(%rdi)
    movq %r13, 0x28(%rdi)
    movq %r14, 0x30(%rdi)
    movq %r15, 0x38(%rdi)

    #restore new regs
    movq (%rsi), %rsp
    movq 0x8(%rsi), %rdi
    movq 0x18(%rsi), %rbx
    movq 0x20(%rsi), %r12
    movq 0x28(%rsi), %r13
    movq 0x30(%rsi), %r14
    movq 0x38(%rsi), %r15
    movq 0x10(%rsi), %rsi
    
    #movq (%rsi), %rsp #done on line 21
    popq %rbp
    ret

asm_yield_cleanup:
    #asm_yield_cleanup(NULL, *TCB nextTcb)
    pushq %rbp
    movq %rsp, %rbp

    #ignore old thread, it is complete

    #restore new regs
    movq (%rsi), %rsp
    movq 0x8(%rsi), %rdi
    movq 0x18(%rsi), %rbx
    movq 0x20(%rsi), %r12
    movq 0x28(%rsi), %r13
    movq 0x30(%rsi), %r14
    movq 0x38(%rsi), %r15
    movq 0x10(%rsi), %rsi
    
    #movq (%rsi), %rsp #done on line 44
    popq %rbp
    ret

