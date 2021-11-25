	.file	"memalloc.c"
	.text
	.local	initializedSpace
	.comm	initializedSpace,8,8
	.local	heapBasePtr
	.comm	heapBasePtr,8,8
	.local	inFinalizeCall
	.comm	inFinalizeCall,4,4
	.globl	memInitialize
	.type	memInitialize, @function
memInitialize:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	initializedSpace(%rip), %rax
	testq	%rax, %rax
	je	.L2
	movl	$0, %eax
	jmp	.L3
.L2:
	cmpq	$0, -8(%rbp)
	jne	.L4
	movl	$0, %eax
	jmp	.L3
.L4:
	movq	-8(%rbp), %rax
	movq	%rax, initializedSpace(%rip)
	movq	-8(%rbp), %rax
	addq	$2, %rax
	salq	$3, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, heapBasePtr(%rip)
	movq	heapBasePtr(%rip), %rax
	testq	%rax, %rax
	jne	.L5
	movl	$0, %eax
	jmp	.L3
.L5:
	movq	heapBasePtr(%rip), %rax
	movq	-8(%rbp), %rdx
	movq	%rdx, (%rax)
	movl	$1, %eax
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	memInitialize, .-memInitialize
	.globl	checkIfValueIsAllocatedBlockAddr
	.type	checkIfValueIsAllocatedBlockAddr, @function
checkIfValueIsAllocatedBlockAddr:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -40(%rbp)
	movq	heapBasePtr(%rip), %rax
	cmpq	%rax, -40(%rbp)
	jb	.L7
	movq	heapBasePtr(%rip), %rax
	movq	%rax, %rdx
	movq	initializedSpace(%rip), %rax
	addq	%rdx, %rax
	addq	$2, %rax
	cmpq	%rax, -40(%rbp)
	jbe	.L8
.L7:
	movl	$0, %eax
	jmp	.L9
.L8:
	movq	heapBasePtr(%rip), %rax
	movq	%rax, -24(%rbp)
	jmp	.L10
.L12:
	movq	-24(%rbp), %rax
	movq	(%rax), %rdx
	movabsq	$2305843009213693951, %rax
	andq	%rdx, %rax
	movq	%rax, -16(%rbp)
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	shrq	$63, %rax
	movq	%rax, -8(%rbp)
	movq	-24(%rbp), %rax
	cmpq	%rax, -40(%rbp)
	jb	.L11
	movq	-16(%rbp), %rax
	addq	$2, %rax
	leaq	0(,%rax,8), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	cmpq	%rax, -40(%rbp)
	ja	.L11
	cmpq	$1, -8(%rbp)
	jne	.L11
	movq	-24(%rbp), %rax
	jmp	.L9
.L11:
	movq	-16(%rbp), %rax
	addq	$2, %rax
	salq	$3, %rax
	addq	%rax, -24(%rbp)
.L10:
	movq	heapBasePtr(%rip), %rax
	movq	initializedSpace(%rip), %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	cmpq	%rax, %rdx
	ja	.L12
	movl	$0, %eax
.L9:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	checkIfValueIsAllocatedBlockAddr, .-checkIfValueIsAllocatedBlockAddr
	.type	findSpace, @function
findSpace:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	-48(%rbp), %rax
	movq	(%rax), %rdx
	movabsq	$2305843009213693951, %rax
	andq	%rdx, %rax
	movq	%rax, -32(%rbp)
	movq	-48(%rbp), %rax
	movq	(%rax), %rax
	shrq	$63, %rax
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.L14
	movq	-40(%rbp), %rax
	addq	$2, %rax
	cmpq	%rax, -32(%rbp)
	jbe	.L14
	movabsq	$-9223372036854775808, %rax
	orq	-40(%rbp), %rax
	movq	%rax, %rdx
	movq	-48(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-40(%rbp), %rax
	addq	$1, %rax
	cmpq	%rax, -32(%rbp)
	jbe	.L15
	movq	-40(%rbp), %rax
	addq	$2, %rax
	leaq	0(,%rax,8), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -16(%rbp)
	movq	-32(%rbp), %rax
	subq	-40(%rbp), %rax
	leaq	-2(%rax), %rdx
	movq	-16(%rbp), %rax
	movq	%rdx, (%rax)
.L15:
	movq	-48(%rbp), %rax
	jmp	.L16
.L14:
	movq	-32(%rbp), %rax
	addq	$2, %rax
	leaq	0(,%rax,8), %rdx
	movq	-48(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -8(%rbp)
	movq	heapBasePtr(%rip), %rax
	movq	initializedSpace(%rip), %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	cmpq	%rax, -8(%rbp)
	jbe	.L17
	movl	$0, %eax
	jmp	.L16
.L17:
	movq	-32(%rbp), %rax
	addq	$2, %rax
	leaq	0(,%rax,8), %rdx
	movq	-48(%rbp), %rax
	addq	%rax, %rdx
	movq	-40(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	findSpace
.L16:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	findSpace, .-findSpace
	.type	callDestructor, @function
callDestructor:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$1, inFinalizeCall(%rip)
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, %rdi
	call	*%rax
	movl	$0, inFinalizeCall(%rip)
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	callDestructor, .-callDestructor
	.type	collectGarbage, @function
collectGarbage:
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
	.size	collectGarbage, .-collectGarbage
	.section	.rodata
	.align 8
.LC0:
	.string	"Cannot call memAllocate from destructor.\n"
	.text
	.globl	memAllocate
	.type	memAllocate, @function
memAllocate:
.LFB10:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movl	inFinalizeCall(%rip), %eax
	cmpl	$1, %eax
	jne	.L22
	movq	stderr(%rip), %rax
	movq	%rax, %rcx
	movl	$41, %edx
	movl	$1, %esi
	leaq	.LC0(%rip), %rdi
	call	fwrite@PLT
	movl	$-1, %edi
	call	exit@PLT
.L22:
	movq	initializedSpace(%rip), %rax
	testq	%rax, %rax
	jne	.L23
	movl	$0, %eax
	jmp	.L24
.L23:
	movq	heapBasePtr(%rip), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	findSpace
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	je	.L25
	movq	-8(%rbp), %rax
	leaq	8(%rax), %rdx
	movq	-32(%rbp), %rax
	movq	%rax, (%rdx)
	movq	-8(%rbp), %rax
	addq	$16, %rax
	jmp	.L24
.L25:
	movl	$0, %eax
	call	collectGarbage
	movq	heapBasePtr(%rip), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	findSpace
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	je	.L26
	movq	-8(%rbp), %rax
	leaq	8(%rax), %rdx
	movq	-32(%rbp), %rax
	movq	%rax, (%rdx)
	movq	-8(%rbp), %rax
	addq	$16, %rax
	jmp	.L24
.L26:
	movl	$0, %eax
.L24:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	memAllocate, .-memAllocate
	.section	.rodata
	.align 8
.LC1:
	.string	"Global Memory: start=0x%016lx, end=0x%016lx, length=%ld\n"
.LC2:
	.string	"*"
.LC3:
	.string	""
.LC4:
	.string	" 0x%016lx %016lx%s\n"
	.text
	.type	dumpGlobalMem, @function
dumpGlobalMem:
.LFB11:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	leaq	__data_start(%rip), %rax
	movq	%rax, -16(%rbp)
	leaq	_end(%rip), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rax
	subq	%rax, %rdx
	movq	%rdx, %rax
	sarq	$3, %rax
	movq	%rax, %rcx
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	$0, -24(%rbp)
	jmp	.L28
.L31:
	movq	-24(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	checkIfValueIsAllocatedBlockAddr
	testq	%rax, %rax
	je	.L29
	leaq	.LC2(%rip), %rax
	jmp	.L30
.L29:
	leaq	.LC3(%rip), %rax
.L30:
	movq	-24(%rbp), %rdx
	leaq	0(,%rdx,8), %rcx
	movq	-16(%rbp), %rdx
	addq	%rcx, %rdx
	movq	(%rdx), %rdx
	movq	-24(%rbp), %rcx
	leaq	0(,%rcx,8), %rsi
	movq	-16(%rbp), %rcx
	addq	%rsi, %rcx
	movq	%rcx, %rsi
	movq	%rax, %rcx
	leaq	.LC4(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	addq	$1, -24(%rbp)
.L28:
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rax
	subq	%rax, %rdx
	movq	%rdx, %rax
	sarq	$3, %rax
	cmpq	%rax, -24(%rbp)
	jb	.L31
	movl	$10, %edi
	call	putchar@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	dumpGlobalMem, .-dumpGlobalMem
	.section	.rodata
.LC5:
	.string	"Registers\n"
.LC6:
	.string	" "
	.align 8
.LC7:
	.string	"rbx %016lx%s rsi %016lx%s rdi %016lx%s"
.LC8:
	.string	"\n"
	.text
	.type	dumpRegisters, @function
dumpRegisters:
.LFB12:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	pushq	%rbx
	subq	$32, %rsp
	.cfi_offset 12, -24
	.cfi_offset 3, -32
	call	GC_get_rdi@PLT
	movq	%rax, -40(%rbp)
	call	GC_get_rsi@PLT
	movq	%rax, -32(%rbp)
	call	GC_get_rbx@PLT
	movq	%rax, -24(%rbp)
	leaq	.LC5(%rip), %rdi
	call	puts@PLT
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	checkIfValueIsAllocatedBlockAddr
	testq	%rax, %rax
	je	.L33
	leaq	.LC2(%rip), %rbx
	jmp	.L34
.L33:
	leaq	.LC6(%rip), %rbx
.L34:
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	checkIfValueIsAllocatedBlockAddr
	testq	%rax, %rax
	je	.L35
	leaq	.LC2(%rip), %r12
	jmp	.L36
.L35:
	leaq	.LC6(%rip), %r12
.L36:
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	checkIfValueIsAllocatedBlockAddr
	testq	%rax, %rax
	je	.L37
	leaq	.LC2(%rip), %rax
	jmp	.L38
.L37:
	leaq	.LC6(%rip), %rax
.L38:
	movq	-40(%rbp), %rcx
	movq	-32(%rbp), %rdx
	movq	-24(%rbp), %rsi
	subq	$8, %rsp
	pushq	%rbx
	movq	%rcx, %r9
	movq	%r12, %r8
	movq	%rdx, %rcx
	movq	%rax, %rdx
	leaq	.LC7(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	addq	$16, %rsp
	leaq	.LC8(%rip), %rdi
	call	puts@PLT
	nop
	leaq	-16(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	dumpRegisters, .-dumpRegisters
	.section	.rodata
.LC9:
	.string	"Heap - 2 word header:"
.LC10:
	.string	"Marked"
.LC11:
	.string	"Unmarked"
.LC12:
	.string	"Allocated"
.LC13:
	.string	"Free"
.LC14:
	.string	"\nBlock - %ld words - %s - %s "
.LC15:
	.string	"- %016lx\n"
.LC16:
	.string	"\n 0x%016lx: "
.LC17:
	.string	"%016lx"
.LC18:
	.string	"* "
.LC19:
	.string	"  "
	.text
	.type	dumpHeap, @function
dumpHeap:
.LFB13:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	leaq	.LC9(%rip), %rdi
	call	puts@PLT
	movq	heapBasePtr(%rip), %rax
	movq	%rax, -40(%rbp)
	jmp	.L40
.L53:
	movq	-40(%rbp), %rax
	movq	(%rax), %rdx
	movabsq	$2305843009213693951, %rax
	andq	%rdx, %rax
	movq	%rax, -24(%rbp)
	movq	-40(%rbp), %rax
	movq	(%rax), %rax
	shrq	$63, %rax
	movq	%rax, -16(%rbp)
	movq	-40(%rbp), %rax
	movq	(%rax), %rax
	shrq	$62, %rax
	andl	$2, %eax
	movq	%rax, -8(%rbp)
	cmpq	$1, -8(%rbp)
	jne	.L41
	leaq	.LC10(%rip), %rdx
	jmp	.L42
.L41:
	leaq	.LC11(%rip), %rdx
.L42:
	cmpq	$1, -16(%rbp)
	jne	.L43
	leaq	.LC12(%rip), %rax
	jmp	.L44
.L43:
	leaq	.LC13(%rip), %rax
.L44:
	movq	-24(%rbp), %rsi
	movq	%rdx, %rcx
	movq	%rax, %rdx
	leaq	.LC14(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	cmpq	$1, -16(%rbp)
	jne	.L45
	movq	-40(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rsi
	leaq	.LC15(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	jmp	.L46
.L45:
	movl	$10, %edi
	call	putchar@PLT
.L46:
	cmpq	$1, -16(%rbp)
	jne	.L47
	movq	$0, -32(%rbp)
	jmp	.L48
.L52:
	movq	-32(%rbp), %rcx
	movabsq	$2635249153387078803, %rdx
	movq	%rcx, %rax
	mulq	%rdx
	movq	%rcx, %rax
	subq	%rdx, %rax
	shrq	%rax
	addq	%rdx, %rax
	shrq	$2, %rax
	movq	%rax, %rdx
	salq	$3, %rdx
	subq	%rax, %rdx
	movq	%rcx, %rax
	subq	%rdx, %rax
	testq	%rax, %rax
	jne	.L49
	movq	-32(%rbp), %rax
	addq	$2, %rax
	leaq	0(,%rax,8), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, %rsi
	leaq	.LC16(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
.L49:
	movq	-32(%rbp), %rax
	addq	$2, %rax
	leaq	0(,%rax,8), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rsi
	leaq	.LC17(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-32(%rbp), %rax
	addq	$2, %rax
	leaq	0(,%rax,8), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	checkIfValueIsAllocatedBlockAddr
	testq	%rax, %rax
	je	.L50
	leaq	.LC18(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	jmp	.L51
.L50:
	leaq	.LC19(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
.L51:
	addq	$1, -32(%rbp)
.L48:
	movq	-32(%rbp), %rax
	cmpq	-24(%rbp), %rax
	jb	.L52
	movl	$10, %edi
	call	putchar@PLT
.L47:
	movq	-24(%rbp), %rax
	addq	$2, %rax
	salq	$3, %rax
	addq	%rax, -40(%rbp)
.L40:
	movq	heapBasePtr(%rip), %rax
	movq	initializedSpace(%rip), %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	cmpq	%rax, -40(%rbp)
	jb	.L53
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	dumpHeap, .-dumpHeap
	.globl	memDump
	.type	memDump, @function
memDump:
.LFB14:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	call	dumpGlobalMem
	call	dumpRegisters
	call	dumpHeap
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	memDump, .-memDump
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
