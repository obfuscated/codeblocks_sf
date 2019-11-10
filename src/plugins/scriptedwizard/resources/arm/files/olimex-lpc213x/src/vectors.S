// #========================================================================
// #
// # vectors.S
// #
// # ARM exception vectors (Olimex LPC-P2138)
// #
// #
// # Copyright HighTec EDV-Systeme GmbH 1982-2007
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

#define SWI_Location		0x28	/* HiMo: address of SWI handler */
#define IRQ_Location		0x38	/* HiMo: address of IRQ handler */

//;
//; # Platform specific definition for LPC213x
//;

/*-----------------------*/
/* PLL Control Registers */
/*-----------------------*/
#define SCB_BASE	0xE01FC000
#define PLL_BASE	(SCB_BASE + 0x080)

/* PLL Control Register */
#define PLLCON		(SCB_BASE + 0x80)
/* PLL Configuration Register */
#define PLLCFG		(SCB_BASE + 0x84)
/* PLL Status Register */
#define PLLSTAT		(SCB_BASE + 0x88)
/* PLL Feed Register */
#define PLLFEED		(SCB_BASE + 0x8C)

#define PLLCON_PLLE	0x01	/* PLL Enable */
#define PLLCON_PLLC	0x02	/* PLL Connect */

#define PLLCFG_VAL	0x23	/* P=2,M=4 */

#define PLL_PLLC	0x100	/* Status of PLL Enable */
#define PLL_PLLE	0x200	/* Status of PLL Connect */
#define PLL_PLOCK	0x400	/* PLL Lock Status */

#define PLLFEED_PW0	0xAA
#define PLLFEED_PW1	0x55

/* Memory Mapping Control Register */
#define MEMMAP		(SCB_BASE + 0x40)

#define MEMMAP_BOOT	0x00	/* vectors mapped to Boot Block */
#define MEMMAP_FLASH	0x01	/* vectors at 0 (internal Flash) */
#define MEMMAP_ISRAM	0x02	/* vectors mapped to internal SRAM */

#define ISRAM_START	0x40000000

/* APB Divider Register */
#define APBDIV		(SCB_BASE + 0x100)

#define APBDIV_4	0	/* peripherals clock == CPU/4 clock */
#define APBDIV_2	2	/* peripherals clock == CPU/2 clock */
#define APBDIV_1	1	/* peripherals clock == CPU clock */


/*-----------------------------------------*/
/* Vectored Interrupt Controller Registers */
/*-----------------------------------------*/
#define VIC_BASE	0xFFFFF000

/* Interrupt Enable Clear Register */
#define VICIEC		(VIC_BASE + 0x14)
/* IRQ Vector Register */
#define VICVADDR	(VIC_BASE + 0x30)

#define ISR_VALID_MASK	0x003FFFFF


//; #==========================================================================
//; #  Hardware exception vectors.
//; #  The vector table will be copied to location 0x0000 at startup time.
//;
	.code	32
	.section ".vectors","ax"
	.global	__exception_handlers

//; # LPC2xxx boot code looks for a valid vector table at start of flash.
//; # A vector table is valid if the word sum over the 8 vector entries is zero.
//; # The unused word at location 0x14 is used for adjusting the checksum.
__exception_handlers:
__vector_table_start:
	ldr	pc,.reset_vector
	ldr	pc,.undefined_instruction
	ldr	pc,.software_interrupt
	ldr	pc,.abort_prefetch
	ldr	pc,.abort_data
	.word	0xB8A06F58	//; ==> checksum = 0
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

	ldr	r0,=SCB_BASE
	ldr	r1,[r0,#(PLLSTAT-SCB_BASE)]
	ldr	r2,=(PLL_PLOCK + PLL_PLLE + PLL_PLLC)
	and	r1,r1,r2
	cmp	r1,#(PLL_PLOCK + PLL_PLLE + PLL_PLLC)
	beq	10f				/* clock is already running */

	ldr	r1,=PLLCFG_VAL
	str	r1,[r0,#(PLLCFG-SCB_BASE)]

	ldr	r1,=PLLCON_PLLE			/* PLLCON.0 = 0x01 -> PLL enabled */
	str	r1,[r0,#(PLLCON-SCB_BASE)]

	ldr	r1,=PLLFEED_PW0			/* write feed sequence */
	str	r1,[r0,#(PLLFEED-SCB_BASE)]
	ldr	r1,=PLLFEED_PW1
	str	r1,[r0,#(PLLFEED-SCB_BASE)]
02:
	ldr	r1,[r0,#(PLLSTAT-SCB_BASE)]
	ands	r1,r1,#PLL_PLOCK
	beq	02b				/* wait until oscillator is stabilized */

	ldr	r1,=(PLLCON_PLLE+PLLCON_PLLC)	/* PLLCON.0 = 1, PLLCON.1 = 1 */
	str	r1,[r0,#(PLLCON-SCB_BASE)]

	ldr	r1,=PLLFEED_PW0			/* write feed sequence */
	str	r1,[r0,#(PLLFEED-SCB_BASE)]
	ldr	r1,=PLLFEED_PW1
	str	r1,[r0,#(PLLFEED-SCB_BASE)]

04:
	ldr	r1,[r0,#(PLLSTAT-SCB_BASE)]
	ands	r1,r1,#PLL_PLOCK
	beq	04b				/* wait until PLL is stabilized */

10:
	mov	r1,#APBDIV_1			/* peripherals clock == CPU clock */
	str	r1,[r0,#(APBDIV-SCB_BASE)]

warm_reset:
	ldr	r1,=VICIEC
	ldr	r0,=ISR_VALID_MASK		/* disable all interrupts */
	str	r0,[r1]

//
//; # copy the vector table (__vector_table_start .. __vector_table_end) to internal SRAM
//
#ifndef USE_HIMO
//; #  HiMo needs its own exception handlers --> don't overwrite these!!
	mov	r8,#ISRAM_START
	ldr	r9,=__exception_handlers
	ldmia	r9!,{r0-r7}
	stmia	r8!,{r0-r7}
	ldmia	r9!,{r0-r7}
	stmia	r8!,{r0-r7}
#endif /* USE_HIMO */

//; then remap vectors to point to internal SRAM
	ldr	r0,=SCB_BASE
	mov	r1,#MEMMAP_ISRAM		/* map vectors to internal SRAM */
	str	r1,[r0,#(MEMMAP-SCB_BASE)]

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


	ldr	r0,=VIC_BASE
	ldr	r12,[r0,#(VICVADDR-VIC_BASE)]	//; load the vector routine

//	; switch to another mode (to avoid  problem with C language handler code)
//	; SVC mode if interrupted service else SYSTEM mode (interrupted task)
	mrs	r0,cpsr			//; read the current status register
	bic	r0,r0,#CPSR_MODE_BITS	//; clear the mode bits
	and	lr,lr,#CPSR_MODE_BITS	//; extract the mode bits of interrupted state
	cmp	lr,#CPSR_SVC_MODE
	orreq	r0,r0,#CPSR_SVC_MODE	//; switch to SVC mode
	orrne	r0,r0,#CPSR_SYSTEM_MODE	//; switch to SYSTEM mode
	msr	cpsr_cf,r0

	stmfd	sp!,{lr}		//; save original lr (lr_svc|lr)
	adr	lr,Back_From_Handler	//; set the return address

//	; now read-modify-write the CPSR to enable interrupts
	mrs	r0,cpsr			//; read the status register
	bic	r0,r0,#CPSR_IRQ_DISABLE	//; clear the IRQ disable bit
	msr	cpsr_cf,r0		//; reenable interrupts

//	; jump to the correct handler
	movs	r12,r12
	movne	pc,r12


Back_From_Handler:
//	; now read-modify-write the CPSR to disable interrupts
	mrs	r0,cpsr			//; read the status register
	orr	r0,r0,#CPSR_IRQ_DISABLE	//; set the IRQ disable bit
	msr	cpsr_cf,r0		//; write it back to disable interrupts

	ldmfd	sp!,{lr}		//; restore original lr (lr_svc|lr)
//	; and switch back to IRQ mode
	mrs	r12,cpsr		//; read the status register
	bic	r12,r12,#CPSR_MODE_BITS	//; clear the mode bits
	orr	r12,r12,#CPSR_IRQ_MODE	//; switch to IRQ mode
	msr	cpsr_cf,r12		//; write it back

	ldr	r1,=VIC_BASE		//; signal end of interrupt to VIC
	str	r1,[r1,#(VICVADDR-VIC_BASE)]

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
	.rept	16
	.long	0
	.endr
__FIQ_exception_stack:
	.rept	16
	.long	0
	.endr
__exception_stack:

//; # Runtime stack used during all IRQ interrupt processing
#ifndef IRQ_STACK_SIZE
#ifdef USE_IRQ
#define IRQ_STACK_SIZE		512
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
#define STARTUP_STACK_SIZE	512
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
#define USER_STACK_SIZE		512
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
