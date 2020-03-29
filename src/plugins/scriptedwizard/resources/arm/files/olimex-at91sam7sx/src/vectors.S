// #========================================================================
// #
// # vectors.S
// #
// # ARM exception vectors (AT91SAM7Sx)
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
//; # Platform specific definition for AT91SAM7Sx controller
//;

/*----------------*/
/* Watchdog Timer */
/*----------------*/
#define WDT_BASE		0xFFFFFD40
/* Watchdog Mode Register */
#define WDT_MR			(WDT_BASE + 0x04)

#define WDT_MR_WDDIS		0x00008000	/* Watchdog Disable */

/*-------------------*/
/* Memory Controller */
/*-------------------*/
#define MC_BASE			0xFFFFFF00
/* Remap Control Register */
#define MC_RCR			(MC_BASE + 0x00)
/* Flash Mode Register */
#define MC_FMR			(MC_BASE + 0x60)

#define MC_RCR_RCB		0x00000001	/* Remap Command Bit */

#define MC_FMR_FWS_1FWS		0x00000100	/* 2 cycle for Read, 3 for Write operations */
#define MC_FMR_FMCN		0x00FF0000	/* Flash Microsecond Cycle Number */
#define MC_FMR_INIT_VAL		0x00320100

#define FLASH_START		0x00100000	/* start of internal Flash */

/*---------------------------------------*/
/* Power Management Controller Registers */
/*---------------------------------------*/
#define PMC_BASE		0xFFFFFC00
/* Main Oscillator Register */
#define PMC_MOR			(PMC_BASE + 0x20)
/* Main Clock Frequency Register */
#define PMC_MCFR		(PMC_BASE + 0x24)
/* PLL Register */
#define PMC_PLLR		(PMC_BASE + 0x2C)
/* Master Clock Register */
#define PMC_MCKR		(PMC_BASE + 0x30)
/* Status Register */
#define PMC_SR			(PMC_BASE + 0x68)

#define PMC_MOR_MOSCEN		0x00000001	/* Main Oscillator Enable */
#define PMC_MOR_OSCBYPASS	0x00000002	/* Main Oscillator Bypass */
#define PMC_MOR_OSCCOUNT	0x0000FF00	/* Main Oscillator Start-up Time */

#define PMC_MOR_INIT_VAL	0x00000601

#define PMC_MCFR_MAINF		0x0000FFFF	/* Main Clock Frequency */
#define PMC_MCFR_MAINRDY	0x00010000	/* Main Clock Ready */

#define PMC_PLLR_DIV		0x000000FF	/* Divider Selected */
#define PMC_PLLR_DIV_0		0x00000000	/* Divider output is 0 */
#define PMC_PLLR_DIV_BYPASS	0x00000001	/* Divider is bypassed */
#define PMC_PLLR_PLLCOUNT	0x00003F00	/* PLL Counter */
#define PMC_PLLR_OUT		0x0000C000	/* PLL Output Frequency Range */
#define PMC_PLLR_MUL		0x07FF0000	/* PLL Multiplier */
#define PMC_PLLR_USBDIV		0x30000000	/* Divider for USB Clocks */

#define PMC_PLLR_INIT_VAL	0x007C1C18	/* (MUL+1)/DIV * fOSC ==> 95.953125 MHz */

#define PMC_MCKR_CSS_PLL	0x00000003	/* Clock from PLL is selected */
#define PMC_MCKR_PRES_DIV2	0x00000004	/* Selected Clock divided by 2 */

#define PMC_MCKR_INIT_VAL	0x00000007

#define PMC_CK_MOSC		0x00000001	/* MOSC Status */
#define PMC_CK_LOCK		0x00000004	/* PLL Status */
#define PMC_CK_MCKRDY		0x00000008	/* MCK_RDY Status */


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

	ldr	r0,=WDT_BASE
	ldr	r1,=(WDT_MR_WDDIS)
	str	r1,[r0,#(WDT_MR-WDT_BASE)]	//;/* disable watchdog */

	ldr	r0,=MC_BASE
	ldr	r1,=MC_FMR_INIT_VAL
	str	r1,[r0,#(MC_FMR-MC_BASE)]	//;/* configure Flash */

	//;/* check if remapping of SRAM must be done */
	ldr	r2,=FLASH_START
	ldr	r2,[r2]				//;/* read first word from Flash */
	ldr	r3,=0
	ldr	r3,[r3]				//;/* read first word at 0 */
	cmp	r3,r2
	ldr	lr,=remap_end
	ldreq	r1,=MC_RCR_RCB
	streq	r1,[r0,#(MC_RCR-MC_BASE)]	//;/* remap SRAM to 0 */
	mov	pc,lr				//;/* continue at remapped address */
remap_end:

	ldr	r0,=PMC_BASE
	ldr	r1,=PMC_MOR_INIT_VAL
	str	r1,[r0,#(PMC_MOR-PMC_BASE)]

02:
	ldr	r1,[r0,#(PMC_SR-PMC_BASE)]
	ands	r1,r1,#PMC_CK_MOSC
	beq	02b				//;/* wait until oscillator is stabilized */

	ldr	r1,=PMC_PLLR_INIT_VAL		//;/* setup PLL */
	str	r1,[r0,#(PMC_PLLR-PMC_BASE)]
04:
	ldr	r1,[r0,#(PMC_SR-PMC_BASE)]
	ands	r1,r1,#PMC_CK_LOCK
	beq	04b				//;/* wait until PLL is stabilized */
06:
	ldr	r1,[r0,#(PMC_SR-PMC_BASE)]
	ands	r1,r1,#PMC_CK_MCKRDY
	beq	06b				//;/* wait until PLL is stabilized */

	ldr	r1,=PMC_MCKR_INIT_VAL		//;/* setup PLL for 48 MHz */
	str	r1,[r0,#(PMC_MCKR-PMC_BASE)]

07:
	ldr	r1,[r0,#(PMC_SR-PMC_BASE)]
	ands	r1,r1,#PMC_CK_MCKRDY
	beq	07b				//;/* wait until PLL is stabilized */



warm_reset:
	ldr	r1,=AIC_IDCR
	ldr	r0,=0xC0007FF7			//;/* disable all interrupts */
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
	ldr	r0,=IrqInit
	mov	lr,pc
	bx	r0
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
#endif /* !__NO_CTOR_DTOR_SUPPORT__ */

	mov	a1,#0		//; # set argc to 0
	mov	a2,#0		//; # and argv to NUL
	ldr	r2,=main
	mov	lr,pc
	bx	r2
#ifdef USE_HIMO
#ifdef USE_IRQ
	ldr	r1,=IrqInit	//; # stop interrupts
	mov	lr,pc
	bx	r1
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
#endif /* USE_HIMO */




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
#endif /* USE_HIMO */

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
#define IRQ_STACK_SIZE		64
#endif
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
