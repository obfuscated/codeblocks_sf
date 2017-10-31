// #========================================================================
// #
// # vectors.S
// #
// # ARM exception vectors (Evaluator-7T)
// #
// #
// # Copyright HighTec EDV-Systeme GmbH 1982-2005
// #
// #========================================================================


#define CPSR_IRQ_DISABLE	0x80	/* IRQ disabled when = 1 */
#define CPSR_FIQ_DISABLE	0x40	/* FIQ disabled when = 1 */
#define CPSR_THUMB_ENABLE	0x20	/* Thumb mode   when = 1 */
#define CPSR_MODE32_ENABLE	0x10	/* 32 bit mode  when = 1 */

#define CPSR_MODE_BITS		0x1F
#define CPSR_USER_MODE		0x10
#define CPSR_FIQ_MODE		0x11
#define CPSR_IRQ_MODE		0x12
#define CPSR_SVC_MODE		0x13
#define CPSR_ABORT_MODE		0x17
#define CPSR_UNDEF_MODE		0x1B
#define CPSR_SYSTEM_MODE	0x1F

#define SWI_Location		0x28	/* BootStrap: address of SWI handler */
#define IRQ_Location		0x38	/* BootStrap: address of IRQ handler */

//;
//; # Platform specific definition for Evaluator-7T board
//;

//; # System Manager Group
#define SYSCFG		0x03FF0000
#define EXTDBWTH	(SYSCFG + 0x3010)
#define ROMCON1		(SYSCFG + 0x3018)

//; # Interrupt Controller Group
#define INTMOD		(SYSCFG + 0x4000)
#define INTPND		(SYSCFG + 0x4004)
#define INTMSK		(SYSCFG + 0x4008)




//; #==========================================================================
//; #  Hardware exception vectors.
//; #  The vector table will be copied to location 0x0000 at startup time.
//;
	.code	32
	.section ".vectors","ax"
	.global	__exception_handlers

	b	start
__exception_handlers:
__vector_table_start:
	ldr	pc,.reset_vector
	ldr	pc,.undefined_instruction
	ldr	pc,.software_interrupt
	ldr	pc,.abort_prefetch
	ldr	pc,.abort_data
	.word	0
	ldr	pc,.IRQ
	ldr	pc,.FIQ

//; # The layout of these pointers should match the vector table above since
//; # they are copied in pairs.
	.global	vectors
vectors:
.reset_vector:
	.word	reset_vector
.undefined_instruction:
	.word	undefined_instruction
.software_interrupt:
	.word	software_interrupt
.abort_prefetch:
	.word	abort_prefetch
.abort_data:
	.word	abort_data
	.word	0
.IRQ:
	.word	IRQ
.FIQ:
	.word	FIQ

__vector_table_end:

	.text
//; begin of startup code
start:

	.global	reset_vector
	.type	reset_vector,function
reset_vector:

//; #	PLATFORM_SETUP1		//; # Early stage platform initialization

	ldr	r0,=ROMCON1
	ldr	r0,[r0]
	cmp	r0,#0x0060	/* powerup init value ? */
	bne	20f
	mrs	r0,cpsr		/* set SVC mode */
	bic	r0,r0,#0x1f
	orr	r0,r0,#0xd3
	msr	cpsr_all,r0
	ldr	r0,=SYSCFG
	ldr	r1,=0x03FFFFA0
	str	r1,[r0]		/* Cache,WB disable,Start_addr = 0x3FF0000 */
	mov	r0,sp
	ldr	sp,=0x03FE2000	/* use internal SRAM/Cache as stack */
	str	r0,[sp,#-4]!	/* preserve previous sp on new stack */
	stmdb	sp!,{r0-r12,lr}
	ldr	lr,=segment_register_setups_end
	adr	r0,segment_register_setups
	ldmia	r0,{r1-r12}
	ldr	r0,=EXTDBWTH
	stmia	r0,{r1-r12}
	mov	pc,lr		/* now run critical jump code */

segment_register_setups_end:
	ldmfd	sp!,{r0-r12,lr}
	ldr	r0,[sp],#4
	mov	sp,r0
	b	20f
segment_register_setups:
	.long	0x0000003e	/* Memory Bus Width Register */
	.long	0x18860030	/* ROM Bank0 Control Register */
	.long	0x00400010	/* ROM Bank1 Control Register */
	.long	0x00801010	/* ROM Bank2 Control Register */
	.long	0x08018020	/* ROM Bank3 Control Register */
	.long	0x0a020040	/* ROM Bank4 Control Register */
	.long	0x0c028040	/* ROM Bank5 Control Register */
	.long	0x00000000	/* DRAM Bank0 Control Register */
	.long	0x00000000	/* DRAM Bank1 Control Register */
	.long	0x00000000	/* DRAM Bank2 Control Register */
	.long	0x00000000	/* DRAM Bank3 Control Register */
	.long	0x9c218360	/* External I/O & Memory Refresh Cycle Control Register */
20:


//; # Come here to reset board
warm_reset:
	ldr	r1,=INTMSK
	ldr	r0,=0x3FFFFF	/* disable all interrupts */
	str	r0,[r1]
	mov	r0,#0
	ldr	r1,=INTMOD
	str	r0,[r1]
	ldr	r1,=INTPND
	str	r0,[r1]

//
//; # copy the vector table (__vector_table_start .. __vector_table_end) to address 0
//
#if !defined(USE_HIMO) && !defined(USE_BOOTSTRAP)
//; #  HiMo needs its own exception handlers --> don't overwrite these!!
	mov	r8,#0
	ldr	r9,=__exception_handlers
	ldmia	r9!,{r0-r7}
	stmia	r8!,{r0-r7}
	ldmia	r9!,{r0-r7}
	stmia	r8!,{r0-r7}
#endif /* USE_HIMO */

//	; Relocate [copy] data from ROM to RAM
	ldr	r0,=__rom_data_start
	ldr	r1,=__ram_data_start
	ldr	r2,=__ram_data_end
1:
	cmp	r1,r2		//; # while (r1 < r2)
	ldrcc	r3,[r0],#4	//; # {
	strcc	r3,[r1],#4	//; #   *r1++ = *r0++;
	bcc	1b		//; # }

//	; clear BSS
	ldr	r1,=__bss_start
	ldr	r2,=__bss_end
	mov	r0,#0
1:
	cmp	r1,r2		//; # while (r1 < r2)
	strcc	r0,[r1],#4	//; #   *r1++ = 0;
	bcc	1b

#if defined(USE_IRQ) && (defined(USE_BOOTSTRAP) || defined(USE_HIMO))
//; replace IRQ handler by our own handler
	ldr	r1,=IRQ_Location
	ldr	r0,=BSL_IRQ_Address
	ldr	r2,[r1]
	str	r2,[r0]
	ldr	r2,=IRQ
	str	r2,[r1]
#endif /* USE_BOOTSTRAP */

//	; # initialize interrupt/exception environments
	ldr	sp,=__startup_stack
	mov	r0,#(CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE|CPSR_IRQ_MODE)
	msr	cpsr,r0
	ldr	sp,=__interrupt_stack
	mov	r0,#(CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE|CPSR_FIQ_MODE)
	msr	cpsr,r0
	ldr	sp,=__FIQ_exception_stack
	mov	r0,#(CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE|CPSR_UNDEF_MODE)
	msr	cpsr,r0
	ldr	sp,=__exception_stack
	mov	r0,#(CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE|CPSR_ABORT_MODE)
	msr	cpsr,r0
	ldr	sp,=__exception_stack

//	; # initialize CPSR (machine state register)
	mov	r0,#(CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE|CPSR_SVC_MODE)
	msr	cpsr,r0

//	; # Note: some functions in LIBGCC1 will cause a "restore from SPSR"!!
	msr	spsr,r0

#ifdef USE_IRQ
//; # initialize interrupt tables
	bl	IrqInit
#endif /* USE_IRQ */

#if 1
//; # do low level PXROS initialization if we are in a PXROS environment
	ldr	r0,=PxPrepareInit
	cmp	r0,#0
	movne	lr,pc
	movne	pc,r0
#endif


//	; # switch to user mode, evtl. IRQs enabled
#ifdef USE_IRQ
	mov	r0,#(CPSR_FIQ_DISABLE|CPSR_USER_MODE)
#else
	mov	r0,#(CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE|CPSR_USER_MODE)
#endif /* USE_IRQ */
	msr	cpsr,r0
	ldr	sp,=__user_stack

#ifndef __NO_CTOR_DTOR_SUPPORT__
#ifdef __USES_INITFINI__
	/* Some arm/elf targets use the .init and .fini sections
	   to create constructors and destructors, and for these
	   targets we need to call the _init function and arrange
	   for _fini to be called at program exit.  */
	ldr	r0,=_fini
	bl	atexit
	bl	_init
#endif /* __USES_INITFINI__ */
#endif /* __NO_CTOR_DTOR_SUPPORT__ */

	mov	a1,#0		//; # set argc to 0
	mov	a2,#0		//; # and argv to NUL
	bl	main
#if defined(USE_IRQ) && (defined(USE_BOOTSTRAP) || defined(USE_HIMO))
	bl	IrqInit		//; # stop interrupts
	bl	RestoreBSL	//; # restore BSL environment
#endif /* USE_IRQ */
#ifdef USE_BOOTSTRAP
	mov	r0,#0
	swi	0x11		//; # exit back to BSL
#else
#ifdef __NO_CTOR_DTOR_SUPPORT__
	bl	_exit
#else
	mov	a1,#0
	bl	exit		//; # exit(0)
#endif /* __NO_CTOR_DTOR_SUPPORT__ */
#endif /* USE_BOOTSTRAP */



//;
//; # Exception handlers
//; # Assumption: get here from a Supervisor context [mode]
//;
	.code	32
undefined_instruction:
	b	undefined_instruction

	.code	32
software_interrupt:
	b	software_interrupt

	.code	32
abort_prefetch:
	b	abort_prefetch

	.code	32
abort_data:
	b	abort_data


	.code	32
FIQ:
        b       FIQ


IRQ:
#ifdef USE_IRQ
	sub	lr,lr,#4		//; adjust return address before saving it
	str	lr,[sp,#-4]!
	mrs	lr,spsr			//; and status bits
	stmfd	sp!,{r0-r3,r12,lr}	//; save APCS working register and SPSR

//	; switch to SVC mode (to avoid  problem with C language handler code)
	mrs	lr,cpsr			//; read the status register
	bic	lr,lr,#CPSR_MODE_BITS	//; clear the mode bits
	orr	lr,lr,#CPSR_SVC_MODE	//; switch to SVC mode
	msr	cpsr_cf,lr
	nop

	stmfd	sp!,{lr}		//; save original lr_svc

	bl	IrqHandler

	ldmfd	sp!,{lr}		//; restore original lr_svc
//	; and switch back to IRQ mode
	mrs	r12,cpsr		//; read the status register
	bic	r12,r12,#CPSR_MODE_BITS	//; clear the mode bits
	orr	r12,r12,#CPSR_IRQ_MODE	//; switch to IRQ mode
	msr	cpsr_cf,r12		//; write it back
	nop

	ldmfd	sp!,{r0-r3,r12,lr}	//; restore APCS working register and SPSR
	msr	spsr_cf,lr
	ldmfd	sp!,{pc}^		//; and return from interrupt and restore CPSR
#else
	b	IRQ
#endif /* USE_IRQ */



#if defined(USE_IRQ) && (defined(USE_BOOTSTRAP) || defined(USE_HIMO))
//; restore BSL's original exception handler environment

	.global	RestoreBSL
RestoreBSL:
	ldr	r0,=BSL_IRQ_Address
	ldr	r0,[r0]
	ldr	r1,=IRQ_Location
	str	r0,[r1]

//; restore BSL's SWI handler if it's a PXROS application
	ldr	r0,=PxPrepareInit
	cmp	r0,#0
	moveq	pc,lr

	ldr	r0,=oldSWIHandler
	ldr	r0,[r0]
	ldr	r1,=SWI_Location
	str	r0,[r1]

	mov	pc,lr
#endif /* USE_BOOTSTRAP */



//; # -------------------------------------------------------------------------
//; # data section used by startup code

	.data

//; # -------------------------------------------------------------------------
//; # Temporary interrupt stack

	.section ".bss"

#if defined(USE_IRQ) && (defined(USE_BOOTSTRAP) || defined(USE_HIMO))
BSL_IRQ_Address:
	.long	0
#endif /* USE_BOOTSTRAP */

	.global	__interrupt_stack
	.global	__startup_stack
	.global	_PxSysstackend

//; # Small stacks, only used for saving information between CPU modes
__exception_stack_base:
	.rept	32
	.long	0
	.endr
__FIQ_exception_stack:
	.rept	32
	.long	0
	.endr
__exception_stack:

//; # Runtime stack used during all IRQ interrupt processing
#ifndef IRQ_STACK_SIZE
#ifdef USE_IRQ
#define IRQ_STACK_SIZE		2048
#else
#define IRQ_STACK_SIZE		16*4
#endif /* USE_IRQ */
#endif /* IRQ_STACK_SIZE */

	.balign 16
__interrupt_stack_base:
	.rept	IRQ_STACK_SIZE
	.byte	0
	.endr
	.balign 16
__interrupt_stack:
//; # the following 2 words are used for PXROS taskret storage
	.long	0
	.long	0

#ifndef STARTUP_STACK_SIZE
#define STARTUP_STACK_SIZE	2048
#endif /* STARTUP_STACK_SIZE */

	.balign 16
_PxSysstackend:
__startup_stack_base:
	.rept	STARTUP_STACK_SIZE
	.byte	0
	.endr
	.balign 16
__startup_stack:

#ifndef USER_STACK_SIZE
#define USER_STACK_SIZE		2048
#endif /* USER_STACK_SIZE */

	.balign 16
__user_stack_base:
	.rept	USER_STACK_SIZE
	.byte	0
	.endr
	.balign 16
__user_stack:


//; # --------------------------------------------------------------------------
//; #  end of vectors.S
