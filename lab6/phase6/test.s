node_insert:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$node_insert.size, %esp
	movl	8(%ebp), %eax
	cmpl	$0, %eax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L0
	pushl	$12
	call	malloc
	addl	$4, %esp
	movl	%eax, 0+8(%ebp)
	movl	12(%ebp), %eax
	movl	8(%ebp), %ecx
	movl	%eax, (%ecx)
	movl	null, %eax
	movl	8(%ebp), %ecx
	movl	%eax, 4(%ecx)
	movl	null, %eax
	movl	8(%ebp), %ecx
	movl	%eax, 8(%ecx)
	jmp	.L1
.L0:
	movl	8(%ebp), %eax
	movl	0 (%eax), %eax
	pushl	%eax
	pushl	12(%ebp)
	movl	16(%ebp), %eax
	call	*%eax
	addl	$8, %esp
	cmpl	$0, %eax
	setl	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L2
	pushl	16(%ebp)
	pushl	12(%ebp)
	movl	8(%ebp), %eax
	movl	4 (%eax), %eax
	pushl	%eax
	call	node_insert
	addl	$12, %esp
	movl	8(%ebp), %ecx
	movl	%eax, 4(%ecx)
	jmp	.L3
.L2:
	movl	8(%ebp), %eax
	movl	0 (%eax), %eax
	pushl	%eax
	pushl	12(%ebp)
	movl	16(%ebp), %eax
	call	*%eax
	addl	$8, %esp
	cmpl	$0, %eax
	setg	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L4
	pushl	16(%ebp)
	pushl	12(%ebp)
	movl	8(%ebp), %eax
	movl	8 (%eax), %eax
	pushl	%eax
	call	node_insert
	addl	$12, %esp
	movl	8(%ebp), %ecx
	movl	%eax, 8(%ecx)
.L4:
.L3:
.L1:
	movl	8(%ebp), %eax
	jmp	node_insert.exit

node_insert.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	node_insert.size, 0
	.globl	node_insert

node_search:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$node_search.size, %esp
	movl	8(%ebp), %eax
	cmpl	$0, %eax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L6
	movl	$0, %eax
	jmp	node_search.exit
.L6:
	movl	8(%ebp), %eax
	movl	0 (%eax), %eax
	pushl	%eax
	pushl	12(%ebp)
	movl	16(%ebp), %eax
	call	*%eax
	addl	$8, %esp
	cmpl	$0, %eax
	setl	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L8
	pushl	16(%ebp)
	pushl	12(%ebp)
	movl	8(%ebp), %eax
	movl	4 (%eax), %eax
	pushl	%eax
	call	node_search
	addl	$12, %esp
	jmp	node_search.exit
.L8:
	movl	8(%ebp), %eax
	movl	0 (%eax), %eax
	pushl	%eax
	pushl	12(%ebp)
	movl	16(%ebp), %eax
	call	*%eax
	addl	$8, %esp
	cmpl	$0, %eax
	setg	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L10
	pushl	16(%ebp)
	pushl	12(%ebp)
	movl	8(%ebp), %eax
	movl	8 (%eax), %eax
	pushl	%eax
	call	node_search
	addl	$12, %esp
	jmp	node_search.exit
.L10:
	movl	$1, %eax
	jmp	node_search.exit

node_search.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	node_search.size, 0
	.globl	node_search

node_preorder:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$node_preorder.size, %esp
	movl	8(%ebp), %eax
	cmpl	$0, %eax
	je	.L12
	movl	8(%ebp), %eax
	movl	0 (%eax), %eax
	pushl	%eax
	leal	.L14, %eax
	pushl	%eax
	call	printf
	addl	$8, %esp
	movl	8(%ebp), %eax
	movl	4 (%eax), %eax
	pushl	%eax
	call	node_preorder
	addl	$4, %esp
	movl	8(%ebp), %eax
	movl	8 (%eax), %eax
	pushl	%eax
	call	node_preorder
	addl	$4, %esp
.L12:

node_preorder.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	node_preorder.size, 0
	.globl	node_preorder

node_inorder:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$node_inorder.size, %esp
	movl	8(%ebp), %eax
	cmpl	$0, %eax
	je	.L15
	movl	8(%ebp), %eax
	movl	4 (%eax), %eax
	pushl	%eax
	call	node_inorder
	addl	$4, %esp
	movl	8(%ebp), %eax
	movl	0 (%eax), %eax
	pushl	%eax
	leal	.L14, %eax
	pushl	%eax
	call	printf
	addl	$8, %esp
	movl	8(%ebp), %eax
	movl	8 (%eax), %eax
	pushl	%eax
	call	node_inorder
	addl	$4, %esp
.L15:

node_inorder.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	node_inorder.size, 0
	.globl	node_inorder

compare:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$compare.size, %esp
	movl	12(%ebp), %eax
	subl	8(%ebp), %eax
	jmp	compare.exit

compare.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	compare.size, 0
	.globl	compare

insert:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$insert.size, %esp
	movl	8(%ebp), %eax
	movl	0 (%eax), %eax
	pushl	%eax
	pushl	12(%ebp)
	movl	8(%ebp), %eax
	movl	4 (%eax), %eax
	pushl	%eax
	call	node_insert
	addl	$12, %esp
	movl	8(%ebp), %ecx
	movl	%eax, 4(%ecx)

insert.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	insert.size, 0
	.globl	insert

search:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$search.size, %esp
	movl	8(%ebp), %eax
	movl	0 (%eax), %eax
	pushl	%eax
	pushl	12(%ebp)
	movl	8(%ebp), %eax
	movl	4 (%eax), %eax
	pushl	%eax
	call	node_search
	addl	$12, %esp
	jmp	search.exit

search.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	search.size, 0
	.globl	search

main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$main.size, %esp
	movl	null, %eax
	movl	%eax, 4+-12(%ebp)
	leal	compare, %eax
	movl	%eax, 0+-12(%ebp)
	pushl	$7
	leal	-12(%ebp), %eax
	pushl	%eax
	call	insert
	addl	$8, %esp
	pushl	$4
	leal	-12(%ebp), %eax
	pushl	%eax
	call	insert
	addl	$8, %esp
	pushl	$1
	leal	-12(%ebp), %eax
	pushl	%eax
	call	insert
	addl	$8, %esp
	pushl	$0
	leal	-12(%ebp), %eax
	pushl	%eax
	call	insert
	addl	$8, %esp
	pushl	$5
	leal	-12(%ebp), %eax
	pushl	%eax
	call	insert
	addl	$8, %esp
	pushl	$2
	leal	-12(%ebp), %eax
	pushl	%eax
	call	insert
	addl	$8, %esp
	pushl	$3
	leal	-12(%ebp), %eax
	pushl	%eax
	call	insert
	addl	$8, %esp
	pushl	$6
	leal	-12(%ebp), %eax
	pushl	%eax
	call	insert
	addl	$8, %esp
	leal	.L18, %eax
	pushl	%eax
	call	printf
	addl	$4, %esp
	movl	4+-12(%ebp), %eax
	pushl	%eax
	call	node_preorder
	addl	$4, %esp
	leal	.L19, %eax
	pushl	%eax
	call	printf
	addl	$4, %esp
	movl	4+-12(%ebp), %eax
	pushl	%eax
	call	node_inorder
	addl	$4, %esp

main.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	main.size, 12
	.globl	main

	.comm	null, 4
	.data	
.L14:	.asciz	"%d\012"
.L19:	.asciz	"inorder traversal:\012"
.L18:	.asciz	"preorder traversal:\012"
