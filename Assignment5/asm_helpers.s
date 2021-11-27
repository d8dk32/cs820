.global GC_get_rdi
.global GC_get_rsi
.global GC_get_rbx
.global GC_get_rsp
.global GC_get_rbp

.type GC_get_rdi, @function
.type GC_get_rsi, @function
.type GC_get_rbx, @function
.type GC_get_rsp, @function
.type GC_get_rbp, @function

GC_get_rdi:
    pushq %rbp
    movq %rsp, %rbp
    movq %rdi, %rax
    popq %rbp
    ret

GC_get_rsi:
    pushq %rbp
    movq %rsp, %rbp
    movq %rsi, %rax
    popq %rbp
    ret

GC_get_rbx:
    pushq %rbp
    movq %rsp, %rbp
    movq %rbx, %rax
    popq %rbp
    ret

GC_get_rsp:
    pushq %rbp
    movq %rsp, %rbp
    movq %rsp, %rax
    popq %rbp
    ret

GC_get_rbp:
    movq %rbp, %rax
    pushq %rbp
    movq %rsp, %rbp
    popq %rbp
    ret

    