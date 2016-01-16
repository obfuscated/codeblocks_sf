// #========================================================================
// #
// # vectors.S
// #
// # ARM exception vectors (phyCORE_AT91M55800)
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

#define SWI_Location		0x28	/* HiMo: address of SWI handler */
#define IRQ_Location		0x38	/* HiMo: address of IRQ handler */
#define FIQ_Location		0x3C	/* HiMo: address of FIQ handler */

//;
//; # Platform specific definition for phyCORE-ARM7/AT91M55800 board
//;

/*------------------------------------------------*/
/* Advanced Power Management Controller Registers */
/*------------------------------------------------*/
#define APMC_BASE		0xFFFF4000
/* Clock Generator Mode Register */
#define APMC_CGMR		(APMC_BASE + 0x20)
/* Status Register */
#define APMC_SR			(APMC_BASE + 0x30)

#define APMC_MOSCS		0x01
#define APMC_PLL_LOCK		0x02


/*----------------------------------*/
/* External Bus Interface Registers */
/*----------------------------------*/
#define EBI_BASE		0xFFE00000

/*-----------------------------------------*/
/* Advanced Interrupt Controller Registers */
/*-----------------------------------------*/
#define AIC_BASE		0xFFFFF000
/* Interrupt Disable Command Register */
#define AIC_IDCR		(AIC_BASE + 0x124)
/* IRQ Vector Register */
#define AIC_IVR			(AIC_BASE + 0x100)
#define AIC_IVR_OFFS		0x100
/* FIQ Vector Register */
#define AIC_FVR			(AIC_BASE + 0x104)
#define AIC_FVR_OFFS		0x104
/* End of Interrupt Command Register */
#define AIC_EOICR		(AIC_BASE + 0x130)
#define AIC_EOICR_OFFS		0x130




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

	ldr	r0,=APMC_BASE
	ldr	r1,[r0,#(APMC_SR-APMC_BASE)]
	cmp	r1,#(APMC_MOSCS+APMC_PLL_LOCK)
	beq	20f				/* low level init already done */
	ldr	r1,=0x002F0002			/* MOSCEN = 1, OSCOUNT = 47 (1.4ms/30µs) */
	str	r1,[r0,#(APMC_CGMR-APMC_BASE)]
02:
	ldr	r1,[r0,#(APMC_SR-APMC_BASE)]
	ands	r1,r1,#APMC_MOSCS
	beq	02b				/* wait until oscillator is stabilized */
	ldr	r1,=0x002F4002			/* switch from slow clock to main osc. */
	str	r1,[r0,#(APMC_CGMR-APMC_BASE)]
	ldr	r1,=0x032F4102			/* setup PLL (MUL=1,PLLCOUNT=3,CSS=1) */
	str	r1,[r0,#(APMC_CGMR-APMC_BASE)]
04:
	ldr	r1,[r0,#(APMC_SR-APMC_BASE)]
	ands	r1,r1,#APMC_PLL_LOCK
	beq	04b				/* wait until PLL is stabilized */
	ldr	r1,=0x032F8102			/* setup PLL for 32.768 MHz */
	str	r1,[r0,#(APMC_CGMR-APMC_BASE)]
10:
	ldr	lr,=segment_register_setups_end
	adr	r0,segment_register_setups
	ldmia	r0,{r1-r10}
	ldr	r0,=EBI_BASE
	stmia	r0,{r1-r10}
	mov	pc,lr				/* now run critical jump code */
segment_register_setups_end:
	b	20f
segment_register_setups:
	.long	0x01002529	/* EBI_CSR_0 */
#if 0
	.long	0x04003525	/* EBI_CSR_1: 2 TDF + 2 WS */
#else
	.long	0x04003121	/* EBI_CSR_1: 0 TDF + 1 WS */
#endif
	.long	0x02003431	/* EBI_CSR_2 */
	.long	0x30000000	/* EBI_CSR_3 */
	.long	0x40000000	/* EBI_CSR_4 */
	.long	0x50000000	/* EBI_CSR_5 */
	.long	0x60000000	/* EBI_CSR_6 */
	.long	0x70000000	/* EBI_CSR_7 */
	.long	0x00000001	/* Remap Control */
	.long	0x00000010	/* Memory Control */
20:


warm_reset:
	ldr	r1,=AIC_IDCR
	ldr	r0,=0xFF9FFFFF	/* disable all interrupts */
	str	r0,[r1]

//
//; # copy the vector table (__vector_table_start .. __vector_table_end) to address 0
//
#ifndef USE_HIMO
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
#if defined(USE_FIQ)
//; replace FIQ handler by our own handler
	ldr	r1,=FIQ_Location
	ldr	r0,=HIMO_FIQ_Address
	ldr	r2,[r1]
	str	r2,[r0]
	ldr	r2,=FIQ
	str	r2,[r1]
#endif /* USE_FIQ */
#endif /* USE_HIMO */

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


//	; # switch to user mode, evtl. IRQs and FIQs enabled
#ifdef USE_IRQ
#ifdef USE_FIQ
	mov	r0,#(CPSR_USER_MODE)
#else
	mov	r0,#(CPSR_FIQ_DISABLE|CPSR_USER_MODE)
#endif /* USE_FIQ */
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
#ifdef USE_FIQ
	sub	lr,lr,#4		//; adjust return address before saving it
	str	lr,[sp,#-4]!
	mrs	lr,spsr			//; and status bits
	stmfd	sp!,{r0-r3,lr}		//; save APCS working register and SPSR
					//; r12 is a banked register in FIQ mode


//;- Write in the FVR to support Protect Mode
//;- No effect in Normal Mode
//;- De-assert the NIRQ and clear the source in Protect Mode
	ldr	r0,=AIC_BASE
	ldr	r1,[r0,#AIC_FVR_OFFS]	//; load the vector routine
	str	r0,[r0,#AIC_FVR_OFFS]

//	; switch to another mode (to avoid  problem with C language handler code)
//	; SVC mode if interrupted service else SYSTEM mode (interrupted task)
	mrs	r0,cpsr			//; read the current status register
	bic	r0,r0,#CPSR_MODE_BITS	//; clear the mode bits
	and	lr,lr,#CPSR_MODE_BITS	//; extract the mode bits of interrupted state
	cmp	lr,#CPSR_SVC_MODE
	orreq	r0,r0,#CPSR_SVC_MODE	//; switch to SVC mode
	orrne	r0,r0,#CPSR_SYSTEM_MODE	//; switch to SYSTEM mode
	msr	cpsr_cf,r0

	stmfd	sp!,{r12,lr}		//; save original lr (lr_svc|lr) and r12_fiq
	adr	lr,Back_FIQ_Handler	//; set the return address
//	; jump to the correct handler
	movs	r1,r1
	movne	pc,r1


Back_FIQ_Handler:
//	; now read-modify-write the CPSR to disable interrupts
	mrs	r0,cpsr			//; read the status register
	orr	r0,r0,#CPSR_FIQ_DISABLE	//; set the FIQ disable bit
	msr	cpsr_cf,r0		//; write it back to disable interrupts

	ldmfd	sp!,{r12,lr}		//; restore original lr (lr_svc|lr) and r12_fiq
//	; and switch back to FIQ mode
	mrs	r1,cpsr			//; read the status register
	bic	r1,r1,#CPSR_MODE_BITS	//; clear the mode bits
	orr	r1,r1,#CPSR_FIQ_MODE	//; switch to FIQ mode
	msr	cpsr_cf,r1		//; write it back

	ldmfd	sp!,{r0-r3,lr}		//; restore APCS working register and SPSR
	msr	spsr_cf,lr
	ldmfd	sp!,{pc}^		//; and return from interrupt and restore CPSR
#else
	b	FIQ
#endif /* USE_FIQ */



IRQ:
#ifdef USE_IRQ
	sub	lr,lr,#4		//; adjust return address before saving it
	str	lr,[sp,#-4]!
	mrs	lr,spsr			//; and status bits
	stmfd	sp!,{r0-r3,r12,lr}	//; save APCS working register and SPSR


//;- Write in the IVR to support Protect Mode
//;- No effect in Normal Mode
//;- De-assert the NIRQ and clear the source in Protect Mode
	ldr	r0,=AIC_BASE
	ldr	r12,[r0,#AIC_IVR_OFFS]	//; load the vector routine
	str	r0,[r0,#AIC_IVR_OFFS]

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
#if 1
//	; now read-modify-write the CPSR to enable interrupts
	mrs	r0,cpsr			//; read the status register
	bic	r0,r0,#CPSR_IRQ_DISABLE	//; clear the IRQ disable bit
	msr	cpsr_cf,r0		//; reenable interrupts
#endif
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

	ldr	r1,=AIC_BASE		//; signal end of interrupt to AIC
	str	r1,[r1,#AIC_EOICR_OFFS]

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
#ifdef USE_FIQ
	ldr	r0,=HIMO_FIQ_Address
	ldr	r0,[r0]
	ldr	r1,=FIQ_Location
	str	r0,[r1]
#endif /* USE_FIQ */

//; restore HIMO's SWI handler if it's a PXROS application
	ldr	r0,=PxPrepareInit
	cmp	r0,#0
	moveq	pc,lr

	ldr	r0,=oldSWIHandler
	ldr	r0,[r0]
	ldr	r1,=SWI_Location
	str	r0,[r1]

	mov	pc,lr
#endif /* USE_HIMO && USE_HIMO */




//; # -------------------------------------------------------------------------
//; # data section used by startup code

	.data

//; # -------------------------------------------------------------------------
//; # Temporary interrupt stack

	.section ".bss"

#if defined(USE_IRQ) && defined(USE_HIMO)
HIMO_IRQ_Address:
	.long	0
#if defined(USE_FIQ)
HIMO_FIQ_Address:
	.long	0
#endif /* USE_FIQ */
#endif /* USE_HIMO && USE_HIMO */

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
#define USER_STACK_SIZE	2048
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
