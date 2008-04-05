/*
 * crt0.S -- startup file for PowerPC systems.
 *
 */

#include "mpc555.h"

/*
# definitions for clock frequency for phyCOREMPC555/phyCOREMPC565
# default is phyCOREMPC565 at 40 MHz
*/

#ifdef PHYCORE_MPC555
# define CLOCK_SCALE	0x00104000	/* 40 MHz ((1+1) * 20 MHz) */
#else
# ifdef FAST_CLOCK
#  define CLOCK_SCALE	0x00D04000	/* 56 MHz ((13+1) * 4 MHz) */
# else
#  define CLOCK_SCALE	0x00904000	/* 40 MHz (( 9+1) * 4 MHz) */
# endif
#endif

	.file	"crt0.S"
	.text
	.globl	_start
	.type	_start,@function
_start:
	/*
	** Make the processor to run at specified clock frequency
	** by programming PLPRCR register with suitable value
	*/
#if !defined(HIMO_DEBUG)
	lis	%r4, __PLPRCRK@ha /* unlock PLPRCR register */
	lis	%r5, 0x55cc
	ori	%r5, %r5, 0xaa33
	stw	%r5, __PLPRCRK@l(%r4)

	lis	%r4, __PLPRCR@ha
	lwz	%r5, __PLPRCR@l(%r4)
	lis	%r5, (CLOCK_SCALE>>16)
	ori	%r5, %r5, (CLOCK_SCALE & 0xffff)
	stw	%r5, __PLPRCR@l(%r4)

	lis	%r4, __SCCR@ha
	lwz	%r5, __SCCR@l(%r4)
	lis	%r6, 0x0100	/* clear RTDIV: set pre divider to 4 */
	andc	%r5,%r5,%r6
	stw	%r5, __SCCR@l(%r4)

#endif
	lis	%r4, __SIMASK@ha
	li	%r5, 0
	sth	%r5, __SIMASK@l(%r4)


	li	%r5, 0x2002
	mtmsr	%r5

	/* load the system stack */
	lis	%r1, __USER_STACK_TOP@ha
	la	%r1, __USER_STACK_TOP@l(%r1)

	lis	%r0,0
	stw	%r0,0(%r1)	/* clear back chain */
	stwu	%r1,-64(%r1)	/* push another stack frame */


	/* initialize data sections */
	bl	clear_table
	bl	copy_table

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

