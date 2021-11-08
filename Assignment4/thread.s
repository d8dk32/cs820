	.file	"thread.c"
	.text
	.globl	readyQueueHead
	.bss
	.align 8
	.type	readyQueueHead, @object
	.size	readyQueueHead, 8
readyQueueHead:
	.zero	8
	.globl	cleanupList
	.align 8
	.type	cleanupList, @object
	.size	cleanupList, 8
cleanupList:
	.zero	8
	.text
	.globl	getReadyQueueTail
	.type	getReadyQueueTail, @function
getReadyQueueTail:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	readyQueueHead(%rip), %rax
	testq	%rax, %rax
	jne	.L2
	movl	$0, %eax
	jmp	.L3
.L2:
	movq	readyQueueHead(%rip), %rax
	movq	%rax, -8(%rbp)
	jmp	.L4
.L5:
	movq	-8(%rbp), %rax
	movq	64(%rax), %rax
	movq	%rax, -8(%rbp)
.L4:
	movq	-8(%rbp), %rax
	movq	64(%rax), %rax
	testq	%rax, %rax
	jne	.L5
	movq	-8(%rbp), %rax
.L3:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	getReadyQueueTail, .-getReadyQueueTail
	.globl	getReadyQueueHead
	.type	getReadyQueueHead, @function
getReadyQueueHead:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	readyQueueHead(%rip), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	getReadyQueueHead, .-getReadyQueueHead
	.globl	setReadyQueueHead
	.type	setReadyQueueHead, @function
setReadyQueueHead:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, readyQueueHead(%rip)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	setReadyQueueHead, .-setReadyQueueHead
	.globl	readyQueueLength
	.type	readyQueueLength, @function
readyQueueLength:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	readyQueueHead(%rip), %rax
	testq	%rax, %rax
	jne	.L10
	movl	$0, %eax
	jmp	.L11
.L10:
	movl	$1, -12(%rbp)
	movq	readyQueueHead(%rip), %rax
	movq	%rax, -8(%rbp)
	jmp	.L12
.L13:
	addl	$1, -12(%rbp)
	movq	-8(%rbp), %rax
	movq	64(%rax), %rax
	movq	%rax, -8(%rbp)
.L12:
	movq	-8(%rbp), %rax
	movq	64(%rax), %rax
	testq	%rax, %rax
	jne	.L13
	movl	-12(%rbp), %eax
.L11:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	readyQueueLength, .-readyQueueLength
	.globl	cleanup
	.type	cleanup, @function
cleanup:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	cleanup, .-cleanup
	.globl	threadStart
	.type	threadStart, @function
threadStart:
.LFB10:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, %rdi
	call	*%rax
	movl	$0, %eax
	call	cleanup
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	threadStart, .-threadStart
	.globl	thread_create
	.type	thread_create, @function
thread_create:
.LFB11:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$40, %rsp
	.cfi_offset 3, -24
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	readyQueueHead(%rip), %rax
	testq	%rax, %rax
	jne	.L17
	movl	$88, %edi
	call	malloc@PLT
	movq	%rax, readyQueueHead(%rip)
	movq	readyQueueHead(%rip), %rax
	movq	$0, 64(%rax)
	movq	readyQueueHead(%rip), %rbx
	movl	$65536, %edi
	call	malloc@PLT
	movq	%rax, 72(%rbx)
.L17:
	movl	$88, %edi
	call	malloc@PLT
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	$0, 64(%rax)
	movl	$65536, %edi
	call	malloc@PLT
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 72(%rax)
	movq	-40(%rbp), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-48(%rbp), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 8(%rax)
	call	getReadyQueueTail
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rax, 64(%rdx)
	movq	-24(%rbp), %rax
	addq	$40, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	thread_create, .-thread_create
	.globl	thread_yield
	.type	thread_yield, @function
thread_yield:
.LFB12:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$24, %rsp
	.cfi_offset 3, -24
	movq	readyQueueHead(%rip), %rax
	testq	%rax, %rax
	jne	.L20
	movl	$88, %edi
	call	malloc@PLT
	movq	%rax, readyQueueHead(%rip)
	movq	readyQueueHead(%rip), %rax
	movq	$0, 64(%rax)
	movq	readyQueueHead(%rip), %rbx
	movl	$65536, %edi
	call	malloc@PLT
	movq	%rax, 72(%rbx)
.L20:
	movq	readyQueueHead(%rip), %rax
	movq	64(%rax), %rax
	testq	%rax, %rax
	je	.L23
	movq	readyQueueHead(%rip), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	64(%rax), %rax
	movq	%rax, readyQueueHead(%rip)
	call	getReadyQueueTail
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rax, 64(%rdx)
	movq	-24(%rbp), %rax
	movq	$0, 64(%rax)
	jmp	.L19
.L23:
	nop
.L19:
	addq	$24, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	thread_yield, .-thread_yield
	.globl	thread_self
	.type	thread_self, @function
thread_self:
.LFB13:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	readyQueueHead(%rip), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	thread_self, .-thread_self
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
