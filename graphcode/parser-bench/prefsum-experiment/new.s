	.file	"new.cpp"
	.text
	.section	.text._ZNKSt5ctypeIcE8do_widenEc,"axG",@progbits,_ZNKSt5ctypeIcE8do_widenEc,comdat
	.align 2
	.p2align 4
	.weak	_ZNKSt5ctypeIcE8do_widenEc
	.type	_ZNKSt5ctypeIcE8do_widenEc, @function
_ZNKSt5ctypeIcE8do_widenEc:
.LFB2009:
	.cfi_startproc
	movl	%esi, %eax
	ret
	.cfi_endproc
.LFE2009:
	.size	_ZNKSt5ctypeIcE8do_widenEc, .-_ZNKSt5ctypeIcE8do_widenEc
	.text
	.align 2
	.p2align 4
	.type	_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE24_M_get_insert_unique_posERS1_.isra.0, @function
_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE24_M_get_insert_unique_posERS1_.isra.0:
.LFB4293:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movl	%esi, %ebp
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	movq	16(%rdi), %rbx
	testq	%rbx, %rbx
	jne	.L4
	jmp	.L17
	.p2align 4,,10
	.p2align 3
.L10:
	movq	%rax, %rbx
.L4:
	movl	32(%rbx), %edx
	movq	24(%rbx), %rax
	cmpl	%ebp, %edx
	cmovg	16(%rbx), %rax
	setg	%cl
	testq	%rax, %rax
	jne	.L10
	movq	%rbx, %rsi
	testb	%cl, %cl
	jne	.L5
.L7:
	xorl	%eax, %eax
	cmpl	%edx, %ebp
	cmovle	%rax, %rsi
	cmovg	%rax, %rbx
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movq	%rbx, %rax
	movq	%rsi, %rdx
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L17:
	.cfi_restore_state
	leaq	8(%rdi), %rbx
.L5:
	cmpq	%rbx, 24(%rdi)
	je	.L12
	movq	%rbx, %rdi
	call	_ZSt18_Rb_tree_decrementPSt18_Rb_tree_node_base@PLT
	movq	%rbx, %rsi
	movl	32(%rax), %edx
	movq	%rax, %rbx
	jmp	.L7
	.p2align 4,,10
	.p2align 3
.L12:
	movq	%rbx, %rsi
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	xorl	%ebx, %ebx
	movq	%rbx, %rax
	movq	%rsi, %rdx
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE4293:
	.size	_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE24_M_get_insert_unique_posERS1_.isra.0, .-_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE24_M_get_insert_unique_posERS1_.isra.0
	.align 2
	.p2align 4
	.type	_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE22_M_emplace_hint_uniqueIJRKSt21piecewise_construct_tSt5tupleIJRS1_EESH_IJEEEEESt17_Rb_tree_iteratorIS6_ESt23_Rb_tree_const_iteratorIS6_EDpOT_.isra.0, @function
_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE22_M_emplace_hint_uniqueIJRKSt21piecewise_construct_tSt5tupleIJRS1_EESH_IJEEEEESt17_Rb_tree_iteratorIS6_ESt23_Rb_tree_const_iteratorIS6_EDpOT_.isra.0:
.LFB4294:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	movq	%rdx, %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	movq	%rdi, %r12
	movl	$64, %edi
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	movq	%rsi, %rbp
	leaq	8(%r12), %r15
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	call	_Znwm@PLT
	pxor	%xmm0, %xmm0
	movq	%rax, %rbx
	movq	0(%r13), %rax
	movl	(%rax), %r14d
	movq	$0, 56(%rbx)
	movups	%xmm0, 40(%rbx)
	movl	%r14d, 32(%rbx)
	cmpq	%rbp, %r15
	je	.L38
	cmpl	%r14d, 32(%rbp)
	jle	.L23
	movq	24(%r12), %rdx
	cmpq	%rbp, %rdx
	je	.L33
	movq	%rbp, %rdi
	call	_ZSt18_Rb_tree_decrementPSt18_Rb_tree_node_base@PLT
	movq	%rax, %rdx
	cmpl	%r14d, 32(%rax)
	jge	.L28
	cmpq	$0, 24(%rax)
	je	.L32
	movq	%rbp, %rdx
	movl	$1, %edi
	jmp	.L24
	.p2align 4,,10
	.p2align 3
.L23:
	movq	%rbp, %r13
	jge	.L26
	cmpq	%rbp, 32(%r12)
	je	.L27
	movq	%rbp, %rdi
	call	_ZSt18_Rb_tree_incrementPSt18_Rb_tree_node_base@PLT
	movq	%rax, %rdx
	cmpl	%r14d, 32(%rax)
	jle	.L28
	cmpq	$0, 24(%rbp)
	movl	$1, %eax
	je	.L27
.L21:
	cmpq	%rdx, %r15
	je	.L33
.L41:
	testb	%al, %al
	je	.L39
.L33:
	movl	$1, %edi
.L24:
	movq	%r15, %rcx
	movq	%rbx, %rsi
	movq	%rbx, %r13
	call	_ZSt29_Rb_tree_insert_and_rebalancebPSt18_Rb_tree_node_baseS0_RS_@PLT
	addq	$1, 40(%r12)
	jmp	.L29
	.p2align 4,,10
	.p2align 3
.L38:
	cmpq	$0, 40(%r12)
	jne	.L40
.L28:
	movl	%r14d, %esi
	movq	%r12, %rdi
	call	_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE24_M_get_insert_unique_posERS1_.isra.0
	testq	%rax, %rax
	movq	%rax, %r13
	setne	%al
	testq	%rdx, %rdx
	jne	.L21
.L26:
	movl	$64, %esi
	movq	%rbx, %rdi
	call	_ZdlPvm@PLT
.L29:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movq	%r13, %rax
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L40:
	.cfi_restore_state
	movq	32(%r12), %rdx
	cmpl	%r14d, 32(%rdx)
	jge	.L28
.L32:
	xorl	%eax, %eax
	cmpq	%rdx, %r15
	jne	.L41
	jmp	.L33
	.p2align 4,,10
	.p2align 3
.L39:
	movq	%rdx, %r13
.L27:
	xorl	%edi, %edi
	cmpl	32(%r13), %r14d
	movq	%r13, %rdx
	setl	%dil
	jmp	.L24
	.cfi_endproc
.LFE4294:
	.size	_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE22_M_emplace_hint_uniqueIJRKSt21piecewise_construct_tSt5tupleIJRS1_EESH_IJEEEEESt17_Rb_tree_iteratorIS6_ESt23_Rb_tree_const_iteratorIS6_EDpOT_.isra.0, .-_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE22_M_emplace_hint_uniqueIJRKSt21piecewise_construct_tSt5tupleIJRS1_EESH_IJEEEEESt17_Rb_tree_iteratorIS6_ESt23_Rb_tree_const_iteratorIS6_EDpOT_.isra.0
	.section	.text._ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE8_M_eraseEPSt13_Rb_tree_nodeIS6_E.isra.0,"axG",@progbits,_ZN5graphD5Ev,comdat
	.align 2
	.p2align 4
	.type	_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE8_M_eraseEPSt13_Rb_tree_nodeIS6_E.isra.0, @function
_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE8_M_eraseEPSt13_Rb_tree_nodeIS6_E.isra.0:
.LFB4302:
	.cfi_startproc
	testq	%rdi, %rdi
	je	.L54
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
.L45:
	movq	24(%rbx), %rdi
	movq	%rbx, %rbp
	call	_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE8_M_eraseEPSt13_Rb_tree_nodeIS6_E.isra.0
	movq	40(%rbp), %rdi
	movq	56(%rbp), %rsi
	movq	16(%rbx), %rbx
	subq	%rdi, %rsi
	testq	%rdi, %rdi
	je	.L44
	call	_ZdlPvm@PLT
.L44:
	movl	$64, %esi
	movq	%rbp, %rdi
	call	_ZdlPvm@PLT
	testq	%rbx, %rbx
	jne	.L45
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L54:
	.cfi_restore 3
	.cfi_restore 6
	ret
	.cfi_endproc
.LFE4302:
	.size	_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE8_M_eraseEPSt13_Rb_tree_nodeIS6_E.isra.0, .-_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE8_M_eraseEPSt13_Rb_tree_nodeIS6_E.isra.0
	.section	.text._ZNSt8_Rb_treeIiSt4pairIKiiESt10_Select1stIS2_ESt4lessIiESaIS2_EE8_M_eraseEPSt13_Rb_tree_nodeIS2_E.isra.0,"axG",@progbits,_ZN5graphD5Ev,comdat
	.align 2
	.p2align 4
	.type	_ZNSt8_Rb_treeIiSt4pairIKiiESt10_Select1stIS2_ESt4lessIiESaIS2_EE8_M_eraseEPSt13_Rb_tree_nodeIS2_E.isra.0, @function
_ZNSt8_Rb_treeIiSt4pairIKiiESt10_Select1stIS2_ESt4lessIiESaIS2_EE8_M_eraseEPSt13_Rb_tree_nodeIS2_E.isra.0:
.LFB4303:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$40, %rsp
	.cfi_def_cfa_offset 96
	movq	%rdi, 16(%rsp)
	testq	%rdi, %rdi
	je	.L57
.L75:
	movq	16(%rsp), %rax
	movq	24(%rax), %rax
	movq	%rax, 8(%rsp)
	testq	%rax, %rax
	je	.L59
.L74:
	movq	8(%rsp), %rax
	movq	24(%rax), %r14
	testq	%r14, %r14
	je	.L60
.L73:
	movq	24(%r14), %r15
	testq	%r15, %r15
	je	.L61
.L72:
	movq	24(%r15), %rbx
	testq	%rbx, %rbx
	je	.L62
.L71:
	movq	24(%rbx), %r12
	testq	%r12, %r12
	je	.L63
.L70:
	movq	24(%r12), %rbp
	testq	%rbp, %rbp
	je	.L64
.L69:
	movq	24(%rbp), %rdx
	testq	%rdx, %rdx
	je	.L65
.L68:
	movq	24(%rdx), %r13
	testq	%r13, %r13
	je	.L66
.L67:
	movq	24(%r13), %rdi
	movq	%rdx, 24(%rsp)
	call	_ZNSt8_Rb_treeIiSt4pairIKiiESt10_Select1stIS2_ESt4lessIiESaIS2_EE8_M_eraseEPSt13_Rb_tree_nodeIS2_E.isra.0
	movq	%r13, %rdi
	movq	16(%r13), %r13
	movl	$40, %esi
	call	_ZdlPvm@PLT
	movq	24(%rsp), %rdx
	testq	%r13, %r13
	jne	.L67
.L66:
	movq	16(%rdx), %r13
	movl	$40, %esi
	movq	%rdx, %rdi
	call	_ZdlPvm@PLT
	testq	%r13, %r13
	je	.L65
	movq	%r13, %rdx
	jmp	.L68
.L63:
	movq	16(%rbx), %rbp
	movl	$40, %esi
	movq	%rbx, %rdi
	call	_ZdlPvm@PLT
	testq	%rbp, %rbp
	je	.L62
	movq	%rbp, %rbx
	jmp	.L71
	.p2align 4,,10
	.p2align 3
.L64:
	movq	16(%r12), %rbp
	movl	$40, %esi
	movq	%r12, %rdi
	call	_ZdlPvm@PLT
	testq	%rbp, %rbp
	je	.L63
	movq	%rbp, %r12
	jmp	.L70
.L62:
	movq	16(%r15), %rbx
	movl	$40, %esi
	movq	%r15, %rdi
	call	_ZdlPvm@PLT
	testq	%rbx, %rbx
	je	.L61
	movq	%rbx, %r15
	jmp	.L72
	.p2align 4,,10
	.p2align 3
.L65:
	movq	16(%rbp), %rdx
	movl	$40, %esi
	movq	%rbp, %rdi
	movq	%rdx, 24(%rsp)
	call	_ZdlPvm@PLT
	movq	24(%rsp), %rdx
	testq	%rdx, %rdx
	je	.L64
	movq	%rdx, %rbp
	jmp	.L69
.L61:
	movq	16(%r14), %rbx
	movl	$40, %esi
	movq	%r14, %rdi
	call	_ZdlPvm@PLT
	testq	%rbx, %rbx
	je	.L60
	movq	%rbx, %r14
	jmp	.L73
.L60:
	movq	8(%rsp), %rdi
	movl	$40, %esi
	movq	16(%rdi), %rbx
	call	_ZdlPvm@PLT
	testq	%rbx, %rbx
	je	.L59
	movq	%rbx, 8(%rsp)
	jmp	.L74
.L59:
	movq	16(%rsp), %rdi
	movl	$40, %esi
	movq	16(%rdi), %rbx
	call	_ZdlPvm@PLT
	testq	%rbx, %rbx
	je	.L57
	movq	%rbx, 16(%rsp)
	jmp	.L75
.L57:
	addq	$40, %rsp
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE4303:
	.size	_ZNSt8_Rb_treeIiSt4pairIKiiESt10_Select1stIS2_ESt4lessIiESaIS2_EE8_M_eraseEPSt13_Rb_tree_nodeIS2_E.isra.0, .-_ZNSt8_Rb_treeIiSt4pairIKiiESt10_Select1stIS2_ESt4lessIiESaIS2_EE8_M_eraseEPSt13_Rb_tree_nodeIS2_E.isra.0
	.section	.text._ZN5graph7prefSumEv,"axG",@progbits,_ZN5graph7prefSumEv,comdat
	.align 2
	.p2align 4
	.weak	_ZN5graph7prefSumEv
	.type	_ZN5graph7prefSumEv, @function
_ZN5graph7prefSumEv:
.LFB3410:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	movabsq	$2305843009213693950, %rbx
	subq	$40, %rsp
	.cfi_def_cfa_offset 80
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	movl	(%rdi), %eax
	addl	$2, %eax
	cltq
	cmpq	%rax, %rbx
	jb	.L114
	movq	%rdi, %rbp
	leaq	0(,%rax,4), %rdi
	call	_Znam@PLT
	movq	%rax, 96(%rbp)
	movslq	4(%rbp), %rax
	cmpq	%rax, %rbx
	jb	.L114
	leaq	0(,%rax,4), %rdi
	call	_Znam@PLT
	movq	%rax, 112(%rbp)
	movslq	4(%rbp), %rax
	cmpq	%rax, %rbx
	jb	.L114
	leaq	0(,%rax,4), %rdi
	call	_Znam@PLT
	movq	%rax, 8(%rbp)
	movl	0(%rbp), %eax
	addl	$2, %eax
	cltq
	cmpq	%rax, %rbx
	jb	.L114
	leaq	0(,%rax,4), %rdi
	call	_Znam@PLT
	movl	$0, 12(%rsp)
	movq	%rax, %r13
	movl	0(%rbp), %eax
	testl	%eax, %eax
	jle	.L113
	movabsq	$-3689348814741910323, %rbx
	xorl	%edx, %edx
	leaq	56(%rbp), %r12
	.p2align 4,,10
	.p2align 3
.L122:
	movq	64(%rbp), %rax
	movq	%r12, %rdi
	testq	%rax, %rax
	jne	.L120
	jmp	.L118
	.p2align 4,,10
	.p2align 3
.L149:
	movq	%rsi, %rax
	testq	%rax, %rax
	je	.L148
.L120:
	movq	16(%rax), %rcx
	movq	24(%rax), %rsi
	cmpl	32(%rax), %edx
	jg	.L149
	movq	%rax, %rdi
	movq	%rcx, %rax
	testq	%rax, %rax
	jne	.L120
.L148:
	cmpq	%rdi, %r12
	je	.L118
	cmpl	32(%rdi), %edx
	jge	.L121
.L118:
	leaq	12(%rsp), %rax
	movq	%rdi, %rsi
	leaq	16(%rsp), %rdx
	movq	%rax, 16(%rsp)
	leaq	48(%rbp), %rax
	movq	%rax, %rdi
	call	_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE22_M_emplace_hint_uniqueIJRKSt21piecewise_construct_tSt5tupleIJRS1_EESH_IJEEEEESt17_Rb_tree_iteratorIS6_ESt23_Rb_tree_const_iteratorIS6_EDpOT_.isra.0
	movq	%rax, %rdi
.L121:
	movq	48(%rdi), %rax
	subq	40(%rdi), %rax
	sarq	$2, %rax
	movslq	12(%rsp), %rcx
	imulq	%rbx, %rax
	movq	%rcx, %rdx
	addl	$1, %edx
	movl	%eax, 0(%r13,%rcx,4)
	movl	0(%rbp), %eax
	movl	%edx, 12(%rsp)
	cmpl	%eax, %edx
	jl	.L122
	testl	%eax, %eax
	jle	.L113
	movq	96(%rbp), %rsi
	movl	$1, %eax
	movl	(%rsi), %edx
	.p2align 4,,10
	.p2align 3
.L125:
	addl	0(%r13,%rax,4), %edx
	movl	%edx, (%rsi,%rax,4)
	movl	0(%rbp), %ecx
	addq	$1, %rax
	cmpl	%eax, %ecx
	jge	.L125
	movl	$1, 12(%rsp)
	testl	%ecx, %ecx
	jle	.L113
	movl	$1, %ecx
	leaq	12(%rsp), %rbx
	.p2align 4,,10
	.p2align 3
.L126:
	movq	64(%rbp), %rdx
	movq	%r12, %rsi
	testq	%rdx, %rdx
	jne	.L129
	jmp	.L127
	.p2align 4,,10
	.p2align 3
.L151:
	movq	%r8, %rdx
	testq	%rdx, %rdx
	je	.L150
.L129:
	movq	16(%rdx), %rdi
	movq	24(%rdx), %r8
	cmpl	32(%rdx), %ecx
	jg	.L151
	movq	%rdx, %rsi
	movq	%rdi, %rdx
	testq	%rdx, %rdx
	jne	.L129
.L150:
	cmpq	%rsi, %r12
	je	.L127
	cmpl	32(%rsi), %ecx
	jge	.L130
.L127:
	leaq	16(%rsp), %rdx
	leaq	48(%rbp), %rdi
	movq	%rbx, 16(%rsp)
	call	_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE22_M_emplace_hint_uniqueIJRKSt21piecewise_construct_tSt5tupleIJRS1_EESH_IJEEEEESt17_Rb_tree_iteratorIS6_ESt23_Rb_tree_const_iteratorIS6_EDpOT_.isra.0
	movq	%rax, %rsi
.L130:
	movq	40(%rsi), %rdx
	movq	48(%rsi), %rcx
	cmpq	%rdx, %rcx
	je	.L133
	movq	112(%rbp), %rsi
	.p2align 4,,10
	.p2align 3
.L132:
	movl	8(%rdx), %eax
	addq	$20, %rdx
	movl	%eax, (%rsi)
	cmpq	%rdx, %rcx
	jne	.L132
.L133:
	movl	12(%rsp), %eax
	leal	1(%rax), %ecx
	movl	%ecx, 12(%rsp)
	cmpl	0(%rbp), %ecx
	jle	.L126
.L113:
	movq	24(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L147
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
.L114:
	.cfi_restore_state
	movq	24(%rsp), %rax
	subq	%fs:40, %rax
	je	.L116
.L147:
	call	__stack_chk_fail@PLT
.L116:
	call	__cxa_throw_bad_array_new_length@PLT
	.cfi_endproc
.LFE3410:
	.size	_ZN5graph7prefSumEv, .-_ZN5graph7prefSumEv
	.section	.text._ZN5graphD2Ev,"axG",@progbits,_ZN5graphD5Ev,comdat
	.align 2
	.p2align 4
	.weak	_ZN5graphD2Ev
	.type	_ZN5graphD2Ev, @function
_ZN5graphD2Ev:
.LFB3413:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	movq	272(%rdi), %rbx
	testq	%rbx, %rbx
	je	.L153
.L154:
	movq	24(%rbx), %rdi
	call	_ZNSt8_Rb_treeIiSt4pairIKiiESt10_Select1stIS2_ESt4lessIiESaIS2_EE8_M_eraseEPSt13_Rb_tree_nodeIS2_E.isra.0
	movq	%rbx, %rdi
	movq	16(%rbx), %rbx
	movl	$40, %esi
	call	_ZdlPvm@PLT
	testq	%rbx, %rbx
	jne	.L154
.L153:
	movq	224(%rbp), %rbx
	testq	%rbx, %rbx
	je	.L155
.L156:
	movq	24(%rbx), %rdi
	call	_ZNSt8_Rb_treeIiSt4pairIKiiESt10_Select1stIS2_ESt4lessIiESaIS2_EE8_M_eraseEPSt13_Rb_tree_nodeIS2_E.isra.0
	movq	%rbx, %rdi
	movq	16(%rbx), %rbx
	movl	$40, %esi
	call	_ZdlPvm@PLT
	testq	%rbx, %rbx
	jne	.L156
.L155:
	movq	128(%rbp), %rdi
	testq	%rdi, %rdi
	je	.L157
	movq	144(%rbp), %rsi
	subq	%rdi, %rsi
	call	_ZdlPvm@PLT
.L157:
	movq	64(%rbp), %rdi
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	jmp	_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE8_M_eraseEPSt13_Rb_tree_nodeIS6_E.isra.0
	.cfi_endproc
.LFE3413:
	.size	_ZN5graphD2Ev, .-_ZN5graphD2Ev
	.weak	_ZN5graphD1Ev
	.set	_ZN5graphD1Ev,_ZN5graphD2Ev
	.section	.rodata._ZNSt6vectorI4edgeSaIS0_EE17_M_realloc_insertIJRKS0_EEEvN9__gnu_cxx17__normal_iteratorIPS0_S2_EEDpOT_.str1.1,"aMS",@progbits,1
.LC0:
	.string	"vector::_M_realloc_insert"
	.section	.text._ZNSt6vectorI4edgeSaIS0_EE17_M_realloc_insertIJRKS0_EEEvN9__gnu_cxx17__normal_iteratorIPS0_S2_EEDpOT_,"axG",@progbits,_ZNSt6vectorI4edgeSaIS0_EE17_M_realloc_insertIJRKS0_EEEvN9__gnu_cxx17__normal_iteratorIPS0_S2_EEDpOT_,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt6vectorI4edgeSaIS0_EE17_M_realloc_insertIJRKS0_EEEvN9__gnu_cxx17__normal_iteratorIPS0_S2_EEDpOT_
	.type	_ZNSt6vectorI4edgeSaIS0_EE17_M_realloc_insertIJRKS0_EEEvN9__gnu_cxx17__normal_iteratorIPS0_S2_EEDpOT_, @function
_ZNSt6vectorI4edgeSaIS0_EE17_M_realloc_insertIJRKS0_EEEvN9__gnu_cxx17__normal_iteratorIPS0_S2_EEDpOT_:
.LFB3947:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	movq	%rdx, %r15
	movabsq	$-3689348814741910323, %rdx
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$24, %rsp
	.cfi_def_cfa_offset 80
	movq	8(%rdi), %rbp
	movq	(%rdi), %r12
	movq	%rbp, %rax
	subq	%r12, %rax
	sarq	$2, %rax
	imulq	%rdx, %rax
	movabsq	$461168601842738790, %rdx
	cmpq	%rdx, %rax
	je	.L192
	movq	%rsi, %rdx
	movq	%rdi, %rbx
	movq	%rsi, %r13
	subq	%r12, %rdx
	cmpq	%rbp, %r12
	je	.L193
	leaq	(%rax,%rax), %rcx
	cmpq	%rax, %rcx
	jb	.L185
	testq	%rcx, %rcx
	jne	.L194
	xorl	%r14d, %r14d
	xorl	%ecx, %ecx
.L176:
	movdqu	(%r15), %xmm0
	movl	16(%r15), %eax
	leaq	20(%rcx,%rdx), %r8
	subq	%r13, %rbp
	leaq	(%r8,%rbp), %r15
	movl	%eax, 16(%rcx,%rdx)
	movups	%xmm0, (%rcx,%rdx)
	testq	%rdx, %rdx
	jg	.L195
	testq	%rbp, %rbp
	jle	.L180
	movq	%rbp, %rdx
	movq	%r13, %rsi
	movq	%r8, %rdi
	movq	%rcx, (%rsp)
	call	memcpy@PLT
	movq	(%rsp), %rcx
.L180:
	testq	%r12, %r12
	jne	.L179
.L182:
	movq	%rcx, (%rbx)
	movq	%r15, 8(%rbx)
	movq	%r14, 16(%rbx)
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L185:
	.cfi_restore_state
	movabsq	$9223372036854775800, %r14
.L175:
	movq	%r14, %rdi
	movq	%rdx, (%rsp)
	call	_Znwm@PLT
	movq	(%rsp), %rdx
	movq	%rax, %rcx
	addq	%rax, %r14
	jmp	.L176
	.p2align 4,,10
	.p2align 3
.L195:
	movq	%rcx, %rdi
	movq	%r12, %rsi
	movq	%r8, (%rsp)
	call	memmove@PLT
	movq	%rax, %rcx
	testq	%rbp, %rbp
	jg	.L196
.L179:
	movq	16(%rbx), %rsi
	movq	%r12, %rdi
	movq	%rcx, (%rsp)
	subq	%r12, %rsi
	call	_ZdlPvm@PLT
	movq	(%rsp), %rcx
	jmp	.L182
	.p2align 4,,10
	.p2align 3
.L193:
	addq	$1, %rax
	jc	.L185
	movabsq	$461168601842738790, %rcx
	cmpq	%rcx, %rax
	cmova	%rcx, %rax
	leaq	(%rax,%rax,4), %r14
	salq	$2, %r14
	jmp	.L175
	.p2align 4,,10
	.p2align 3
.L196:
	movq	(%rsp), %rdi
	movq	%rbp, %rdx
	movq	%r13, %rsi
	movq	%rax, 8(%rsp)
	call	memcpy@PLT
	movq	8(%rsp), %rcx
	jmp	.L179
.L194:
	movabsq	$461168601842738790, %rax
	cmpq	%rax, %rcx
	cmova	%rax, %rcx
	leaq	(%rcx,%rcx,4), %r14
	salq	$2, %r14
	jmp	.L175
.L192:
	leaq	.LC0(%rip), %rdi
	call	_ZSt20__throw_length_errorPKc@PLT
	.cfi_endproc
.LFE3947:
	.size	_ZNSt6vectorI4edgeSaIS0_EE17_M_realloc_insertIJRKS0_EEEvN9__gnu_cxx17__normal_iteratorIPS0_S2_EEDpOT_, .-_ZNSt6vectorI4edgeSaIS0_EE17_M_realloc_insertIJRKS0_EEEvN9__gnu_cxx17__normal_iteratorIPS0_S2_EEDpOT_
	.section	.rodata._ZN5graph10parseEdgesEv.str1.8,"aMS",@progbits,1
	.align 8
.LC1:
	.string	"basic_string: construction from null is not valid"
	.section	.text._ZN5graph10parseEdgesEv,"axG",@progbits,_ZN5graph10parseEdgesEv,comdat
	.align 2
	.p2align 4
	.weak	_ZN5graph10parseEdgesEv
	.type	_ZN5graph10parseEdgesEv, @function
_ZN5graph10parseEdgesEv:
.LFB3408:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA3408
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movq	%rdi, %rbx
	subq	$1128, %rsp
	.cfi_def_cfa_offset 1184
	leaq	848(%rsp), %r15
	leaq	592(%rsp), %r12
	movq	%fs:40, %rax
	movq	%rax, 1112(%rsp)
	xorl	%eax, %eax
	movq	%r15, %rdi
	movq	%r15, 80(%rsp)
	call	_ZNSt8ios_baseC2Ev@PLT
	leaq	16+_ZTVSt9basic_iosIcSt11char_traitsIcEE(%rip), %rax
	xorl	%esi, %esi
	pxor	%xmm0, %xmm0
	movq	%rax, 848(%rsp)
	movq	8+_ZTTSt14basic_ifstreamIcSt11char_traitsIcEE(%rip), %rax
	movw	%si, 1072(%rsp)
	movq	16+_ZTTSt14basic_ifstreamIcSt11char_traitsIcEE(%rip), %rcx
	xorl	%esi, %esi
	movups	%xmm0, 1080(%rsp)
	movups	%xmm0, 1096(%rsp)
	movq	%rax, 592(%rsp)
	movq	-24(%rax), %rax
	movq	$0, 1064(%rsp)
	movq	%rcx, 592(%rsp,%rax)
	movq	8+_ZTTSt14basic_ifstreamIcSt11char_traitsIcEE(%rip), %rax
	movq	$0, 600(%rsp)
	movq	-24(%rax), %rdi
	addq	%r12, %rdi
.LEHB0:
	call	_ZNSt9basic_iosIcSt11char_traitsIcEE4initEPSt15basic_streambufIcS1_E@PLT
.LEHE0:
	leaq	24+_ZTVSt14basic_ifstreamIcSt11char_traitsIcEE(%rip), %rax
	movq	%rax, 592(%rsp)
	addq	$40, %rax
	movq	%rax, 848(%rsp)
	leaq	608(%rsp), %rax
	movq	%rax, %rdi
	movq	%rax, 32(%rsp)
	movq	%rax, %r14
.LEHB1:
	call	_ZNSt13basic_filebufIcSt11char_traitsIcEEC1Ev@PLT
.LEHE1:
	movq	%r14, %rsi
	movq	%r15, %rdi
.LEHB2:
	call	_ZNSt9basic_iosIcSt11char_traitsIcEE4initEPSt15basic_streambufIcS1_E@PLT
.LEHE2:
	movq	40(%rbx), %rsi
	movq	32(%rsp), %rdi
	movl	$8, %edx
.LEHB3:
	call	_ZNSt13basic_filebufIcSt11char_traitsIcEE4openEPKcSt13_Ios_Openmode@PLT
	movq	592(%rsp), %rdx
	movq	-24(%rdx), %rdi
	addq	%r12, %rdi
	testq	%rax, %rax
	je	.L283
	xorl	%esi, %esi
	call	_ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate@PLT
.LEHE3:
.L204:
	leaq	176(%rsp), %rax
	movb	$0, 176(%rsp)
	movq	%rax, 88(%rsp)
	movq	%rax, 160(%rsp)
	movq	592(%rsp), %rax
	movq	$0, 168(%rsp)
	movq	-24(%rax), %rax
	movq	832(%rsp,%rax), %rbp
	testq	%rbp, %rbp
	je	.L238
	movq	.LC2(%rip), %xmm2
	leaq	160(%rsp), %rax
	movq	16+_ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %r13
	movq	%rax, 8(%rsp)
	movhps	.LC3(%rip), %xmm2
	movaps	%xmm2, 64(%rsp)
	jmp	.L205
	.p2align 4,,10
	.p2align 3
.L286:
	movsbl	67(%rbp), %edx
.L240:
	movq	8(%rsp), %rsi
	movq	%r12, %rdi
.LEHB4:
	call	_ZSt7getlineIcSt11char_traitsIcESaIcEERSt13basic_istreamIT_T0_ES7_RNSt7__cxx1112basic_stringIS4_S5_T1_EES4_@PLT
	movq	(%rax), %rdx
	movq	-24(%rdx), %rdx
	testb	$5, 32(%rax,%rdx)
	jne	.L284
	cmpq	$0, 168(%rsp)
	je	.L207
	movq	160(%rsp), %rax
	movzbl	(%rax), %eax
	subl	$48, %eax
	cmpb	$9, %al
	jbe	.L285
.L207:
	movq	592(%rsp), %rax
	movq	-24(%rax), %rax
	movq	832(%rsp,%rax), %rbp
	testq	%rbp, %rbp
	je	.L238
.L205:
	cmpb	$0, 56(%rbp)
	jne	.L286
	movq	%rbp, %rdi
	call	_ZNKSt5ctypeIcE13_M_widen_initEv@PLT
	movq	0(%rbp), %rax
	leaq	_ZNKSt5ctypeIcE8do_widenEc(%rip), %rcx
	movl	$10, %edx
	movq	48(%rax), %rax
	cmpq	%rcx, %rax
	je	.L240
	movl	$10, %esi
	movq	%rbp, %rdi
	call	*%rax
.LEHE4:
	movsbl	%al, %edx
	jmp	.L240
	.p2align 4,,10
	.p2align 3
.L285:
	leaq	320(%rsp), %rax
	leaq	192(%rsp), %r14
	movq	%rax, %rdi
	movq	%rax, 16(%rsp)
	call	_ZNSt8ios_baseC2Ev@PLT
	leaq	16+_ZTVSt9basic_iosIcSt11char_traitsIcEE(%rip), %rax
	xorl	%edx, %edx
	xorl	%esi, %esi
	pxor	%xmm0, %xmm0
	movw	%dx, 544(%rsp)
	movq	24+_ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %rcx
	movups	%xmm0, 552(%rsp)
	movups	%xmm0, 568(%rsp)
	movq	%rax, 320(%rsp)
	movq	-24(%r13), %rax
	movq	$0, 536(%rsp)
	movq	%r13, 192(%rsp)
	movq	%rcx, 192(%rsp,%rax)
	movq	$0, 200(%rsp)
	movq	-24(%r13), %rdi
	addq	%r14, %rdi
.LEHB5:
	call	_ZNSt9basic_iosIcSt11char_traitsIcEE4initEPSt15basic_streambufIcS1_E@PLT
.LEHE5:
	movq	32+_ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %r15
	xorl	%esi, %esi
	movq	-24(%r15), %rax
	movq	%r15, 208(%rsp)
	leaq	208(%rsp,%rax), %rdi
	movq	40+_ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %rax
	movq	%rax, (%rdi)
.LEHB6:
	call	_ZNSt9basic_iosIcSt11char_traitsIcEE4initEPSt15basic_streambufIcS1_E@PLT
.LEHE6:
	movq	8+_ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %rax
	movq	48+_ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %rcx
	pxor	%xmm0, %xmm0
	movdqa	64(%rsp), %xmm3
	movq	-24(%rax), %rax
	movq	%rcx, 192(%rsp,%rax)
	leaq	24+_ZTVNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %rax
	movq	%rax, 192(%rsp)
	addq	$80, %rax
	movq	%rax, 320(%rsp)
	leaq	272(%rsp), %rax
	movq	%rax, %rdi
	movq	%rax, 40(%rsp)
	movaps	%xmm3, 208(%rsp)
	movaps	%xmm0, 224(%rsp)
	movaps	%xmm0, 240(%rsp)
	movaps	%xmm0, 256(%rsp)
	call	_ZNSt6localeC1Ev@PLT
	leaq	16+_ZTVNSt7__cxx1115basic_stringbufIcSt11char_traitsIcESaIcEEE(%rip), %rax
	leaq	304(%rsp), %rcx
	movl	$0, 280(%rsp)
	movq	%rax, 216(%rsp)
	movq	160(%rsp), %rax
	movq	%rcx, 24(%rsp)
	movq	168(%rsp), %rbp
	movq	%rax, 48(%rsp)
	movq	%rcx, 288(%rsp)
	testq	%rax, %rax
	jne	.L210
	testq	%rbp, %rbp
	jne	.L287
.L210:
	movq	%rbp, 128(%rsp)
	cmpq	$15, %rbp
	ja	.L288
	cmpq	$1, %rbp
	jne	.L216
	movq	48(%rsp), %rax
	movzbl	(%rax), %eax
	movb	%al, 304(%rsp)
.L218:
	movq	24(%rsp), %rax
.L217:
	movq	%rbp, 296(%rsp)
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movb	$0, (%rax,%rbp)
	leaq	216(%rsp), %rbp
	movq	288(%rsp), %rsi
	movl	$24, 280(%rsp)
	movq	%rbp, %rdi
.LEHB7:
	call	_ZNSt7__cxx1115basic_stringbufIcSt11char_traitsIcESaIcEE7_M_syncEPcmm@PLT
.LEHE7:
	movq	16(%rsp), %rdi
	movq	%rbp, %rsi
.LEHB8:
	call	_ZNSt9basic_iosIcSt11char_traitsIcEE4initEPSt15basic_streambufIcS1_E@PLT
.LEHE8:
	addl	$1, 4(%rbx)
	leaq	108(%rsp), %rbp
	movq	%r14, %rdi
	movq	%rbp, %rsi
.LEHB9:
	call	_ZNSirsERi@PLT
	movl	108(%rsp), %eax
	cmpl	%eax, (%rbx)
	jge	.L227
	movl	%eax, (%rbx)
.L227:
	leaq	112(%rsp), %rsi
	movq	%r14, %rdi
	call	_ZNSirsERi@PLT
	movl	112(%rsp), %eax
	cmpl	%eax, (%rbx)
	jge	.L228
	movl	%eax, (%rbx)
.L228:
	movl	108(%rsp), %edi
	movd	%eax, %xmm4
	movq	64(%rbx), %rax
	leaq	56(%rbx), %rcx
	movl	$1, 136(%rsp)
	movq	%rcx, %rsi
	movd	%edi, %xmm0
	punpckldq	%xmm4, %xmm0
	movq	%xmm0, 128(%rsp)
	testq	%rax, %rax
	jne	.L231
	jmp	.L229
	.p2align 4,,10
	.p2align 3
.L290:
	movq	%r8, %rax
	testq	%rax, %rax
	je	.L289
.L231:
	movq	16(%rax), %rdx
	movq	24(%rax), %r8
	cmpl	32(%rax), %edi
	jg	.L290
	movq	%rax, %rsi
	movq	%rdx, %rax
	testq	%rax, %rax
	jne	.L231
.L289:
	cmpq	%rcx, %rsi
	je	.L229
	cmpl	32(%rsi), %edi
	jge	.L232
.L229:
	leaq	120(%rsp), %rdx
	leaq	48(%rbx), %rdi
	movq	%rbp, 120(%rsp)
	call	_ZNSt8_Rb_treeIiSt4pairIKiSt6vectorI4edgeSaIS3_EEESt10_Select1stIS6_ESt4lessIiESaIS6_EE22_M_emplace_hint_uniqueIJRKSt21piecewise_construct_tSt5tupleIJRS1_EESH_IJEEEEESt17_Rb_tree_iteratorIS6_ESt23_Rb_tree_const_iteratorIS6_EDpOT_.isra.0
	movq	%rax, %rsi
.L232:
	movq	48(%rsi), %rax
	cmpq	56(%rsi), %rax
	je	.L233
	movdqa	128(%rsp), %xmm5
	addq	$20, %rax
	movups	%xmm5, -20(%rax)
	movl	144(%rsp), %edx
	movl	%edx, -4(%rax)
	movq	%rax, 48(%rsi)
	movq	136(%rbx), %rsi
	cmpq	144(%rbx), %rsi
	je	.L235
.L292:
	movdqa	128(%rsp), %xmm6
	addq	$20, %rsi
	movups	%xmm6, -20(%rsi)
	movl	144(%rsp), %eax
	movl	%eax, -4(%rsi)
	movq	%rsi, 136(%rbx)
.L236:
	movq	.LC2(%rip), %xmm1
	leaq	116(%rsp), %rsi
	movq	%r14, %rdi
	movhps	.LC4(%rip), %xmm1
	movaps	%xmm1, 48(%rsp)
	call	_ZNSirsERi@PLT
.LEHE9:
	leaq	24+_ZTVNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %rax
	movdqa	48(%rsp), %xmm1
	movq	288(%rsp), %rdi
	movq	%rax, 192(%rsp)
	addq	$80, %rax
	movq	%rax, 320(%rsp)
	movq	24(%rsp), %rax
	movaps	%xmm1, 208(%rsp)
	cmpq	%rax, %rdi
	je	.L237
	movq	304(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
.L237:
	movq	40(%rsp), %rdi
	leaq	16+_ZTVSt15basic_streambufIcSt11char_traitsIcEE(%rip), %rax
	movq	%rax, 216(%rsp)
	call	_ZNSt6localeD1Ev@PLT
	movq	8+_ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %rax
	movq	16(%rsp), %rdi
	movq	48+_ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %rcx
	movq	-24(%rax), %rax
	movq	%rcx, 192(%rsp,%rax)
	movq	-24(%r15), %rax
	movq	40+_ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %rcx
	movq	%r15, 208(%rsp)
	movq	%rcx, 208(%rsp,%rax)
	movq	-24(%r13), %rax
	movq	24+_ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %rcx
	movq	%r13, 192(%rsp)
	movq	%rcx, 192(%rsp,%rax)
	leaq	16+_ZTVSt9basic_iosIcSt11char_traitsIcEE(%rip), %rax
	movq	$0, 200(%rsp)
	movq	%rax, 320(%rsp)
	call	_ZNSt8ios_baseD2Ev@PLT
	jmp	.L207
	.p2align 4,,10
	.p2align 3
.L284:
	movq	32(%rsp), %rdi
.LEHB10:
	call	_ZNSt13basic_filebufIcSt11char_traitsIcEE5closeEv@PLT
.LEHE10:
	testq	%rax, %rax
	je	.L291
.L242:
	movq	160(%rsp), %rdi
	movq	88(%rsp), %rax
	cmpq	%rax, %rdi
	je	.L243
	movq	176(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
.L243:
	leaq	24+_ZTVSt14basic_ifstreamIcSt11char_traitsIcEE(%rip), %rax
	movq	32(%rsp), %rdi
	movq	%rax, 592(%rsp)
	addq	$40, %rax
	movq	%rax, 848(%rsp)
	leaq	16+_ZTVSt13basic_filebufIcSt11char_traitsIcEE(%rip), %rax
	movq	%rax, 608(%rsp)
.LEHB11:
	call	_ZNSt13basic_filebufIcSt11char_traitsIcEE5closeEv@PLT
.LEHE11:
.L245:
	leaq	712(%rsp), %rdi
	call	_ZNSt12__basic_fileIcED1Ev@PLT
	leaq	16+_ZTVSt15basic_streambufIcSt11char_traitsIcEE(%rip), %rax
	leaq	664(%rsp), %rdi
	movq	%rax, 608(%rsp)
	call	_ZNSt6localeD1Ev@PLT
	movq	8+_ZTTSt14basic_ifstreamIcSt11char_traitsIcEE(%rip), %rax
	movq	80(%rsp), %rdi
	movq	16+_ZTTSt14basic_ifstreamIcSt11char_traitsIcEE(%rip), %rbx
	movq	%rax, 592(%rsp)
	movq	-24(%rax), %rax
	movq	%rbx, 592(%rsp,%rax)
	leaq	16+_ZTVSt9basic_iosIcSt11char_traitsIcEE(%rip), %rax
	movq	%rax, 848(%rsp)
	movq	$0, 600(%rsp)
	call	_ZNSt8ios_baseD2Ev@PLT
	movq	1112(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L280
	addq	$1128, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L216:
	.cfi_restore_state
	testq	%rbp, %rbp
	je	.L218
	movq	24(%rsp), %rdi
	jmp	.L215
	.p2align 4,,10
	.p2align 3
.L288:
	leaq	128(%rsp), %rsi
	leaq	288(%rsp), %rdi
	xorl	%edx, %edx
.LEHB12:
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERmm@PLT
.LEHE12:
	movq	%rax, 288(%rsp)
	movq	%rax, %rdi
	movq	128(%rsp), %rax
	movq	%rax, 304(%rsp)
.L215:
	movq	48(%rsp), %rsi
	movq	%rbp, %rdx
	call	memcpy@PLT
	movq	128(%rsp), %rbp
	movq	288(%rsp), %rax
	jmp	.L217
	.p2align 4,,10
	.p2align 3
.L233:
	leaq	40(%rsi), %rdi
	leaq	128(%rsp), %rdx
	movq	%rax, %rsi
.LEHB13:
	call	_ZNSt6vectorI4edgeSaIS0_EE17_M_realloc_insertIJRKS0_EEEvN9__gnu_cxx17__normal_iteratorIPS0_S2_EEDpOT_
	movq	136(%rbx), %rsi
	cmpq	144(%rbx), %rsi
	jne	.L292
	.p2align 4,,10
	.p2align 3
.L235:
	leaq	128(%rsp), %rdx
	leaq	128(%rbx), %rdi
	call	_ZNSt6vectorI4edgeSaIS0_EE17_M_realloc_insertIJRKS0_EEEvN9__gnu_cxx17__normal_iteratorIPS0_S2_EEDpOT_
.LEHE13:
	jmp	.L236
	.p2align 4,,10
	.p2align 3
.L283:
	movl	32(%rdi), %esi
	orl	$4, %esi
.LEHB14:
	call	_ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate@PLT
.LEHE14:
	jmp	.L204
	.p2align 4,,10
	.p2align 3
.L291:
	movq	592(%rsp), %rax
	movq	-24(%rax), %rdi
	addq	%r12, %rdi
	movl	32(%rdi), %esi
	orl	$4, %esi
.LEHB15:
	call	_ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate@PLT
	jmp	.L242
.L238:
	movq	1112(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L280
	leaq	160(%rsp), %rax
	movq	%rax, 8(%rsp)
	call	_ZSt16__throw_bad_castv@PLT
.LEHE15:
.L287:
	movq	1112(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L280
	leaq	.LC1(%rip), %rdi
.LEHB16:
	call	_ZSt19__throw_logic_errorPKc@PLT
.LEHE16:
.L224:
	leaq	16+_ZTVNSt7__cxx1115basic_stringbufIcSt11char_traitsIcESaIcEEE(%rip), %rax
	movq	288(%rsp), %rdi
	movq	%rax, 216(%rsp)
	movq	24(%rsp), %rax
	cmpq	%rax, %rdi
	je	.L225
	movq	304(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
.L225:
	movq	40(%rsp), %rdi
	leaq	16+_ZTVSt15basic_streambufIcSt11char_traitsIcEE(%rip), %rax
	movq	%rax, 216(%rsp)
	call	_ZNSt6localeD1Ev@PLT
	leaq	8+_ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %rsi
	movq	%r14, %rdi
	call	_ZNSdD2Ev@PLT
.L212:
	movq	16(%rsp), %rdi
	leaq	16+_ZTVSt9basic_iosIcSt11char_traitsIcEE(%rip), %rax
	movq	%rax, 320(%rsp)
	call	_ZNSt8ios_baseD2Ev@PLT
.L226:
	movq	8(%rsp), %rdi
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv@PLT
.L247:
	movq	%r12, %rdi
	call	_ZNSt14basic_ifstreamIcSt11char_traitsIcEED1Ev@PLT
	movq	1112(%rsp), %rax
	subq	%fs:40, %rax
	je	.L248
.L280:
	call	__stack_chk_fail@PLT
.L256:
	movq	%rax, %rbx
	jmp	.L226
.L261:
	movq	%rax, %rbx
	jmp	.L224
.L199:
	movq	32(%rsp), %rdi
	call	_ZNSt13basic_filebufIcSt11char_traitsIcEED1Ev@PLT
.L200:
	movq	8+_ZTTSt14basic_ifstreamIcSt11char_traitsIcEE(%rip), %rax
	movq	16+_ZTTSt14basic_ifstreamIcSt11char_traitsIcEE(%rip), %rcx
	movq	%rax, 592(%rsp)
	movq	-24(%rax), %rax
	movq	%rcx, 592(%rsp,%rax)
	xorl	%ecx, %ecx
	movq	%rcx, 600(%rsp)
.L201:
	movq	80(%rsp), %rdi
	leaq	16+_ZTVSt9basic_iosIcSt11char_traitsIcEE(%rip), %rax
	movq	%rax, 848(%rsp)
	call	_ZNSt8ios_baseD2Ev@PLT
	movq	1112(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L280
.L248:
	movq	%rbx, %rdi
.LEHB17:
	call	_Unwind_Resume@PLT
.LEHE17:
.L264:
	movq	%rax, %rbx
	jmp	.L220
.L254:
	movq	%rax, %rbx
	jmp	.L247
.L220:
	leaq	288(%rsp), %rdi
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv@PLT
	jmp	.L225
.L262:
	movq	%rax, %rbx
	jmp	.L211
.L260:
	movq	%rax, %rbx
	jmp	.L212
.L211:
	movq	-24(%r13), %rax
	movq	24+_ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE(%rip), %rcx
	movq	%r13, 192(%rsp)
	movq	%rcx, 192(%rsp,%rax)
	xorl	%eax, %eax
	movq	%rax, 200(%rsp)
	jmp	.L212
.L263:
	movq	%rax, %rbx
	jmp	.L221
.L257:
	movq	%rax, %rbx
	jmp	.L201
.L221:
	jmp	.L225
.L259:
	movq	%rax, %rbx
	jmp	.L199
.L255:
	movq	%rax, %rbx
.L246:
	movq	%r14, %rdi
	call	_ZNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEED1Ev@PLT
	jmp	.L226
.L265:
	movq	%rax, %rdi
	jmp	.L244
.L258:
	movq	%rax, %rbx
	jmp	.L200
.L244:
	call	__cxa_begin_catch@PLT
	call	__cxa_end_catch@PLT
	jmp	.L245
	.cfi_endproc
.LFE3408:
	.globl	__gxx_personality_v0
	.section	.gcc_except_table._ZN5graph10parseEdgesEv,"aG",@progbits,_ZN5graph10parseEdgesEv,comdat
	.align 4
.LLSDA3408:
	.byte	0xff
	.byte	0x9b
	.uleb128 .LLSDATT3408-.LLSDATTD3408
.LLSDATTD3408:
	.byte	0x1
	.uleb128 .LLSDACSE3408-.LLSDACSB3408
.LLSDACSB3408:
	.uleb128 .LEHB0-.LFB3408
	.uleb128 .LEHE0-.LEHB0
	.uleb128 .L257-.LFB3408
	.uleb128 0
	.uleb128 .LEHB1-.LFB3408
	.uleb128 .LEHE1-.LEHB1
	.uleb128 .L258-.LFB3408
	.uleb128 0
	.uleb128 .LEHB2-.LFB3408
	.uleb128 .LEHE2-.LEHB2
	.uleb128 .L259-.LFB3408
	.uleb128 0
	.uleb128 .LEHB3-.LFB3408
	.uleb128 .LEHE3-.LEHB3
	.uleb128 .L254-.LFB3408
	.uleb128 0
	.uleb128 .LEHB4-.LFB3408
	.uleb128 .LEHE4-.LEHB4
	.uleb128 .L256-.LFB3408
	.uleb128 0
	.uleb128 .LEHB5-.LFB3408
	.uleb128 .LEHE5-.LEHB5
	.uleb128 .L260-.LFB3408
	.uleb128 0
	.uleb128 .LEHB6-.LFB3408
	.uleb128 .LEHE6-.LEHB6
	.uleb128 .L262-.LFB3408
	.uleb128 0
	.uleb128 .LEHB7-.LFB3408
	.uleb128 .LEHE7-.LEHB7
	.uleb128 .L264-.LFB3408
	.uleb128 0
	.uleb128 .LEHB8-.LFB3408
	.uleb128 .LEHE8-.LEHB8
	.uleb128 .L261-.LFB3408
	.uleb128 0
	.uleb128 .LEHB9-.LFB3408
	.uleb128 .LEHE9-.LEHB9
	.uleb128 .L255-.LFB3408
	.uleb128 0
	.uleb128 .LEHB10-.LFB3408
	.uleb128 .LEHE10-.LEHB10
	.uleb128 .L256-.LFB3408
	.uleb128 0
	.uleb128 .LEHB11-.LFB3408
	.uleb128 .LEHE11-.LEHB11
	.uleb128 .L265-.LFB3408
	.uleb128 0x1
	.uleb128 .LEHB12-.LFB3408
	.uleb128 .LEHE12-.LEHB12
	.uleb128 .L263-.LFB3408
	.uleb128 0
	.uleb128 .LEHB13-.LFB3408
	.uleb128 .LEHE13-.LEHB13
	.uleb128 .L255-.LFB3408
	.uleb128 0
	.uleb128 .LEHB14-.LFB3408
	.uleb128 .LEHE14-.LEHB14
	.uleb128 .L254-.LFB3408
	.uleb128 0
	.uleb128 .LEHB15-.LFB3408
	.uleb128 .LEHE15-.LEHB15
	.uleb128 .L256-.LFB3408
	.uleb128 0
	.uleb128 .LEHB16-.LFB3408
	.uleb128 .LEHE16-.LEHB16
	.uleb128 .L263-.LFB3408
	.uleb128 0
	.uleb128 .LEHB17-.LFB3408
	.uleb128 .LEHE17-.LEHB17
	.uleb128 0
	.uleb128 0
.LLSDACSE3408:
	.byte	0x1
	.byte	0
	.align 4
	.long	0

.LLSDATT3408:
	.section	.text._ZN5graph10parseEdgesEv,"axG",@progbits,_ZN5graph10parseEdgesEv,comdat
	.size	_ZN5graph10parseEdgesEv, .-_ZN5graph10parseEdgesEv
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC5:
	.string	"../inp.txt"
.LC6:
	.string	"done"
	.section	.text.unlikely,"ax",@progbits
.LCOLDB7:
	.section	.text.startup,"ax",@progbits
.LHOTB7:
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB3411:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA3411
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pxor	%xmm0, %xmm0
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$328, %rsp
	.cfi_def_cfa_offset 352
	movq	%fs:40, %rax
	movq	%rax, 312(%rsp)
	xorl	%eax, %eax
	leaq	56(%rsp), %rax
	movq	%rsp, %rbx
	movl	$0, 56(%rsp)
	movq	%rax, 72(%rsp)
	movq	%rbx, %rdi
	movq	%rax, 80(%rsp)
	leaq	216(%rsp), %rax
	movq	%rax, 232(%rsp)
	movq	%rax, 240(%rsp)
	leaq	264(%rsp), %rax
	movq	%rax, 280(%rsp)
	movq	%rax, 288(%rsp)
	leaq	.LC5(%rip), %rax
	movq	$0, 64(%rsp)
	movq	$0, 88(%rsp)
	movl	$0, 216(%rsp)
	movq	$0, 224(%rsp)
	movq	$0, 248(%rsp)
	movl	$0, 264(%rsp)
	movq	$0, 272(%rsp)
	movq	$0, 296(%rsp)
	movq	%rax, 40(%rsp)
	movq	$0, (%rsp)
	movq	$0, 176(%rsp)
	movq	$0, 24(%rsp)
	movaps	%xmm0, 128(%rsp)
	movaps	%xmm0, 144(%rsp)
	movaps	%xmm0, 160(%rsp)
.LEHB18:
	call	_ZN5graph10parseEdgesEv
	movq	%rbx, %rdi
	call	_ZN5graph7prefSumEv
	leaq	.LC6(%rip), %rdi
	call	puts@PLT
.LEHE18:
	movq	%rbx, %rdi
	call	_ZN5graphD1Ev
	movq	312(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L300
	addq	$328, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L300:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
.L297:
	movq	%rax, %rbp
	jmp	.L294
	.section	.gcc_except_table,"a",@progbits
.LLSDA3411:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE3411-.LLSDACSB3411
.LLSDACSB3411:
	.uleb128 .LEHB18-.LFB3411
	.uleb128 .LEHE18-.LEHB18
	.uleb128 .L297-.LFB3411
	.uleb128 0
.LLSDACSE3411:
	.section	.text.startup
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDAC3411
	.type	main.cold, @function
main.cold:
.LFSB3411:
.L294:
	.cfi_def_cfa_offset 352
	.cfi_offset 3, -24
	.cfi_offset 6, -16
	movq	%rbx, %rdi
	call	_ZN5graphD1Ev
	movq	312(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L301
	movq	%rbp, %rdi
.LEHB19:
	call	_Unwind_Resume@PLT
.LEHE19:
.L301:
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE3411:
	.section	.gcc_except_table
.LLSDAC3411:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC3411-.LLSDACSBC3411
.LLSDACSBC3411:
	.uleb128 .LEHB19-.LCOLDB7
	.uleb128 .LEHE19-.LEHB19
	.uleb128 0
	.uleb128 0
.LLSDACSEC3411:
	.section	.text.unlikely
	.section	.text.startup
	.size	main, .-main
	.section	.text.unlikely
	.size	main.cold, .-main.cold
.LCOLDE7:
	.section	.text.startup
.LHOTE7:
	.section	.data.rel.ro,"aw"
	.align 8
.LC2:
	.quad	_ZTVNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE+64
	.align 8
.LC3:
	.quad	_ZTVSt15basic_streambufIcSt11char_traitsIcEE+16
	.align 8
.LC4:
	.quad	_ZTVNSt7__cxx1115basic_stringbufIcSt11char_traitsIcESaIcEEE+16
	.hidden	DW.ref.__gxx_personality_v0
	.weak	DW.ref.__gxx_personality_v0
	.section	.data.rel.local.DW.ref.__gxx_personality_v0,"awG",@progbits,DW.ref.__gxx_personality_v0,comdat
	.align 8
	.type	DW.ref.__gxx_personality_v0, @object
	.size	DW.ref.__gxx_personality_v0, 8
DW.ref.__gxx_personality_v0:
	.quad	__gxx_personality_v0
	.ident	"GCC: (GNU) 13.1.1 20230429"
	.section	.note.GNU-stack,"",@progbits
