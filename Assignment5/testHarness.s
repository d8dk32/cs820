	.file	"testHarness.c"
	.text
	.globl	fakeFinalizer
	.type	fakeFinalizer, @function
fakeFinalizer:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	fakeFinalizer, .-fakeFinalizer
	.section	.rodata
.LC0:
	.string	"no test number specified"
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	%edi, -36(%rbp)
	movq	%rsi, -48(%rbp)
	cmpl	$2, -36(%rbp)
	je	.L4
	leaq	.LC0(%rip), %rdi
	call	puts@PLT
	movl	$-1, %eax
	jmp	.L5
.L4:
	movq	-48(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	atoi@PLT
	movl	%eax, -24(%rbp)
	cmpl	$1, -24(%rbp)
	jne	.L6
	movl	$100, %edi
	call	memInitialize@PLT
	movl	%eax, -20(%rbp)
	leaq	fakeFinalizer(%rip), %rsi
	movl	$20, %edi
	call	memAllocate@PLT
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	leaq	56(%rax), %rdx
	movq	-16(%rbp), %rax
	addq	$56, %rax
	movq	%rdx, (%rax)
	movl	$0, %esi
	movl	$10, %edi
	call	memAllocate@PLT
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	addq	$40, %rax
	movabsq	$1234605616436508552, %rcx
	movq	%rcx, (%rax)
	movl	$0, %esi
	movl	$42, %edi
	call	memAllocate@PLT
	call	memDump@PLT
.L6:
	movl	$0, %eax
.L5:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
