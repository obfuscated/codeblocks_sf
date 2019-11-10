// #========================================================================
// #
// # vectors.S
// #
// # ARM exception vectors (phyCORE_PXA255)
// #
// #
// # Copyright HighTec EDV-Systeme GmbH 1982-2006
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
#define CPSR_DBG_MODE		0x15	/* XScale extension */
#define CPSR_ABORT_MODE		0x17
#define CPSR_UNDEF_MODE		0x1B
#define CPSR_SYSTEM_MODE	0x1F

#define SWI_Location		0x28	/* HiMo: address of SWI handler */
#define IRQ_Location		0x38	/* HiMo: address of IRQ handler */

//;
//; # Platform specific definition for phyCORE-PXA255 board
//;

/* Reset Controller Registers */
#define RST_BASE		0x40F00030
#define RCSR_OFFS		0
#define RCSR			(RST_BASE + 0x00)

/* Operating System Timer */
#define OSTIMER_BASE	0x40A00000
#define OSMR3_OFFS	0x0C
#define OSCR_OFFS	0x10
#define OWER_OFFS	0x18
#define OIER_OFFS	0x1C

/* Clock Manager Register */
#define CC_BASE		0x41300000
#define CCCR_OFFS	0x00
#define CKEN_OFFS	0x04

/* Interrupt Controller Registers */
#define IC_BASE		0x40D00000
/* Interrupt Mask Register */
#define ICMR		(IC_BASE + 0x04)
#define ICMR_OFFS	0x04

/* MMU bit definitions */

#define MMU_Control_M	(1 << 0)	/* Enable MMU */
#define MMU_Control_A	(1 << 1)	/* Enable address alignment faults */
#define MMU_Control_C	(1 << 2)	/* Enable cache */
#define MMU_Control_B	(1 << 7)	/* Enable Big-Endian */
#define MMU_Control_S	(1 << 8)	/* Enable system protection */
#define MMU_Control_R	(1 << 9)	/* Enable ROM protection */
#define MMU_Control_Z	(1 << 11)	/* Branch Target Buffer Enable */
#define MMU_Control_I	(1 << 11)	/* Enable Instruction cache */
#define MMU_Control_V	(1 << 12)	/* Interrupt vectors at 0xFFFF0000 */

#define PHYS_RAM_BASE	0xa0000000

/* Intel proposed macro for writing CP15 */
	.macro CPWAIT
	mrc	p15,0,r0,c2,c0,0
	mov	r0,r0
	sub	pc,pc,#4
	.endm


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

//; # Init MMU
//; # MMU disabled, Alignment fault disabled, DCache disabled
//; # exceptions vectors at 0
	mrc	p15,0,r1,c1,c0
	bic	r1,r1,#(MMU_Control_M+MMU_Control_A+MMU_Control_C)
	bic	r1,r1,#MMU_Control_V
	mcr	p15,0,r1,c1,c0

	mov	r0,#0
	mcr	p15,0,r0,c8,c7,0	//; # invalidate all TLBs
	mcr	p15,0,r0,c7,c7,0	//; # invalidate caches

	/* mask all IRQs */
	ldr	r0,=IC_BASE
	mov	r1,#0
	str	r1,[r0,#ICMR_OFFS]

//; # initialize clock
	ldr	r0,=CC_BASE
	mov	r1,#0			//; disable all peripheral clocks
	str	r1,[r0,#CKEN_OFFS]	//; CKEN Clock Enable Register
#ifdef FAST_CLOCK
	ldr	r1,=0x161		//; 4 x fMemory
#else
	ldr	r1,=0x141		//; 2 x fMemory
#endif /* FAST_CLOCK */
	str	r1,[r0,#CCCR_OFFS]	//; CCCR Core Clock Configuration Register
	mov	r0,#2
	mcr	p14,0,r0,c6,c0		//; activate clock change

	/* ToDo: initialize RAM/ROM/SDRAM */

#ifndef DONT_USE_MMU
	/* initialize MMU tables */
	ldr	r0,=PHYS_RAM_BASE
	ldr	sp,=__startup_stack
	add	sp,sp,r0
	bl	pxa_mmu_init

	/* MMU on */
	ldr	r2,=1f
	mrc	p15,0,r0,c1,c0,0
	orr	r0,r0,#MMU_Control_M	/* enable MMU */
	mcr	p15,0,r0,c1,c0,0
	mov	pc,r2
1:
	nop
	nop
	nop
#endif /* DONT_USE_MMU */


//;
//; # copy the vector table (__vector_table_start .. __vector_table_end) to address 0
//;
#ifndef USE_HIMO
//; #  HiMo needs its own exception handlers --> don't overwrite these!!
	mov	r8,#0
	ldr	r9,=__exception_handlers
	ldmia	r9!,{r0-r7}
	stmia	r8!,{r0-r7}
	ldmia	r9!,{r0-r7}
	stmia	r8!,{r0-r7}
#endif /* !USE_HIMO */

//	; Relocate [copy] data from ROM to RAM
	ldr	r0,=__rom_data_start
	ldr	r1,=__ram_data_start
	ldr	r2,=__ram_data_end
1:
	cmp	r1,r2		//; #  while (r1 < r2)
	ldrcc	r3,[r0],#4	//; #  {
	strcc	r3,[r1],#4	//; #    *r1++ = *r0++;
	bcc	1b		//; #  }

//	; clear BSS
	ldr	r1,=__bss_start
	ldr	r2,=__bss_end
	mov	r0,#0
1:
	cmp	r1,r2		//; # while (r1 < r2)
	strcc	r0,[r1],#4	//; #   *r1++ = 0;
	bcc	1b

#if defined(USE_IRQ) && defined(USE_HIMO)
//; replace IRQ handler by our own handler
	ldr	r1,=IRQ_Location
	ldr	r0,=HIMO_IRQ_Address
	ldr	r2,[r1]
	str	r2,[r0]
	ldr	r2,=IRQ
	str	r2,[r1]
#endif /* USE_IRQ && USE_HIMO */

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

#ifndef DONT_USE_MMU
#ifdef ENABLE_ICACHE
	mrc	p15,0,r0,c1,c0,0
	orr	r0,r0,#MMU_Control_I	/* enable instruction cache */
#ifdef ENABLE_DCACHE
	orr	r0,r0,#(MMU_Control_C)	/* enable data cache */
#endif /* ENABLE_DCACHE */
	mcr	p15,0,r0,c1,c0,0
	CPWAIT
#endif /* ENABLE_ICACHE */
#endif /* DONT_USE_MMU */

//	; # switch to user mode, evtl. IRQs enabled
#ifdef USE_IRQ
#ifdef USE_SYSTEM_MODE
	mov	r0,#(CPSR_FIQ_DISABLE | CPSR_SYSTEM_MODE)
#else
	mov	r0,#(CPSR_FIQ_DISABLE | CPSR_USER_MODE)
#endif /* USE_SYSTEM_MODE */
#else
#ifdef USE_SYSTEM_MODE
	mov	r0,#(CPSR_IRQ_DISABLE | CPSR_FIQ_DISABLE | CPSR_SYSTEM_MODE)
#else
	mov	r0,#(CPSR_IRQ_DISABLE | CPSR_FIQ_DISABLE | CPSR_USER_MODE)
#endif /* USE_SYSTEM_MODE */
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
#endif /* !__NO_CTOR_DTOR_SUPPORT__ */

	mov	a1,#0		//; # set argc to 0
	mov	a2,#0		//; # and argv to NUL
	bl	main
#ifdef USE_HIMO
#ifdef USE_IRQ
	bl	IrqInit		//; # stop interrupts
	bl	RestoreHIMO	//; # restore HIMO environment
#endif /* USE_IRQ */
#endif /* USE_HIMO */

#ifdef __NO_CTOR_DTOR_SUPPORT__
	bl	_exit
#else
	mov	a1,#0
	bl	exit		//; # exit(0)
#endif /* __NO_CTOR_DTOR_SUPPORT__ */



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
	b	FIQ



IRQ:
#ifdef USE_IRQ
	sub	lr,lr,#4		//; adjust return address before saving it
	str	lr,[sp,#-4]!
	mrs	lr,spsr			//; and status bits
	stmfd	sp!,{r0-r3,r12,lr}	//; save APCS working register and SPSR

//	; switch to another mode (to avoid  problem with C language handler code)
//	; SVC mode if interrupted service else SYSTEM mode (interrupted task)
	mrs	r0,cpsr			//; read the current status register
	bic	r0,r0,#CPSR_MODE_BITS	//; clear the mode bits
	and	lr,lr,#CPSR_MODE_BITS	//; extract the mode bits of interrupted state
	cmp	lr,#CPSR_SVC_MODE
	orreq	r0,r0,#CPSR_SVC_MODE	//; switch to SVC mode
	orrne	r0,r0,#CPSR_SYSTEM_MODE	//; switch to SYSTEM mode
	msr	cpsr_cf,r0
	nop

	stmfd	sp!,{lr}		//; save original lr (lr_svc|lr)

	bl	IrqHandler

	ldmfd	sp!,{lr}		//; restore original lr (lr_svc|lr)
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



#if defined(USE_IRQ) && defined(USE_HIMO)
//; restore HIMO's original exception handler environment

	.global RestoreHIMO
RestoreHIMO:
	ldr	r0,=HIMO_IRQ_Address
	ldr	r0,[r0]
	ldr	r1,=IRQ_Location
	str	r0,[r1]

//; restore HIMO's SWI handler if it's a PXROS application
	ldr	r0,=PxPrepareInit
	cmp	r0,#0
	moveq	pc,lr

	ldr	r0,=oldSWIHandler
	ldr	r0,[r0]
	ldr	r1,=SWI_Location
	str	r0,[r1]

	mov	pc,lr
#endif /* USE_IRQ && USE_HIMO */




//; # -------------------------------------------------------------------------
//; # data section used by startup code

	.data

//; # -------------------------------------------------------------------------
//; # Temporary interrupt stack

	.section ".bss"

#if defined(USE_IRQ) && defined(USE_HIMO)
HIMO_IRQ_Address:
	.long	0
#endif /* USE_IRQ && USE_HIMO */

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
