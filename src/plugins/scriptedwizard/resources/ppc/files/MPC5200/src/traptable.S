/******************************************************************************
 * $Id$
 *
 * This file defines the global Interrupt and Exception Table for the MPC 555
 * We use to Interrupt stacks
 * - The TrapFrameStack
 *		which holds a Constant amount of saved registers
 * - The InterruptStack
 *		on this stack all C-Function are called
 *
 * The TrapFrameStacks looks like:
 *
 *	top:	 0:	ps	(program status)
 *		 4:	pc	(program counter)
 *		 8:	cr
 *		12:	lr
 *		16:	xer
 *		20:	ctr
 *		24:	r15
 *		28:	r14
 *		32:	r13
 *		36:	r12
 *		40:	r11
 *		44:	r10
 *		48:	r9
 *		52:	r8
 *		56:	r7
 *		60:	r6
 *		64:	r5
 *		68:	r4
 *		72:	r3
 *		76:	r2
 *		80:	r1	(the stack pointer)
 *		84:	r0
 *
 * if you use PXROS function in your interrupts the trap frame has always
 * to look like this (because PXROS saves and restores Taskregister from
 * this frame
 * in "C" this TrapFrame is defined through the structure
 *	PPCTrapFrame_T		defined in pxtrp.h
*/
#include "pxtrp.h"

#if 0
#define TRAPDisablePending()		\
	lis	%r6,__SIPEND@ha		;\
	lbz	%r4,__SIVEC@l(%r6)	;\
	lhz	%r3,__SIPEND@l(%r6)	;\
	lhz	%r5,__SIMASK@l(%r6)	;\
	and	%r3,%r5,%r3		;\
	andc	%r5,%r5,%r3		;\
	sth	%r5,__SIMASK@l(%r6)

#define TRAPEnablePending()		\
	lis	%r4,__SIMASK@ha		;\
	lhz	%r5,__SIMASK@l(%r4)	;\
	or	%r5,%r5,%r3		;\
	sth	%r5,__SIMASK@l(%r4)
#else
#define TRAPEnablePending()
#define TRAPDisablePending()
#endif

#if defined(HIMO_DEBUG)
#if defined(PHYCORE_MPC555) || defined(OAK_EMUF)
# define FLASHBASE	0xFFF00000
#else
# define FLASHBASE	0xFFC00000
#endif
#define HIMO_TRAP	(FLASHBASE+0x4000)

	/* we use the HiMo (HighTecs Monitor) to debug the application
	   the HiMo installs his own Traptable at 0x4000. Therefore we
	   we jump to the Monitor trapentry as the default trap action
	*/
#define DefaultTrapEntry(trapno)				\
	ba	HIMO_TRAP+trapno
#else

#define DefaultTrapEntry(trapno)				\
	TRAPEntry()						;\
	TRAPSaveCR(14)						;\
	TRAPSwitchStack(14)					;\
	TRAPSaveLR(14)						;\
	TRAPSaveGPRS(14)					;\
	mr	%r4,%r14					;\
	li	%r3,trapno					;\
	lis	%r5,PPCDefaultTrapHandler@ha			;\
	la	%r5,PPCDefaultTrapHandler@l(%r5)		;\
	mtlr	%r5 						;\
	subi	%r1,%r1,8					;\
	blrl							;\
	addi	%r1,%r1,8					;\
								;\
	b	__trap_return
#endif

	.section .vectab, "awx", @progbits
	.globl DecInterrupt
	.globl _TrapDispatch
	.globl Px_svcentry
	.align 8

	.globl _ppc_InterruptVectorTable
_ppc_InterruptVectorTable:

/******************************************************************************
*
*	0x0100 System Reset
*
******************************************************************************/
	.org	0x100
	.globl _ppc_reset
_ppc_reset:
	ba	_start
	/* we have nothing to save because we reset the system
		just call something like reset
	*/

/******************************************************************************
*
*	0x0200 Machine Check
*
******************************************************************************/
	.org 	0x200
	.globl _ppc_machine_check
_ppc_machine_check:
	DefaultTrapEntry(0x200)

/******************************************************************************
*
*	0x0300 Data Storage (this is reserved on mpc 555)
*
******************************************************************************/
	.org	0x300
	.globl _ppc_data_storage
_ppc_data_storage:
	DefaultTrapEntry(0x300)


/******************************************************************************
*
*	0x0400 Instruction Storage (this is reserved on mpc 555)
*
******************************************************************************/
	.org	0x400
	.globl _ppc_instruction_storage
_ppc_instruction_storage:

	DefaultTrapEntry(0x400)

/******************************************************************************
*
*	0x0500 External Interrupt
*
* 	This trap needs a special handling.
*	Here we get all the interrupts from the internal ICU and have to take
*	special care to reenable the interrupts
*
******************************************************************************/
	.org	0x500

	.globl _ppc_external_interrupt
	b _ppc_external_interrupt

/******************************************************************************
*
*	0x0600 Alignment
*
******************************************************************************/
	.org	0x600

	.globl _ppc_alignment_trap
_ppc_alignment_trap:

	DefaultTrapEntry(0x600)


/******************************************************************************
*
*	0x0700 Program
*
******************************************************************************/
	.org	0x700
	.globl _ppc_program_trap
_ppc_program_trap:

	DefaultTrapEntry(0x700)


/******************************************************************************
*
*	0x0800 Floating Point Unavailable
*
******************************************************************************/
	.org	0x800
	.globl _ppc_fp_unavailable_trap
_ppc_fp_unavailable_trap:

	DefaultTrapEntry(0x800)

/******************************************************************************
*
*	0x0900 Decrementer
*
******************************************************************************/
	.org	0x900
	.globl _ppc_decrementer
_ppc_decrementer:

#ifndef DONT_USE_DECREMENTER
	TRAPEntry()
	mtspr	80, 12			 /* set EE and RI */
	TRAPSaveCR(14)
	TRAPSwitchStack(14)
	TRAPSaveLR(14)
	TRAPSaveGPRS(14)
	TRAPSetMsr()
	lis 	%r12,DecInterrupt@ha
	la 	%r12,DecInterrupt@l(%r12)
	mtlr	%r12
	subi	%r1,%r1,8
	blrl
	addi	%r1,%r1,8

	b	__trap_return
#else
	DefaultTrapEntry(0x900)
#endif /* !DONT_USE_DECREMENTER */

/******************************************************************************
*
*	0x0C00 System Call
*
* here we have to handle the PXROS system call entry
* we only save the interrupt frame all other saves and restores will be done
* by PXROS
******************************************************************************/
	.org	0xC00

	.globl _ppc_sc_call
_ppc_sc_call:
	PXROS_SVC

/******************************************************************************
*
*	0x0D00 Trace
*
******************************************************************************/
	.org	0xD00

	.globl _ppc_trace_trap
_ppc_trace_trap:

	DefaultTrapEntry(0xd00)


/******************************************************************************
*
 *	0x0E00 FP Assist
*
******************************************************************************/
	.org	0xE00

	.globl _ppc_fp_assist
_ppc_fp_assist:

	DefaultTrapEntry(0xe00)

/******************************************************************************
*
*	0x1000 	MPC8xx and MPC505:	Software Emulation
*			PPC603e/82xx:		Instruction TLB Miss
*
******************************************************************************/
	.org	0x1000
	.globl _ppc_software_emulation_trap
_ppc_software_emulation_trap:

	DefaultTrapEntry(0x1000)

/******************************************************************************
*
*	0x1100 	MPC8xx:				Instruction TLB Miss
*			PPC603e/82xx:		Data Load TLB Miss
*		MPC5xx:				Reserved
*
******************************************************************************/
	.org	0x1100
	.globl _ppc_instruction_tlb_miss
_ppc_instruction_tlb_miss:


	DefaultTrapEntry(0x1100)

/******************************************************************************
*
*	0x1200	MPC8xx:				Data TLB Miss
*			PPC603e/82xx:		Data Store TLB Miss
*		MPC5xx:				Reserved
*
******************************************************************************/
	.org	0x1200
	.globl _ppc_data_tlb_miss
_ppc_data_tlb_miss:

	DefaultTrapEntry(0x1200)

/******************************************************************************
*
*	0x1300	MPC8xx:				Instruction TLB Error
*			PPC7xx and PPC603e/82xx:Instruction address breakpoint
*
******************************************************************************/
	.org	0x1300
	.globl _ppc_instruction_protection_trap
_ppc_instruction_protection_trap:

	DefaultTrapEntry(0x1300)

/******************************************************************************
*
*	0x1400	MPC8xx:					Data TLB Error
*			PPC7xx and PPC603e/82xx/52xx:	System management
*
******************************************************************************/
	.org	0x1400
	.globl _ppc_data_protection_trap
_ppc_data_protection_trap:
	b _ppc_external_interrupt


/******************************************************************************
*
*	0x1C00 MPC8xx and MPC505:	Data breakpoint
*
******************************************************************************/
	.org	0x1C00
	.globl _ppc_data_breakpoint
_ppc_data_breakpoint:

	DefaultTrapEntry(0x1c00)


/******************************************************************************
*
*	0x1D00 MPC8xx and MPC505:	Instruction breakpoint
*
******************************************************************************/
	.org	0x1D00
	.globl _ppc_instruction_breakpoint
_ppc_instruction_breakpoint:

	DefaultTrapEntry(0x1d00)


/******************************************************************************
*
*	0x1E00 MPC8xx and MPC505:	maskable external breakpoint
*
******************************************************************************/
	.org	0x1E00
	.globl _ppc_maskable_breakpoint
_ppc_maskable_breakpoint:

	DefaultTrapEntry(0x1e00)


/******************************************************************************
*
*	0x1F00 MPC8xx and MPC505:	Non-maskable development port
*
******************************************************************************/
	.org	0x1F00
	.globl _ppc_development_breakpoint
_ppc_development_breakpoint:

	DefaultTrapEntry(0x1f00)



__trap_return:
	TRAPRestoreGPRS()
	TRAPExit()


_ppc_external_interrupt:

	TRAPEntry()
	mtspr	81, 12			/* set RI */
	TRAPSaveCR(14)
	TRAPSwitchStack(14)
	stw	%r0,_TF_GPR_OFFS(0)(%r14)
	stw	%r2,_TF_GPR_OFFS(2)(%r14)
	stw	%r3,_TF_GPR_OFFS(3)(%r14)
	stw	%r4,_TF_GPR_OFFS(4)(%r14)

	mtspr	80, 12			/* set EE and RI	*/
	stw	%r5,_TF_GPR_OFFS(5)(%r14)
	stw	%r6,_TF_GPR_OFFS(6)(%r14)
	stw	%r7,_TF_GPR_OFFS(7)(%r14)
	stw	%r8,_TF_GPR_OFFS(8)(%r14)
	stw	%r9,_TF_GPR_OFFS(9)(%r14)
	stw	%r10,_TF_GPR_OFFS(10)(%r14)
	stw	%r11,_TF_GPR_OFFS(11)(%r14)
	stw	%r13,_TF_GPR_OFFS(13)(%r14)
	mflr	%r11
	stw	%r11,_TF_LR_OFFS(%r14)
	mfxer	%r11
	stw	%r11,_TF_XER_OFFS(%r14)
	mfctr	%r11
	stw	%r11,_TF_CTR_OFFS(%r14)
	TRAPSetMsr()

	lis	%r12,_TrapDispatch@ha
	la	%r12,_TrapDispatch@l(%r12)
	mtlr	%r12
	subi	%r1,%r1,8
	blrl				/* call the dispatcher to handle the
					   interrupt */
	addi	%r1,%r1,8

	b	__trap_return
	.globl _ppc_InterruptVectorTableEnd
_ppc_InterruptVectorTableEnd:
