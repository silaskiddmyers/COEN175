#Procedure:
main:
#Prepare Function:
	push	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
#Block:
#Assignment:
	movl	$10, %eax
	movl	%eax, 4294967292(%ebp)

#Function Call:
	pushl	4294967292(%ebp)

	call	init_array
#Function Call:
	pushl	4294967292(%ebp)

	call	print_array
#Close Function:
	movl	%ebp, %esp
	popl	%ebp
	ret
	.globl	main
#Global Variables:
	.comm	a, 40
