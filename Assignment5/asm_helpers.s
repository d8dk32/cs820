.global GC_get_rdi
.global GC_get_rsi
.global GC_get_rbx

.type GC_get_rdi, @function
.type GC_get_rsi, @function
.type GC_get_rbx, @function

GC_get_rdi:
    movq %rdi, %rax

GC_get_rsi:
    movq %rsi, %rax

GC_get_rbx:
    movq %rbx, %rax

    