/*
 * crt0.S -- startup file for PowerPC systems.
 *
 */


	.text
	.globl	_start
	.type	_start,@function
_start:
#if 1
	/* load the system stack */
	lis	%r1, __USER_STACK_TOP@ha
	la	%r1, __USER_STACK_TOP@l(%r1)
#endif


	/* initialize data sections */
	bl	clear_table
	bl	copy_table

	/* initialize interrupts */

	bl	Mpc52xxInitInterrupts

	li	%r3,0		/* argc = 0 */
	li 	%r4,0		/* argv = NULL */
	/* Let her rip */
	bl	main

	/* return value from main is argument to exit */
	bl	exit
.Lstart:
	.size	_start,.Lstart-_start

	.extern	atexit
	.globl	__atexit
	.section ".sdata","aw"
	.align	2
__atexit:			/* tell C's eabi-ctor's we have an atexit function */
	.long	atexit@fixup	/* and that it is to register __do_global_dtors */

	.section ".fixup","aw"
	.align	2
	.long	__atexit


	.section	".text"
	.align 2
	.globl clear_table
	.type	 clear_table,@function
clear_table:
	lis	%r9,__clear_table@ha
	la	%r9,__clear_table@l(%r9)
	lwz	%r0,4(%r9)
	cmpwi	%r0,-1
	beqlr
clear_loop1:
	lwz	%r0,4(%r9)
	addi	%r11,%r9,8
	srwi.	%r10,%r0,2
	lwz	%r9,0(%r9)
	mtctr	%r10
	beq	clear_loop3
	li	%r0,0
clear_loop2:
	stw	%r0,0(%r9)
	addi	%r9,%r9,4
	bdnz	clear_loop2
clear_loop3:
	mr	%r9,%r11
	lwz	%r0,4(%r9)
	cmpwi	%r0,-1
	bne	clear_loop1
	blr
clear_table_end:
	.size	 clear_table,clear_table_end-clear_table
	.align 2

	.globl copy_table
	.type	 copy_table,@function
copy_table:
	lis	%r9,__copy_table@ha
	la	%r9,__copy_table@l(%r9)
	lwz	%r0,8(%r9)
	cmpwi	%r0,-1
	beqlr
copy_loop1:
	lwz	%r0,8(%r9)
	addi	%r10,%r9,12
	srwi.	%r11,%r0,2
	mtctr	%r11
	lwz	%r11,0(%r9)
	lwz	%r9,4(%r9)
	beq	copy_loop3
copy_loop2:
	lwz	%r0,0(%r11)
	addi	%r11,%r11,4
	stw	%r0,0(%r9)
	addi	%r9,%r9,4
	bdnz	copy_loop2
copy_loop3:
	mr	%r9,%r10
	lwz	%r0,8(%r9)
	cmpwi	%r0,-1
	bne	copy_loop1
	blr
copy_table_end:
	.size	 copy_table,copy_table_end-copy_table

