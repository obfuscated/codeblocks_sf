/*====================================================================
* Project: Board Support Package (BSP)
* Developed using:
* Function: Standard definitions for ATMEL ARM7TDMI controller family
*           (AT91SAM7S32, AT91SAM7S64, AT91SAM7S128, AT91SAM7S256)
*
* Copyright HighTec EDV-Systeme GmbH 1982-2006
*====================================================================*/

#ifndef __AT91SAM7SX_H__
#define __AT91SAM7SX_H__

/* general register definition macro */
#define __REG32(x)	((volatile unsigned int *)(x))
#define __REG16(x)	((volatile unsigned short *)(x))
#define __REG8(x)	((volatile unsigned char *)(x))


/*-------------------------------------*/
/* Peripheral and Interrupt Identifier */
/*-------------------------------------*/

#define FIQ_ID		 0		/* Fast Interrupt */
#define SYS_ID		 1		/* System Peripheral interrupt */
#define PIOA_ID		 2		/* Parallel I/O Controller A interrupt */
#define RES3_ID		 3		/* Reserved */
#define ADC_ID		 4		/* Analog to Digital Converter interrupt */
#define SPI_ID		 5		/* Serial Peripheral Interface interrupt */
#define US0_ID		 6		/* USART Channel 0 interrupt */
#define US1_ID		 7		/* USART Channel 1 interrupt */
#define SSC_ID		 8		/* Synchronous Serial Controller interrupt */
#define TWI_ID		 9		/* Two-Wire Interface interrupt */
#define PWMC_ID		10		/* PWM Controller interrupt */
#define UDP_ID		11		/* USB Device Port interrupt */
#define TC0_ID		12		/* Timer Channel 0 interrupt */
#define TC1_ID		13		/* Timer Channel 1 interrupt */
#define TC2_ID		14		/* Timer Channel 2 interrupt */
#define RES15_ID	15		/* Reserved */
#define RES16_ID	16		/* Reserved */
#define RES17_ID	17		/* Reserved */
#define RES18_ID	18		/* Reserved */
#define RES19_ID	19		/* Reserved */
#define RES20_ID	20		/* Reserved */
#define RES21_ID	21		/* Reserved */
#define RES22_ID	22		/* Reserved */
#define RES23_ID	23		/* Reserved */
#define RES24_ID	24		/* Reserved */
#define RES25_ID	25		/* Reserved */
#define RES26_ID	26		/* Reserved */
#define RES27_ID	27		/* Reserved */
#define RES28_ID	28		/* Reserved */
#define RES29_ID	29		/* Reserved */
#define IRQ0_ID		30		/* External interrupt 0 */
#define IRQ1_ID		31		/* External interrupt 1 */

#define ISR_VALID_MASK	0xC0007FF7

#define ISR_MIN		 0
#define ISR_MAX		31
#define ISR_COUNT	31

#define MAXIRQNUM	(ISR_MAX)
#define MAXFIQNUM	(MAXIRQNUM)
#define MAXSWINUM	15

#define NR_IRQS		(MAXIRQNUM + 1)


/*---------------------------------------*/
/* Power Management Controller Registers */
/*---------------------------------------*/
#define PMC_BASE	0xFFFFFC00
#define PMC_SCER	__REG32(PMC_BASE + 0x00)	/* System Clock Enable  Register */
#define PMC_SCDR	__REG32(PMC_BASE + 0x04)	/* System Clock Disable Register */
#define PMC_SCSR	__REG32(PMC_BASE + 0x08)	/* System Clock Status  Register */
#define PMC_PCER	__REG32(PMC_BASE + 0x10)	/* Peripheral Clock Enable  Register */
#define PMC_PCDR	__REG32(PMC_BASE + 0x14)	/* Peripheral Clock Disable Register */
#define PMC_PCSR	__REG32(PMC_BASE + 0x18)	/* Peripheral Clock Status  Register */
#define PMC_MOR		__REG32(PMC_BASE + 0x20)	/* Main Oscillator Register */
#define PMC_MCFR	__REG32(PMC_BASE + 0x24)	/* Main Clock Frequency Register */
#define PMC_PLLR	__REG32(PMC_BASE + 0x2C)	/* PLL Register */
#define PMC_MCKR	__REG32(PMC_BASE + 0x30)	/* Master Clock Register */
#define PMC_PCKR0	__REG32(PMC_BASE + 0x40)	/* Programmable Clock Register 0 */
#define PMC_PCKR1	__REG32(PMC_BASE + 0x44)	/* Programmable Clock Register 1 */
#define PMC_PCKR2	__REG32(PMC_BASE + 0x48)	/* Programmable Clock Register 2 */
#define PMC_IER		__REG32(PMC_BASE + 0x60)	/* Interrupt Enable Register */
#define PMC_IDR		__REG32(PMC_BASE + 0x64)	/* Interrupt Disable Register */
#define PMC_SR		__REG32(PMC_BASE + 0x68)	/* Status Register */
#define PMC_IMR		__REG32(PMC_BASE + 0x6C)	/* Interrupt Mask Register */

/* bits in System Clock Registers (PMC_SCER, PMC_SCDR, PMC_SCSR) */
#define PMC_SCR_PCK				(1 <<  0)	/* Processor Clock */
#define PMC_SCR_UDP				(1 <<  7)	/* USB Device Port Clock */
#define PMC_SCR_PCK0			(1 <<  8)	/* Programmable Clock Output 0 */
#define PMC_SCR_PCK1			(1 <<  9)	/* Programmable Clock Output 1 */
#define PMC_SCR_PCK2			(1 << 10)	/* Programmable Clock Output 2 */

/* bits in PMC_MCKR: Master Clock Register */
#define PMC_MCKR_CSS			(0x03 << 0)	/* Programmable Clock Selection */
#define PMC_MCKR_CSS_SLOW		(0x00 << 0)	/* Slow Clock is selected */
#define PMC_MCKR_CSS_MAIN		(0x01 << 0)	/* Main Clock is selected */
#define PMC_MCKR_CSS_PLL		(0x03 << 0)	/* Clock from PLL is selected */
#define PMC_MCKR_PRES			(0x07 << 2)	/* Programmable Clock Prescaler */
#define PMC_MCKR_PRES_NONE		(0x00 << 2)	/* Selected Clock */
#define PMC_MCKR_PRES_DIV2		(0x01 << 2)	/* Selected Clock divided by 2 */
#define PMC_MCKR_PRES_DIV4		(0x02 << 2)	/* Selected Clock divided by 4 */
#define PMC_MCKR_PRES_DIV8		(0x03 << 2)	/* Selected Clock divided by 8 */
#define PMC_MCKR_PRES_DIV16		(0x04 << 2)	/* Selected Clock divided by 16 */
#define PMC_MCKR_PRES_DIV32		(0x05 << 2)	/* Selected Clock divided by 32 */
#define PMC_MCKR_PRES_DIV64		(0x06 << 2)	/* Selected Clock divided by 64 */

/* bits in PMC_PCKx: Programmable Clock Register */
#define PMC_PCK_CSS				(0x03 << 0)	/* Programmable Clock Selection */
#define PMC_PCK_CSS_SLOW		(0x00 << 0)	/* Slow Clock is selected */
#define PMC_PCK_CSS_MAIN		(0x01 << 0)	/* Main Clock is selected */
#define PMC_PCK_CSS_PLL			(0x03 << 0)	/* Clock from PLL is selected */
#define PMC_PCK_PRES			(0x07 << 2)	/* Programmable Clock Prescaler */
#define PMC_PCK_PRES_SHFT		2
#define PMC_PCK_PRES_NONE		(0x00 << 2)	/* Selected Clock */
#define PMC_PCK_PRES_DIV2		(0x01 << 2)	/* Selected Clock divided by 2 */
#define PMC_PCK_PRES_DIV4		(0x02 << 2)	/* Selected Clock divided by 4 */
#define PMC_PCK_PRES_DIV8		(0x03 << 2)	/* Selected Clock divided by 8 */
#define PMC_PCK_PRES_DIV16		(0x04 << 2)	/* Selected Clock divided by 16 */
#define PMC_PCK_PRES_DIV32		(0x05 << 2)	/* Selected Clock divided by 32 */
#define PMC_PCK_PRES_DIV64		(0x06 << 2)	/* Selected Clock divided by 64 */

/* bits in PMC_IER, PMC_IDR, PMC_SR, PMC_IMR */
#define PMC_CK_MOSC				(1 <<  0)	/* MOSC Status */
#define PMC_CK_LOCK				(1 <<  2)	/* PLL Status */
#define PMC_CK_MCKRDY			(1 <<  3)	/* MCK_RDY Status */
#define PMC_CK_PCK0RDY			(1 <<  8)	/* PCK0_RDY Status */
#define PMC_CK_PCK1RDY			(1 <<  9)	/* PCK1_RDY Status */
#define PMC_CK_PCK2RDY			(1 << 10)	/* PCK2_RDY Status */

/*--------------------------------------*/
/* Clock Generator Controller Registers */
/*--------------------------------------*/
#define CKGR_BASE	(PMC_BASE + 0x20)
#define CKGR_MOR	__REG32(CKGR_BASE + 0x00)	/* Main Oscillator Register */
#define CKGR_MCFR	__REG32(CKGR_BASE + 0x04)	/* Main Clock Frequency Register */
#define CKGR_PLLR	__REG32(CKGR_BASE + 0x0C)	/* PLL Register */

/* CKGR_MOR: Main Oscillator Register */
#define CKGR_MOR_MOSCEN			(1 << 0)	/* Main Oscillator Enable */
#define CKGR_MOR_OSCBYPASS		(1 << 1)	/* Main Oscillator Bypass */
#define CKGR_MOR_OSCCOUNT		(0xFF << 8)	/* Main Oscillator Start-up Time */

/* CKGR_MCFR: Main Clock Frequency Register */
#define CKGR_MCFR_MAINF			(0xFFFF << 0)	/* Main Clock Frequency */
#define CKGR_MCFR_MAINRDY		(1 << 16)		/* Main Clock Ready */

/* CKGR_PLLR: PLL Register */
#define CKGR_PLLR_DIV			(0xFF << 0)		/* Divider Selected */
#define CKGR_PLLR_DIV_0			(0x00)			/* Divider output is 0 */
#define CKGR_PLLR_DIV_BYPASS	(0x01)			/* Divider is bypassed */
#define CKGR_PLLR_PLLCOUNT		(0x3F << 8)		/* PLL Counter */
#define CKGR_PLLR_OUT			(0x03 << 14)	/* PLL Output Frequency Range */
#define CKGR_PLLR_MUL			(0x7FF << 16)	/* PLL Multiplier */
#define CKGR_PLLR_USBDIV		(0x03 << 28)	/* Divider for USB Clocks */
#define CKGR_PLLR_USBDIV_0		(0x01 << 28)	/* USB Clock == PLL */
#define CKGR_PLLR_USBDIV_2		(0x01 << 28)	/* USB Clock == PLL / 2 */
#define CKGR_PLLR_USBDIV_4		(0x02 << 28)	/* USB Clock == PLL / 4 */


/*----------------------------*/
/* Reset Controller Interface */
/*----------------------------*/
#define RSTC_BASE	0xFFFFFD00
#define RSTC_CR		__REG32(RSTC_BASE + 0x00)	/* Control Register */
#define RSTC_SR		__REG32(RSTC_BASE + 0x04)	/* Status Register */
#define RSTC_MR		__REG32(RSTC_BASE + 0x08)	/* Mode Register */

/* RSTC_CR: Reset Controller Control Register */
#define RSTC_CR_PROCRST		(1 << 0)		/* Processor Reset */
#define RSTC_CR_PERRST		(1 << 2)		/* Peripheral Reset */
#define RSTC_CR_EXTRST		(1 << 3)		/* External Reset */
#define RSTC_CR_KEY			(0xFFu << 24)	/* Password */

/* RSTC_SR: Reset Controller Status Register */
#define RSTC_SR_URSTS			(1 << 0)	/* User Reset Status */
#define RSTC_SR_BODSTS			(1 << 1)	/* Brownout Reset Status */
#define RSTC_SR_RSTTYP			(0x07 << 8)	/* Reset Type */
#define RSTC_SR_RSTTYP_POWERUP	(0 << 8)	/* Power-up Reset */
#define RSTC_SR_RSTTYP_WAKEUP	(1 << 8)	/* WakeUp Reset */
#define RSTC_SR_RSTTYP_WATCHDOG	(2 << 8)	/* Watchdog Reset */
#define RSTC_SR_RSTTYP_SOFTWARE	(3 << 8)	/* Software Reset */
#define RSTC_SR_RSTTYP_USER		(4 << 8)	/* User Reset */
#define RSTC_SR_RSTTYP_BROWNOUT	(5 << 8)	/* Brownout Reset */

#define RSTC_SR_NRSTL			(1 << 16)	/* NRST pin level */
#define RSTC_SR_SRCMP			(1 << 17)	/* Software Reset Command in Progress */

/* RSTC_MR: Reset Controller Mode Register */
#define RSTC_MR_URSTEN		(0x01 <<  0)	/* User Reset Enable */
#define RSTC_MR_URSTIEN		(0x01 <<  4)	/* User Reset Interrupt Enable */
#define RSTC_MR_ERSTL		(0x0F <<  8)	/* User Reset Length */
#define RSTC_MR_BODIEN		(0x01 << 16)	/* Brownout Detection Interrupt */

/*-----------------*/
/* Real-time Timer */
/*-----------------*/
#define RTT_BASE	0xFFFFFD20
#define RTT_MR	__REG32(RTT_BASE + 0x00)	/* Real-time Timer Mode Register */
#define RTT_AR	__REG32(RTT_BASE + 0x04)	/* Real-time Timer Alarm Register */
#define RTT_VR	__REG32(RTT_BASE + 0x08)	/* Real-time Timer Value Register */
#define RTT_SR	__REG32(RTT_BASE + 0x0C)	/* Real-time Timer Status Register */

/* RTT_MR: Real-time Timer Mode Register */
#define RTT_MR_RTPRES		(0xFFFF << 0)	/* Real Time Timer Prescaler Value */
#define RTT_MR_ALMIEN		(1 << 16)		/* Alarm Interrupt Enable */
#define RTT_MR_RTTINCIEN	(1 << 17)		/* Real Time Timer Increment Interrupt Enable */
#define RTT_MR_RTTRST		(1 << 18)		/* Real Time Timer Restart */

/* RTT_AR: Real-time Timer Alarm Register */
#define RTT_AR_ALMV			(0x0 << 0)		/* Alarm Value */

/* RTT_VR: Real-time Timer Value Register */
#define RTT_VR_CRTV			(0x0 << 0)		/* Current Real Time Value */

/* RTT_SR: Real-time Timer Status Register */
#define RTT_SR_ALMS			(1 << 0)		/* Alarm Status */
#define RTT_SR_RTTINC		(1 << 1)		/* Real Time Timer Increment */


/*-------------------------*/
/* Periodic Interval Timer */
/*-------------------------*/
#define PIT_BASE	0xFFFFFD30
#define PIT_MR		__REG32(PIT_BASE + 0x00)	/* Period Interval Mode Register */
#define PIT_SR		__REG32(PIT_BASE + 0x04)	/* Period Interval Status Register */
#define PIT_PIVR	__REG32(PIT_BASE + 0x08)	/* Period Interval Value Register */
#define PIT_PIIR	__REG32(PIT_BASE + 0x0C)	/* Period Interval Image Register */

/* PIT_MR: Periodic Interval Timer Mode Register */
#define PIT_MR_PIV			(0xFFFFF << 0)	/* Periodic Interval Value */
#define PIT_MR_PITEN		(1 << 24)		/* Periodic Interval Timer Enabled */
#define PIT_MR_PITIEN		(1 << 25)		/* Periodic Interval Timer Interrupt Enabled */

/* PIT_SR: Periodic Interval Timer Status Register */
#define PIT_SR_PITS			(1 << 0)		/* Periodic Interval Timer Status */

/* PIT_PIVR: Periodic Interval Timer Value Register */
#define PIT_PIVR_CPIV		(0xFFFFF << 0)	/* Current Periodic Interval Value */
#define PIT_PIVR_PICNT		(0xFFFu << 20)	/* Periodic Interval Counter */


/*----------------*/
/* Watchdog Timer */
/*----------------*/
#define WDT_BASE	0xFFFFFD40
#define WDT_CR		__REG32(WDT_BASE + 0x00)	/* Watchdog Control Register */
#define WDT_MR		__REG32(WDT_BASE + 0x04)	/* Watchdog Mode Register */
#define WDT_SR		__REG32(WDT_BASE + 0x08)	/* Watchdog Status Register */

/* WDT_CR: Watchdog Timer Control Register */
#define WDT_CR_WDRSTT		(1 << 0)		/* Watchdog Restart */
#define WDT_CR_KEY			(0xFFu << 24)	/* Watchdog KEY Password */

/* WDT_MR: Watchdog Timer Mode Register */
#define WDT_MR_WDV			(0xFFF << 0)	/* Watchdog Counter Value */
#define WDT_MR_WDFIEN		(1 << 12)		/* Watchdog Fault Interrupt Enable */
#define WDT_MR_WDRSTEN		(1 << 13)		/* Watchdog Reset Enable */
#define WDT_MR_WDRPROC		(1 << 14)		/* Watchdog Reset Processor */
#define WDT_MR_WDDIS		(1 << 15)		/* Watchdog Disable */
#define WDT_MR_WDD			(0xFFF << 16)	/* Watchdog Delta Value */
#define WDT_MR_WDDBGHLT		(1 << 28)		/* Watchdog Debug Halt */
#define WDT_MR_WDIDLEHLT	(1 << 29)		/* Watchdog Idle Halt */

/* WDT_SR: Watchdog Timer Status Register */
#define WDT_SR_WDUNF		(1 << 0)		/* Watchdog Underflow */
#define WDT_SR_WDERR		(1 << 1)		/* Watchdog Error */


/*---------------------------------------------*/
/* Voltage Regulator Mode Controller Registers */
/*---------------------------------------------*/
#define VREG_BASE	0xFFFFFD60
#define VREG_MR		__REG32(VREG_BASE + 0x00)	/* Voltage Regulator Mode Register */

#define VREG_MR_PSTDBY		(1 << 0)		/* Voltage Regulator Power Standby Mode */


/*-----------------------------------------*/
/* Advanced Interrupt Controller Registers */
/*-----------------------------------------*/
#define AIC_BASE	0xFFFFF000
/* Source Mode Register */
#define AIC_SMR0	__REG32(AIC_BASE + 0x000)
#define AIC_SMR1	__REG32(AIC_BASE + 0x004)
#define AIC_SMR2	__REG32(AIC_BASE + 0x008)
#define AIC_SMR3	__REG32(AIC_BASE + 0x00C)
#define AIC_SMR4	__REG32(AIC_BASE + 0x010)
#define AIC_SMR5	__REG32(AIC_BASE + 0x014)
#define AIC_SMR6	__REG32(AIC_BASE + 0x018)
#define AIC_SMR7	__REG32(AIC_BASE + 0x01C)
#define AIC_SMR8	__REG32(AIC_BASE + 0x020)
#define AIC_SMR9	__REG32(AIC_BASE + 0x024)
#define AIC_SMR10	__REG32(AIC_BASE + 0x028)
#define AIC_SMR11	__REG32(AIC_BASE + 0x02C)
#define AIC_SMR12	__REG32(AIC_BASE + 0x030)
#define AIC_SMR13	__REG32(AIC_BASE + 0x034)
#define AIC_SMR14	__REG32(AIC_BASE + 0x038)
#define AIC_SMR15	__REG32(AIC_BASE + 0x03C)
#define AIC_SMR16	__REG32(AIC_BASE + 0x040)
#define AIC_SMR17	__REG32(AIC_BASE + 0x044)
#define AIC_SMR18	__REG32(AIC_BASE + 0x048)
#define AIC_SMR19	__REG32(AIC_BASE + 0x04C)
#define AIC_SMR20	__REG32(AIC_BASE + 0x050)
#define AIC_SMR21	__REG32(AIC_BASE + 0x054)
#define AIC_SMR22	__REG32(AIC_BASE + 0x058)
#define AIC_SMR23	__REG32(AIC_BASE + 0x05C)
#define AIC_SMR24	__REG32(AIC_BASE + 0x060)
#define AIC_SMR25	__REG32(AIC_BASE + 0x064)
#define AIC_SMR26	__REG32(AIC_BASE + 0x068)
#define AIC_SMR27	__REG32(AIC_BASE + 0x06C)
#define AIC_SMR28	__REG32(AIC_BASE + 0x070)
#define AIC_SMR29	__REG32(AIC_BASE + 0x074)
#define AIC_SMR30	__REG32(AIC_BASE + 0x078)
#define AIC_SMR31	__REG32(AIC_BASE + 0x07C)
/* Source Vector Register */
#define AIC_SVR0	__REG32(AIC_BASE + 0x080)
#define AIC_SVR1	__REG32(AIC_BASE + 0x084)
#define AIC_SVR2	__REG32(AIC_BASE + 0x088)
#define AIC_SVR3	__REG32(AIC_BASE + 0x08C)
#define AIC_SVR4	__REG32(AIC_BASE + 0x090)
#define AIC_SVR5	__REG32(AIC_BASE + 0x094)
#define AIC_SVR6	__REG32(AIC_BASE + 0x098)
#define AIC_SVR7	__REG32(AIC_BASE + 0x09C)
#define AIC_SVR8	__REG32(AIC_BASE + 0x0A0)
#define AIC_SVR9	__REG32(AIC_BASE + 0x0A4)
#define AIC_SVR10	__REG32(AIC_BASE + 0x0A8)
#define AIC_SVR11	__REG32(AIC_BASE + 0x0AC)
#define AIC_SVR12	__REG32(AIC_BASE + 0x0B0)
#define AIC_SVR13	__REG32(AIC_BASE + 0x0B4)
#define AIC_SVR14	__REG32(AIC_BASE + 0x0B8)
#define AIC_SVR15	__REG32(AIC_BASE + 0x0BC)
#define AIC_SVR16	__REG32(AIC_BASE + 0x0C0)
#define AIC_SVR17	__REG32(AIC_BASE + 0x0C4)
#define AIC_SVR18	__REG32(AIC_BASE + 0x0C8)
#define AIC_SVR19	__REG32(AIC_BASE + 0x0CC)
#define AIC_SVR20	__REG32(AIC_BASE + 0x0D0)
#define AIC_SVR21	__REG32(AIC_BASE + 0x0D4)
#define AIC_SVR22	__REG32(AIC_BASE + 0x0D8)
#define AIC_SVR23	__REG32(AIC_BASE + 0x0DC)
#define AIC_SVR24	__REG32(AIC_BASE + 0x0E0)
#define AIC_SVR25	__REG32(AIC_BASE + 0x0E4)
#define AIC_SVR26	__REG32(AIC_BASE + 0x0E8)
#define AIC_SVR27	__REG32(AIC_BASE + 0x0EC)
#define AIC_SVR28	__REG32(AIC_BASE + 0x0F0)
#define AIC_SVR29	__REG32(AIC_BASE + 0x0F4)
#define AIC_SVR30	__REG32(AIC_BASE + 0x0F8)
#define AIC_SVR31	__REG32(AIC_BASE + 0x0FC)


#define AIC_IVR		__REG32(AIC_BASE + 0x100)	/* IRQ Vector Register */
#define AIC_FVR		__REG32(AIC_BASE + 0x104)	/* FIQ Vector Register */
#define AIC_ISR		__REG32(AIC_BASE + 0x108)	/* Interrupt Status Register */
#define AIC_IPR		__REG32(AIC_BASE + 0x10C)	/* Interrupt Pending Register */
#define AIC_IMR		__REG32(AIC_BASE + 0x110)	/* Interrupt Mask Register */
#define AIC_CISR	__REG32(AIC_BASE + 0x114)	/* Core Interrupt Status Register */
#define AIC_IECR	__REG32(AIC_BASE + 0x120)	/* Interrupt Enable Command Register */
#define AIC_IDCR	__REG32(AIC_BASE + 0x124)	/* Interrupt Disable Command Register */
#define AIC_ICCR	__REG32(AIC_BASE + 0x128)	/* Interrupt Clear Command Register */
#define AIC_ISCR	__REG32(AIC_BASE + 0x12C)	/* Interrupt Set Command Register */
#define AIC_EOICR	__REG32(AIC_BASE + 0x130)	/* End of Interrupt Command Register */
#define AIC_SPU		__REG32(AIC_BASE + 0x134)	/* Spurious Vector Register */
#define AIC_DCR		__REG32(AIC_BASE + 0x138)	/* Debug Control Register (Protect) */
#define AIC_FFER	__REG32(AIC_BASE + 0x140)	/* Fast Forcing Enable Register */
#define AIC_FFDR	__REG32(AIC_BASE + 0x144)	/* Fast Forcing Disable Register */
#define AIC_FFSR	__REG32(AIC_BASE + 0x148)	/* Fast Forcing Status Register */

/* AIC_SMR[]: Interrupt Source Mode Registers */
#define AIC_PRIOR						0x07	/* Priority */
#define AIC_PRIOR_LOWEST				0		/* lowest priority level */
#define AIC_PRIOR_HIGHEST				7		/* highest priority level */

#define AIC_SRCTYPE						0x60	/* Source Type Definition */

/* Internal Interrupts */
#define AIC_SRCTYPE_INT_LEVEL_SENSITIVE	0x00	/* Level Sensitive */
#define AIC_SRCTYPE_INT_EDGE_TRIGGERED	0x20	/* Edge Triggered */

/* External Interrupts */
#define AIC_SRCTYPE_EXT_LOW_LEVEL		0x00	/* Low Level */
#define AIC_SRCTYPE_EXT_NEGATIVE_EDGE	0x20	/* Negative Edge */
#define AIC_SRCTYPE_EXT_HIGH_LEVEL		0x40	/* High Level */
#define AIC_SRCTYPE_EXT_POSITIVE_EDGE	0x60	/* Positive Edge */

/* AIC_ISR: Interrupt Status Register */
#define AIC_IRQID						0x1F	/* Current source interrupt */

/* AIC_CISR: Interrupt Core Status Register */
#define AIC_NFIQ						0x01	/* Core FIQ Status */
#define AIC_NIRQ						0x02	/* Core IRQ Status */

/* AIC_DCR: Debug Control Register (Protect) */
#define AIC_DCR_PROT					0x01	/* Protection Mode */
#define AIC_DCR_GMSK					0x02	/* General Mask */


/*-----------------------------*/
/* Memory Controller Interface */
/*-----------------------------*/
#define MC_BASE		0xFFFFFF00
#define MC_RCR		__REG32(MC_BASE + 0x00)	/* Remap Control Register */
#define MC_ASR		__REG32(MC_BASE + 0x04)	/* Abort Status Register */
#define MC_AASR		__REG32(MC_BASE + 0x08)	/* Abort Address Status Register */
#define MC_FMR		__REG32(MC_BASE + 0x60)	/* Flash Mode Register */
#define MC_FCR		__REG32(MC_BASE + 0x64)	/* Flash Command Register */
#define MC_FSR		__REG32(MC_BASE + 0x68)	/* Flash Status Register */

/* MC_RCR: Remap Control Register */
#define MC_RCR_RCB		(1 << 0)		/* Remap Command Bit */

/* MC_ASR: Abort Status Register */
#define MC_ASR_UNDADD	(1 << 0)		/* Undefined Address */
#define MC_ASR_MISADD	(1 << 1)		/* Misaligned Address */
#define MC_ASR_ABTSZ	(0x03 << 8)		/* Abort Size */
#define MC_ASR_ABTSZ_BYTE	(0 << 8)	/* Byte */
#define MC_ASR_ABTSZ_HWORD	(1 << 8)	/* Half-word */
#define MC_ASR_ABTSZ_WORD	(2 << 8)	/* Word */
#define MC_ASR_ABTTYP	(0x03 << 10)	/* Abort Type */
#define MC_ASR_ABTTYP_DATAR	(0 << 10)	/* Data Read */
#define MC_ASR_ABTTYP_DATAW	(1 << 10)	/* Data Write */
#define MC_ASR_ABTTYP_FETCH	(2 << 10)	/* Code Fetch */
#define MC_ASR_MST0		(1 << 16)		/* Master 0 Abort Source */
#define MC_ASR_MST1		(1 << 17)		/* Master 1 Abort Source */
#define MC_ASR_SVMST0	(1 << 24)		/* Saved Master 0 Abort Source */
#define MC_ASR_SVMST1	(1 << 25)		/* Saved Master 1 Abort Source */

/* MC_FMR: Flash Mode Register */
#define MC_FMR_FRDY		(1 << 0)		/* Flash Ready Interrupt Enable */
#define MC_FMR_LOCKE	(1 << 2)		/* Lock Error Interrupt Enable */
#define MC_FMR_PROGE	(1 << 3)		/* Programming Error Interrupt Enable */
#define MC_FMR_NEBP		(1 << 7)		/* No Erase Before Programming */
#define MC_FMR_FWS		(0x03 << 8)		/* Flash Wait State */
#define MC_FMR_FWS_0FWS	(0 << 8)		/* 1 cycle for Read, 2 for Write operations */
#define MC_FMR_FWS_1FWS	(1 << 8)		/* 2 cycle for Read, 3 for Write operations */
#define MC_FMR_FWS_2FWS	(2 << 8)		/* 3 cycle for Read, 4 for Write operations */
#define MC_FMR_FWS_3FWS	(3 << 8)		/* 4 cycle for Read, 5 for Write operations */
#define MC_FMR_FMCN		(0xFF << 16)	/* Flash Microsecond Cycle Number */

/* MC_FCR: Flash Command Register */
#define MC_FCR_FCMD		(0x0F << 0)		/* Flash Command */
#define MC_FCR_FCMD_WP		(1 << 0)	/* Write Page */
#define MC_FCR_FCMD_SLB		(2 << 0)	/* Set Lock Bit */
#define MC_FCR_FCMD_WPL		(3 << 0)	/* Write Page and Lock */
#define MC_FCR_FCMD_CLB		(4 << 0)	/* Clear Lock Bit */
#define MC_FCR_FCMD_EA		(8 << 0)	/* Erase All */
#define MC_FCR_FCMD_SGPB	(0xB << 0)	/* Set General Purpose NVM Bit */
#define MC_FCR_FCMD_CGPB	(0xD << 0)	/* Clear General Purpose NVM Bit */
#define MC_FCR_FCMD_SSB		(0xF << 0)	/* Set Security Bit */
#define MC_FCR_PAGEN	(0x3FF <<  8)	/* Page Number */
#define MC_FCR_KEY		(0xFFu << 24)	/* Write Protection Key */

/* MC_FSR: Flash Status Register */
#define MC_FSR_FRDY			(1 <<  0)	/* Flash Ready Status */
#define MC_FSR_LOCKE		(1 <<  2)	/* Lock Error Status */
#define MC_FSR_PROGE		(1 <<  3)	/* Programming Error Status */
#define MC_FSR_SECURITY		(1 <<  4)	/* Security Bit */
#define MC_FSR_GPNVM0		(1 <<  8)	/* General-purpose NVM Bit 0 Status */
#define MC_FSR_GPNVM1		(1 <<  9)	/* General-purpose NVM Bit 1 Status */
#define MC_FSR_GPNVM2		(1 << 10)	/* General-purpose NVM Bit 2 Status */
#define MC_FSR_GPNVM3		(1 << 11)	/* General-purpose NVM Bit 3 Status */
#define MC_FSR_GPNVM4		(1 << 12)	/* General-purpose NVM Bit 4 Status */
#define MC_FSR_GPNVM5		(1 << 13)	/* General-purpose NVM Bit 5 Status */
#define MC_FSR_GPNVM6		(1 << 14)	/* General-purpose NVM Bit 6 Status */
#define MC_FSR_GPNVM7		(1 << 15)	/* General-purpose NVM Bit 7 Status */
#define MC_FSR_LOCKS0		(1 << 16)	/* Sector 0 Lock Status */
#define MC_FSR_LOCKS1		(1 << 17)	/* Sector 1 Lock Status */
#define MC_FSR_LOCKS2		(1 << 18)	/* Sector 2 Lock Status */
#define MC_FSR_LOCKS3		(1 << 19)	/* Sector 3 Lock Status */
#define MC_FSR_LOCKS4		(1 << 20)	/* Sector 4 Lock Status */
#define MC_FSR_LOCKS5		(1 << 21)	/* Sector 5 Lock Status */
#define MC_FSR_LOCKS6		(1 << 22)	/* Sector 6 Lock Status */
#define MC_FSR_LOCKS7		(1 << 23)	/* Sector 7 Lock Status */
#define MC_FSR_LOCKS8		(1 << 24)	/* Sector 8 Lock Status */
#define MC_FSR_LOCKS9		(1 << 25)	/* Sector 9 Lock Status */
#define MC_FSR_LOCKS10		(1 << 26)	/* Sector 10 Lock Status */
#define MC_FSR_LOCKS11		(1 << 27)	/* Sector 11 Lock Status */
#define MC_FSR_LOCKS12		(1 << 28)	/* Sector 12 Lock Status */
#define MC_FSR_LOCKS13		(1 << 29)	/* Sector 13 Lock Status */
#define MC_FSR_LOCKS14		(1 << 30)	/* Sector 14 Lock Status */
#define MC_FSR_LOCKS15		(1u << 31)	/* Sector 15 Lock Status */



/*------------*/
/* Debug Unit */
/*------------*/
#define DBGU_BASE	0xFFFFF200
#define DBGU_CR		__REG32(DBGU_BASE + 0x000)	/* Control Register */
#define DBGU_MR		__REG32(DBGU_BASE + 0x004)	/* Mode Register */
#define DBGU_IER	__REG32(DBGU_BASE + 0x008)	/* Interrupt Enable Register */
#define DBGU_IDR	__REG32(DBGU_BASE + 0x00C)	/* Interrupt Disable Register */
#define DBGU_IMR	__REG32(DBGU_BASE + 0x010)	/* Interrupt Mask Register */
#define DBGU_SR		__REG32(DBGU_BASE + 0x014)	/* Status Register */
#define DBGU_RHR	__REG32(DBGU_BASE + 0x018)	/* Receive Holding Register */
#define DBGU_THR	__REG32(DBGU_BASE + 0x01C)	/* Transmit Holding Register */
#define DBGU_BRGR	__REG32(DBGU_BASE + 0x020)	/* Baud Rate Generator Register */
#define DBGU_CIDR	__REG32(DBGU_BASE + 0x040)	/* Chip ID Register */
#define DBGU_EXID	__REG32(DBGU_BASE + 0x044)	/* Chip ID Extension Register */
#define DBGU_FNR	__REG32(DBGU_BASE + 0x048)	/* Force NTRST Register */

#define DBGU_RPR	__REG32(DBGU_BASE + 0x100)	/* Receiver Pointer Register */
#define DBGU_RCR	__REG32(DBGU_BASE + 0x104)	/* Receiver Counter Register */
#define DBGU_TPR	__REG32(DBGU_BASE + 0x108)	/* Transmitter Pointer Register */
#define DBGU_TCR	__REG32(DBGU_BASE + 0x10C)	/* Transmitter Counter Register */
#define DBGU_RNPR	__REG32(DBGU_BASE + 0x110)	/* Receiver Next Pointer Register */
#define DBGU_RNCR	__REG32(DBGU_BASE + 0x114)	/* Receiver Next Counter Register */
#define DBGU_TNPR	__REG32(DBGU_BASE + 0x118)	/* Transmitter Next Pointer Register */
#define DBGU_TNCR	__REG32(DBGU_BASE + 0x11C)	/* Transmitter Next Counter Register */
#define DBGU_PTCR	__REG32(DBGU_BASE + 0x120)	/* PDC Transfer Control Register */
#define DBGU_PTSR	__REG32(DBGU_BASE + 0x124)	/* PDC Transfer Status Register */

/* DBGU_CIDR: Chip ID Register */
#define DBGU_CIDR_VERSION	(0x1F <<  0)		/* Version of device */
#define DBGU_CIDR_EPROC		(0x07 <<  5)		/* Embedded Processor */
#define DBGU_CIDR_NVPSIZ	(0x0F <<  8)		/* Non Volatile Program Memory Size */
#define DBGU_CIDR_NVPSIZ2	(0x0F << 12)		/* Second Non Volatile Program Memory Size */
#define DBGU_CIDR_SRAMSIZ	(0x0F << 16)		/* Internal SRAM Size */
#define DBGU_CIDR_ARCH		(0xFF << 20)		/* Architecture Identifier */
#define DBGU_CIDR_NVPTYP	(0x07 << 28)		/* Non Volatile Program Memory Type*/
#define DBGU_CIDR_EXT		(1u << 31)			/* Extension Flag */


/*---------------------------------------*/
/* User Interface Parallel I/O Interface */
/*---------------------------------------*/
/* Parallel I/O Controller A */
#define PIOA_BASE	0xFFFFF400
#define PIOA_PER	__REG32(PIOA_BASE + 0x00)	/* PIO Enable Register */
#define PIOA_PDR	__REG32(PIOA_BASE + 0x04)	/* PIO Disable Register */
#define PIOA_PSR	__REG32(PIOA_BASE + 0x08)	/* PIO Status Register */
#define PIOA_OER	__REG32(PIOA_BASE + 0x10)	/* Output Enable Register */
#define PIOA_ODR	__REG32(PIOA_BASE + 0x14)	/* Output Disable Register */
#define PIOA_OSR	__REG32(PIOA_BASE + 0x18)	/* Output Status Register */
#define PIOA_IFER	__REG32(PIOA_BASE + 0x20)	/* Input Filter Enable Register */
#define PIOA_IFDR	__REG32(PIOA_BASE + 0x24)	/* Input Filter Disable Register */
#define PIOA_IFSR	__REG32(PIOA_BASE + 0x28)	/* Input Filter Status Register */
#define PIOA_SODR	__REG32(PIOA_BASE + 0x30)	/* Set Output Data Register */
#define PIOA_CODR	__REG32(PIOA_BASE + 0x34)	/* Clear Output Data Register */
#define PIOA_ODSR	__REG32(PIOA_BASE + 0x38)	/* Output Data Status Register */
#define PIOA_PDSR	__REG32(PIOA_BASE + 0x3C)	/* Pin Data Status Register */
#define PIOA_IER	__REG32(PIOA_BASE + 0x40)	/* Interrupt Enable Register */
#define PIOA_IDR	__REG32(PIOA_BASE + 0x44)	/* Interrupt Disable Register */
#define PIOA_IMR	__REG32(PIOA_BASE + 0x48)	/* Interrupt Mask Register */
#define PIOA_ISR	__REG32(PIOA_BASE + 0x4C)	/* Interrupt Status Register */
#define PIOA_MDER	__REG32(PIOA_BASE + 0x50)	/* Multi Driver Enable Register */
#define PIOA_MDDR	__REG32(PIOA_BASE + 0x54)	/* Multi Driver Disable Register */
#define PIOA_MDSR	__REG32(PIOA_BASE + 0x58)	/* Multi Driver Status Register */
#define PIOA_PUDR	__REG32(PIOA_BASE + 0x60)	/* Pull-Up Disable Register */
#define PIOA_PUER	__REG32(PIOA_BASE + 0x64)	/* Pull-Up Enable Register */
#define PIOA_PUSR	__REG32(PIOA_BASE + 0x68)	/* Pull-Up Status Register */
#define PIOA_ASR	__REG32(PIOA_BASE + 0x70)	/* Select A Register */
#define PIOA_BSR	__REG32(PIOA_BASE + 0x74)	/* Select B Register */
#define PIOA_ABSR	__REG32(PIOA_BASE + 0x78)	/* AB Select Status Register */
#define PIOA_OWER	__REG32(PIOA_BASE + 0xA0)	/* Output Write Enable Register */
#define PIOA_OWDR	__REG32(PIOA_BASE + 0xA4)	/* Output Write Disable Register */
#define PIOA_OWSR	__REG32(PIOA_BASE + 0xA8)	/* Output Write Status Register */


/*------------------*/
/* Pin Multiplexing */
/*------------------*/
#define PIO_PA0		(1 << 0)	/* Pin controlled by PIO */
#define PA0_PWM0	(1 << 0)	/* PWM Channel 0 */
#define PA0_TIOA0	(1 << 0)	/* Timer Counter 0 multi purpose I/O pin A */

#define PIO_PA1		(1 << 1)	/* Pin controlled by PIO */
#define PA1_PWM1	(1 << 1)	/* PWM Channel 1 */
#define PA1_TIOB0	(1 << 1)	/* Timer Counter 0 multi purpose I/O pin B */

#define PIO_PA2		(1 << 2)	/* Pin controlled by PIO */
#define PA2_PWM2	(1 << 2)	/* PWM Channel 2 */
#define PA2_SCK0	(1 << 2)	/* USART 0 Serial Clock */

#define PIO_PA3		(1 << 3)	/* Pin controlled by PIO */
#define PA3_TWD		(1 << 3)	/* TWI Serial Data */
#define PA3_NPCS3	(1 << 3)	/* SPI Peripheral Chip Select 3 */

#define PIO_PA4		(1 << 4)	/* Pin controlled by PIO */
#define PA4_TWCK	(1 << 4)	/* TWI Serial Clock */
#define PA4_TCLK0	(1 << 4)	/* Timer Counter 0 external clock input */

#define PIO_PA5		(1 << 5)	/* Pin controlled by PIO */
#define PA5_RXD0	(1 << 5)	/* USART 0 Receive Data */
#define PA5_NPCS3	(1 << 5)	/* SPI Peripheral Chip Select 3 */

#define PIO_PA6		(1 << 6)	/* Pin controlled by PIO */
#define PA6_TXD0	(1 << 6)	/* USART 0 Transmit Data */
#define PA6_PCK0	(1 << 6)	/* PMC Programmable Clock Output 0 */

#define PIO_PA7		(1 << 7)	/* Pin controlled by PIO */
#define PA7_RTS0	(1 << 7)	/* USART 0 Ready To Send */
#define PA7_PWM3	(1 << 7)	/* PWM Channel 3 */

#define PIO_PA8		(1 << 8)	/* Pin controlled by PIO */
#define PA8_CTS0	(1 << 8)	/* USART 0 Clear To Send */
#define PA8_ADTRG	(1 << 8)	/* ADC External Trigger */

#define PIO_PA9		(1 << 9)	/* Pin controlled by PIO */
#define PA9_DRXD	(1 << 9)	/* DBGU Debug Receive Data */
#define PA9_NPCS1	(1 << 9)	/* SPI Peripheral Chip Select 1 */

#define PIO_PA10	(1 << 10)	/* Pin controlled by PIO */
#define PA10_DTXD	(1 << 10)	/* DBGU Debug Transmit Data */
#define PA10_NPCS2	(1 << 10)	/* SPI Peripheral Chip Select 2 */

#define PIO_PA11	(1 << 11)	/* Pin controlled by PIO */
#define PA11_NPCS0	(1 << 11)	/* SPI Peripheral Chip Select 0 */
#define PA11_PWM0	(1 << 11)	/* PWM Channel 0 */

#define PIO_PA12	(1 << 12)	/* Pin controlled by PIO */
#define PA12_MISO	(1 << 12)	/* SPI Master In Slave Out */
#define PA12_PWM1	(1 << 12)	/* PWM Channel 1 */

#define PIO_PA13	(1 << 13)	/* Pin controlled by PIO */
#define PA13_MOSI	(1 << 13)	/* SPI Master Out Slave In */
#define PA13_PWM2	(1 << 13)	/* PWM Channel 2 */

#define PIO_PA14	(1 << 14)	/* Pin controlled by PIO */
#define PA14_SPCK	(1 << 14)	/* SPI Serial Clock */
#define PA14_PWM3	(1 << 14)	/* PWM Channel 3 */

#define PIO_PA15	(1 << 15)	/* Pin controlled by PIO */
#define PA15_TF		(1 << 15)	/* SSC Transmit Frame Sync */
#define PA15_TIOA1	(1 << 15)	/* Timer Counter 1 multi purpose I/O pin A */

#define PIO_PA16	(1 << 16)	/* Pin controlled by PIO */
#define PA16_TK		(1 << 16)	/* SSC Transmit Clock */
#define PA16_TIOB1	(1 << 16)	/* Timer Counter 1 multi purpose I/O pin B */

#define PIO_PA17	(1 << 17)	/* Pin controlled by PIO */
#define PA17_TD		(1 << 17)	/* SSC Transmit Data */
#define PA17_PCK1	(1 << 17)	/* PMC Programmable Clock Output 1 */

#define PIO_PA18	(1 << 18)	/* Pin controlled by PIO */
#define PA18_RD		(1 << 18)	/* SSC Receive Data */
#define PA18_PCK2	(1 << 18)	/* PMC Programmable Clock Output 2 */

#define PIO_PA19	(1 << 19)	/* Pin controlled by PIO */
#define PA19_RK		(1 << 19)	/* SSC Receive Clock */
#define PA19_FIQ	(1 << 19)	/* AIC Fast Interrupt Input */

#define PIO_PA20	(1 << 20)	/* Pin controlled by PIO */
#define PA20_RF		(1 << 20)	/* SSC Receive Frame Sync */
#define PA20_IRQ0	(1 << 20)	/* External Interrupt 0 */

#define PIO_PA21	(1 << 21)	/* Pin controlled by PIO */
#define PA21_RXD1	(1 << 21)	/* USART 1 Receive Data */
#define PA21_PCK1	(1 << 21)	/* PMC Programmable Clock Output 1 */

#define PIO_PA22	(1 << 22)	/* Pin controlled by PIO */
#define PA22_TXD1	(1 << 22)	/* USART 1 Transmit Data */
#define PA22_NPCS3	(1 << 22)	/* SPI Peripheral Chip Select 3 */

#define PIO_PA23	(1 << 23)	/* Pin controlled by PIO */
#define PA23_SCK1	(1 << 23)	/* USART 1 Serial Clock */
#define PA23_PWM0	(1 << 23)	/* PWM Channel 0 */

#define PIO_PA24	(1 << 24)	/* Pin controlled by PIO */
#define PA24_RTS1	(1 << 24)	/* USART 1 Ready To Send */
#define PA24_PWM1	(1 << 24)	/* PWM Channel 1 */

#define PIO_PA25	(1 << 25)	/* Pin controlled by PIO */
#define PA25_CTS1	(1 << 25)	/* USART 1 Clear To Send */
#define PA25_PWM2	(1 << 25)	/* PWM Channel 2 */

#define PIO_PA26	(1 << 26)	/* Pin controlled by PIO */
#define PA26_DCD1	(1 << 26)	/* USART 1 Data Carrier Detect */
#define PA26_TIOA2	(1 << 26)	/* Timer Counter 2 multi purpose I/O pin A */

#define PIO_PA27	(1 << 27)	/* Pin controlled by PIO */
#define PA27_DTR1	(1 << 27)	/* USART 1 Data Terminal Ready */
#define PA27_TIOB2	(1 << 27)	/* Timer Counter 2 multi purpose I/O pin B */

#define PIO_PA28	(1 << 28)	/* Pin controlled by PIO */
#define PA28_DSR1	(1 << 28)	/* USART 1 Data Set Ready */
#define PA28_TCLK1	(1 << 28)	/* Timer Counter 1 external clock input */

#define PIO_PA29	(1 << 29)	/* Pin controlled by PIO */
#define PA29_RI1	(1 << 29)	/* USART 1 Ring Indicator */
#define PA29_TCLK2	(1 << 29)	/* Timer Counter 2 external clock input */

#define PIO_PA30	(1 << 30)	/* Pin controlled by PIO */
#define PA30_IRQ1	(1 << 30)	/* External Interrupt 1 */
#define PA30_NPCS2	(1 << 30)	/* SPI Peripheral Chip Select 2 */

#define PIO_PA31	(1u << 31)	/* Pin controlled by PIO */
#define PA31_NPCS1	(1u << 31)	/* SPI Peripheral Chip Select 1 */
#define PA31_PCK2	(1u << 31)	/* PMC Programmable Clock Output 2 */



/*-----------------------------------*/
/* User Interface Timer/Counter Unit */
/*-----------------------------------*/

/* Timer Control Block: Channels 0, 1, 2 */
#define TCB_BASE	0xFFFA0000
#define TC0_CCR		__REG32(TCB_BASE + 0x00)	/* Control Register */
#define TC0_CMR		__REG32(TCB_BASE + 0x04)	/* Mode Register */
#define TC0_CV		__REG32(TCB_BASE + 0x10)	/* Counter value */
#define TC0_RA		__REG32(TCB_BASE + 0x14)	/* Register A */
#define TC0_RB		__REG32(TCB_BASE + 0x18)	/* Register B */
#define TC0_RC		__REG32(TCB_BASE + 0x1C)	/* Register C */
#define TC0_SR		__REG32(TCB_BASE + 0x20)	/* Status Register */
#define TC0_IER		__REG32(TCB_BASE + 0x24)	/* Interrupt Enable Register */
#define TC0_IDR		__REG32(TCB_BASE + 0x28)	/* Interrupt Disable Register */
#define TC0_IMR		__REG32(TCB_BASE + 0x2C)	/* Interrupt Mask Register */

#define TC1_CCR		__REG32(TCB_BASE + 0x40)	/* Control Register */
#define TC1_CMR		__REG32(TCB_BASE + 0x44)	/* Mode Register */
#define TC1_CV		__REG32(TCB_BASE + 0x50)	/* Counter value */
#define TC1_RA		__REG32(TCB_BASE + 0x54)	/* Register A */
#define TC1_RB		__REG32(TCB_BASE + 0x58)	/* Register B */
#define TC1_RC		__REG32(TCB_BASE + 0x5C)	/* Register C */
#define TC1_SR		__REG32(TCB_BASE + 0x60)	/* Status Register */
#define TC1_IER		__REG32(TCB_BASE + 0x64)	/* Interrupt Enable Register */
#define TC1_IDR		__REG32(TCB_BASE + 0x68)	/* Interrupt Disable Register */
#define TC1_IMR		__REG32(TCB_BASE + 0x6C)	/* Interrupt Mask Register */

#define TC2_CCR		__REG32(TCB_BASE + 0x80)	/* Control Register */
#define TC2_CMR		__REG32(TCB_BASE + 0x84)	/* Mode Register */
#define TC2_CV		__REG32(TCB_BASE + 0x90)	/* Counter value */
#define TC2_RA		__REG32(TCB_BASE + 0x94)	/* Register A */
#define TC2_RB		__REG32(TCB_BASE + 0x98)	/* Register B */
#define TC2_RC		__REG32(TCB_BASE + 0x9C)	/* Register C */
#define TC2_SR		__REG32(TCB_BASE + 0xA0)	/* Status Register */
#define TC2_IER		__REG32(TCB_BASE + 0xA4)	/* Interrupt Enable Register */
#define TC2_IDR		__REG32(TCB_BASE + 0xA8)	/* Interrupt Disable Register */
#define TC2_IMR		__REG32(TCB_BASE + 0xAC)	/* Interrupt Mask Register */

#define TCB_BCR		__REG32(TCB_BASE + 0xC0)	/* Block Control Register */
#define TCB_BMR		__REG32(TCB_BASE + 0xC4)	/* Block Mode Register  */


/*--------------------------------------------------------*/
/* TC_CCR: Timer Counter Control Register Bits Definition */
/*--------------------------------------------------------*/
#define TC_CLKEN			(1 << 0)	/* Counter Clock Enable */
#define TC_CLKDIS			(1 << 1)	/* Counter Clock Disable */
#define TC_SWTRG			(1 << 2)	/* Software Trigger */

/* TC_CMR: Timer Counter Channel Mode Register Bits Definition   */

/* Clock Selection */
#define TC_CLKS				(0x07 << 0)
#define TC_CLKS_MCK2		(0x00 << 0)
#define TC_CLKS_MCK8		(0x01 << 0)
#define TC_CLKS_MCK32		(0x02 << 0)
#define TC_CLKS_MCK128		(0x03 << 0)
#define TC_CLKS_MCK1024		(0x04 << 0)

#define TC_CLKS_SLCK		(0x04 << 0)

#define TC_CLKS_XC0			(0x05 << 0)
#define TC_CLKS_XC1			(0x06 << 0)
#define TC_CLKS_XC2			(0x07 << 0)


/* Clock Inversion */
#define TC_CLKI				(1 << 3)

/* Burst Signal Selection */
#define TC_BURST			(0x03 << 4)
#define TC_BURST_NONE		(0x00 << 4)
#define TC_BUSRT_XC0		(0x01 << 4)
#define TC_BURST_XC1		(0x02 << 4)
#define TC_BURST_XC2		(0x03 << 4)

/* Capture Mode : Counter Clock Stopped with RB Loading */
#define TC_LDBSTOP			(1 << 6)

/* Waveform Mode : Counter Clock Stopped with RC Compare */
#define TC_CPCSTOP			(1 << 6)

/* Capture Mode : Counter Clock Disabled with RB Loading */
#define TC_LDBDIS			(1 << 7)

/* Waveform Mode : Counter Clock Disabled with RC Compare */
#define TC_CPCDIS			(1 << 7)

/* Capture Mode : External Trigger Edge Selection */
#define TC_ETRGEDG			(0x03 << 8)
#define TC_ETRGEDG_NONE		(0x00 << 8)
#define TC_ETRGEDG_RISING	(0x01 << 8)
#define TC_ETRGEDG_FALLING	(0x02 << 8)
#define TC_ETRGEDG_BOTH		(0x03 << 8)

/* Waveform Mode : External Event Edge Selection */
#define TC_EEVTEDG			(0x03 << 8)
#define TC_EEVTEDG_NONE		(0x00 << 8)
#define TC_EEVTEDG_RISING	(0x01 << 8)
#define TC_EEVTEDG_FALLING	(0x02 << 8)
#define TC_EEVTEDG_BOTH		(0x03 << 8)

/* Capture Mode : TIOA or TIOB External Trigger Selection */
#define TC_ABETRG			(0x01 << 10)
#define TC_ABETRG_TIOB		(0x00 << 10)
#define TC_ABETRG_TIOA		(0x01 << 10)

/* Waveform Mode : External Event Selection */
#define TC_EEVT				(0x03 << 10)
#define TC_EEVT_TIOB		(0x00 << 10)
#define TC_EEVT_XC0			(0x01 << 10)
#define TC_EEVT_XC1			(0x02 << 10)
#define TC_EEVT_XC2			(0x03 << 10)


#define TC_ENETRG			(1 << 12)	/* Waveform Mode : External Event Trigger enable */

#define TC_CPCTRG			(1 << 14)	/* RC Compare Trigger Enable */

/* Mode Selection */
#define TC_WAVE				(1 << 15)
#define TC_CAPT				(0 << 15)

/* Capture Mode : RA Loading Selection */
#define TC_LDRA				(0x03 << 16)
#define TC_LDRA_NONE		(0x00 << 16)
#define TC_LDRA_RISING		(0x01 << 16)
#define TC_LDRA_FALLING		(0x02 << 16)
#define TC_LDRA_BOTH		(0x03 << 16)

/* Waveform Mode : RA Compare Effect on TIOA */
#define TC_ACPA				(0x03 << 16)
#define TC_ACPA_NONE		(0x00 << 16)
#define TC_ACPA_SET			(0x01 << 16)
#define TC_ACPA_CLEAR		(0x02 << 16)
#define TC_ACPA_TOGGLE		(0x03 << 16)

/* Capture Mode : RB Loading Selection */
#define TC_LDRB				(0x03 << 18)
#define TC_LDRB_NONE		(0x00 << 18)
#define TC_LDRB_RISING		(0x01 << 18)
#define TC_LDRB_FALLING		(0x02 << 18)
#define TC_LDRB_BOTH		(0x03 << 18)

/* Waveform Mode : RC Compare Effect on TIOA */
#define TC_ACPC				(0x03 << 18)
#define TC_ACPC_NONE		(0x00 << 18)
#define TC_ACPC_SET			(0x01 << 18)
#define TC_ACPC_CLEAR		(0x02 << 18)
#define TC_ACPC_TOGGLE		(0x03 << 18)

/* Waveform Mode : External Event Effect on TIOA */
#define TC_AEEVT			(0x03 << 20)
#define TC_AEEVT_NONE		(0x00 << 20)
#define TC_AEEVT_SET		(0x01 << 20)
#define TC_AEEVT_CLEAR		(0x02 << 20)
#define TC_AEEVT_TOGGLE		(0x03 << 20)

/* Waveform Mode : Software Trigger Effect on TIOA */
#define TC_ASWTRG			(0x03 << 22)
#define TC_ASWTRG_NONE		(0x00 << 22)
#define TC_ASWTRG_SET		(0x01 << 22)
#define TC_ASWTRG_CLEAR		(0x02 << 22)
#define TC_ASWTRG_TOGGLE	(0x03 << 22)

/* Waveform Mode : RB Compare Effect on TIOB */
#define TC_BCPB				(0x01 << 24)
#define TC_BCPB_NONE		(0x00 << 24)
#define TC_BCPB_SET			(0x01 << 24)
#define TC_BCPB_CLEAR		(0x02 << 24)
#define TC_BCPB_TOGGLE		(0x03 << 24)

/* Waveform Mode : RC Compare Effect on TIOB */
#define TC_BCPC				(0x03 << 26)
#define TC_BCPC_NONE		(0x00 << 26)
#define TC_BCPC_SET			(0x01 << 26)
#define TC_BCPC_CLEAR		(0x02 << 26)
#define TC_BCPC_TOGGLE		(0x03 << 26)

/* Waveform Mode : External Event Effect on TIOB */
#define TC_BEEVT			(0x03 << 28)
#define TC_BEEVT_NONE		(0x00 << 28)
#define TC_BEEVT_SET		(0x01 << 28)
#define TC_BEEVT_CLEAR		(0x02 << 28)
#define TC_BEEVT_TOGGLE		(0x03 << 28)

/* Waveform Mode : Software Trigger Effect on TIOB */
#define TC_BSWTRG			(0x03u << 30)
#define TC_BSWTRG_NONE		(0x00u << 30)
#define TC_BSWTRG_SET		(0x01u << 30)
#define TC_BSWTRG_CLEAR		(0x02u << 30)
#define TC_BSWTRG_TOGGLE	(0x03u << 30)

/* TC_SR: Timer Counter Status Register Bits Definition */
#define TC_COVFS			(1 <<  0)	/* Counter Overflow Status */
#define TC_LOVRS			(1 <<  1)	/* Load Overrun Status */
#define TC_CPAS				(1 <<  2)	/* RA Compare Status */
#define TC_CPBS				(1 <<  3)	/* RB Compare Status */
#define TC_CPCS				(1 <<  4)	/* RC Compare Status */
#define TC_LDRAS			(1 <<  5)	/* RA Loading Status */
#define TC_LDRBS			(1 <<  6)	/* RB Loading Status */
#define TC_ETRGS			(1 <<  7)	/* External Trigger Status */
#define TC_CLKSTA			(1 << 16)	/* Clock Status */
#define TC_MTIOA			(1 << 17)	/* TIOA Mirror */
#define TC_MTIOB			(1 << 18)	/* TIOB Mirror */

/* TC_BCR: Timer Counter Block Control Register Bits Definition */
#define TC_SYNC				(1 << 0)	/* Synchronisation Trigger */

/*  TC_BMR: Timer Counter Block Mode Register Bits Definition */
#define TC_TC0XC0S			(0x03 << 0)	/* External Clock Signal 0 Selection */
#define TC_TC0XC0S_TCLK0	(0x00 << 0)	/* TCLK0 connected to XC0 */
#define TC_TC0XC0S_NONE		(0x01 << 0)	/* No signal connected to XC0 */
#define TC_TC0XC0S_TIOA1	(0x02 << 0)	/* TIOA1 connected to XC0 */
#define TC_TC0XC0S_TIOA2	(0x03 << 0)	/* TIOA2 connected to XC0 */

#define TC_TC1XC1S			(0x03 << 2)	/* External Clock Signal 1 Selection */
#define TC_TC1XC1S_TCLK1	(0x00 << 2)	/* TCLK1 connected to XC1 */
#define TC_TC1XC1S_NONE		(0x01 << 2)	/* No signal connected to XC1 */
#define TC_TC1XC1S_TIOA0	(0x02 << 2)	/* TIOA0 connected to XC1 */
#define TC_TC1XC1S_TIOA2	(0x03 << 2)	/* TIOA2 connected to XC1 */

#define TC_TC2XC2S			(0x03 << 4)	/* External Clock Signal 2 Selection */
#define TC_TC2XC2S_TCLK2	(0x00 << 4)	/* TCLK2 connected to XC2 */
#define TC_TC2XC2S_NONE		(0x01 << 4)	/* No signal connected to XC2 */
#define TC_TC2XC2S_TIOA0	(0x02 << 4)	/* TIOA0 connected to XC2 */
#define TC_TC2XC2S_TIOA1	(0x03 << 4)	/* TIOA1 connected to XC2 */



/*----------------------*/
/* User Interface USART */
/*----------------------*/

/* USART 0 */
#define USART0_BASE	0xFFFC0000
#define US0_CR		__REG32(USART0_BASE + 0x000)	/* Control Register */
#define US0_MR		__REG32(USART0_BASE + 0x004)	/* Mode Register */
#define US0_IER		__REG32(USART0_BASE + 0x008)	/* Interrupt Enable Register */
#define US0_IDR		__REG32(USART0_BASE + 0x00C)	/* Interrupt Disable Register */
#define US0_IMR		__REG32(USART0_BASE + 0x010)	/* Interrupt Mask Register */
#define US0_CSR		__REG32(USART0_BASE + 0x014)	/* Channel Status Register */
#define US0_RHR		__REG32(USART0_BASE + 0x018)	/* Receive Holding Register */
#define US0_THR		__REG32(USART0_BASE + 0x01C)	/* Transmit Holding Register */
#define US0_BRGR	__REG32(USART0_BASE + 0x020)	/* Baud Rate Generator Register */
#define US0_RTOR	__REG32(USART0_BASE + 0x024)	/* Receiver Timeout Register */
#define US0_TTGR	__REG32(USART0_BASE + 0x028)	/* Transmitter Time-guard Register */
#define US0_FIDI	__REG32(USART0_BASE + 0x040)	/* FI_DI_Ratio Register */
#define US0_NER		__REG32(USART0_BASE + 0x044)	/* Number of Errors Register */
#define US0_IF		__REG32(USART0_BASE + 0x04C)	/* IrDA Filter Register */

#define US0_RPR		__REG32(USART0_BASE + 0x100)	/* Receiver Pointer Register */
#define US0_RCR		__REG32(USART0_BASE + 0x104)	/* Receiver Counter Register */
#define US0_TPR		__REG32(USART0_BASE + 0x108)	/* Transmitter Pointer Register */
#define US0_TCR		__REG32(USART0_BASE + 0x10C)	/* Transmitter Counter Register */
#define US0_RNPR	__REG32(USART0_BASE + 0x110)	/* Receiver Next Pointer Register */
#define US0_RNCR	__REG32(USART0_BASE + 0x114)	/* Receiver Next Counter Register */
#define US0_TNPR	__REG32(USART0_BASE + 0x118)	/* Transmitter Next Pointer Register */
#define US0_TNCR	__REG32(USART0_BASE + 0x11C)	/* Transmitter Next Counter Register */
#define US0_PTCR	__REG32(USART0_BASE + 0x120)	/* PDC Transfer Control Register */
#define US0_PTSR	__REG32(USART0_BASE + 0x124)	/* PDC Transfer Status Register */

/* USART 1 */
#define USART1_BASE 	0xFFFC4000
#define US1_CR		__REG32(USART1_BASE + 0x000)	/* Control Register */
#define US1_MR		__REG32(USART1_BASE + 0x004)	/* Mode Register */
#define US1_IER		__REG32(USART1_BASE + 0x008)	/* Interrupt Enable Register */
#define US1_IDR		__REG32(USART1_BASE + 0x00C)	/* Interrupt Disable Register */
#define US1_IMR		__REG32(USART1_BASE + 0x010)	/* Interrupt Mask Register */
#define US1_CSR		__REG32(USART1_BASE + 0x014)	/* Channel Status Register */
#define US1_RHR		__REG32(USART1_BASE + 0x018)	/* Receive Holding Register */
#define US1_THR		__REG32(USART1_BASE + 0x01C)	/* Transmit Holding Register */
#define US1_BRGR	__REG32(USART1_BASE + 0x020)	/* Baud Rate Generator Register */
#define US1_RTOR	__REG32(USART1_BASE + 0x024)	/* Receiver Timeout Register */
#define US1_TTGR	__REG32(USART1_BASE + 0x028)	/* Transmitter Time-guard Register */
#define US1_FIDI	__REG32(USART1_BASE + 0x040)	/* FI_DI_Ratio Register */
#define US1_NER		__REG32(USART1_BASE + 0x044)	/* Number of Errors Register */
#define US1_IF		__REG32(USART1_BASE + 0x04C)	/* IrDA Filter Register */

#define US1_RPR		__REG32(USART1_BASE + 0x100)	/* Receiver Pointer Register */
#define US1_RCR		__REG32(USART1_BASE + 0x104)	/* Receiver Counter Register */
#define US1_TPR		__REG32(USART1_BASE + 0x108)	/* Transmitter Pointer Register */
#define US1_TCR		__REG32(USART1_BASE + 0x10C)	/* Transmitter Counter Register */
#define US1_RNPR	__REG32(USART1_BASE + 0x110)	/* Receiver Next Pointer Register */
#define US1_RNCR	__REG32(USART1_BASE + 0x114)	/* Receiver Next Counter Register */
#define US1_TNPR	__REG32(USART1_BASE + 0x118)	/* Transmitter Next Pointer Register */
#define US1_TNCR	__REG32(USART1_BASE + 0x11C)	/* Transmitter Next Counter Register */
#define US1_PTCR	__REG32(USART1_BASE + 0x120)	/* PDC Transfer Control Register */
#define US1_PTSR	__REG32(USART1_BASE + 0x124)	/* PDC Transfer Status Register */


/* US_CR : Control Register */

#define US_RSTRX			(1 <<  2)	/* Reset Receiver */
#define US_RSTTX			(1 <<  3)	/* Reset Transmitter */
#define US_RXEN				(1 <<  4)	/* Receiver Enable */
#define US_RXDIS			(1 <<  5)	/* Receiver Disable */
#define US_TXEN				(1 <<  6)	/* Transmitter Enable */
#define US_TXDIS			(1 <<  7)	/* Transmitter Disable */
#define US_RSTSTA			(1 <<  8)	/* Reset Status Bits */
#define US_STTBRK			(1 <<  9)	/* Start Break */
#define US_STPBRK			(1 << 10)	/* Stop Break */
#define US_STTTO			(1 << 11)	/* Start Time-out */
#define US_SENDA			(1 << 12)	/* Send Address */
#define US_RSTIT			(1 << 13)	/* Reset Iterations */
#define US_RSTNACK			(1 << 14)	/* Reset Non Acknowledge */
#define US_RETTO			(1 << 15)	/* Rearm Time-out */
#define US_DTREN			(1 << 16)	/* Data Terminal Ready Enable */
#define US_DTRDIS			(1 << 17)	/* Data Terminal Ready Disable */
#define US_RTSEN			(1 << 18)	/* Request To Send Enable */
#define US_RTSDIS			(1 << 19)	/* Request To Send Disable */


/* US_MR : Mode Register */
#define US_USMODE			(0x0F << 0)		/* USART Mode */
#define US_USMODE_NORMAL	(0x00 << 0)
#define US_USMODE_RS485		(0x01 << 0)
#define US_USMODE_HWHSH		(0x02 << 0)		/* Hardware Handshaking */
#define US_USMODE_MODEM		(0x03 << 0)
#define US_USMODE_ISO7816_0	(0x04 << 0)
#define US_USMODE_ISO7816_1	(0x06 << 0)
#define US_USMODE_IRDA		(0x08 << 0)
#define US_USMODE_SWHSH		(0x0C << 0)		/* Software Handshaking */

#define US_CLKS				(0x03 << 4)		/* Clock Selection */
#define US_CLKS_CLOCK		(0x00 << 4)		/* Clock */
#define US_CLKS_FDIV1		(0x01 << 4)		/* Clock divided by 8 */
#define US_CLKS_SLOW		(0x02 << 4)		/* Slow Clock */
#define US_CLKS_EXT			(0x03 << 4)		/* External Clock */

#define US_CHRL				(0x03 << 6)		/* Byte Length */
#define US_CHRL_5			(0x00 << 6)		/* 5 bits */
#define US_CHRL_6			(0x01 << 6)		/* 6 bits */
#define US_CHRL_7			(0x02 << 6)		/* 7 bits */
#define US_CHRL_8			(0x03 << 6)		/* 8 bits */

#define US_SYNC				(1 << 8)		/* Synchronous Mode Enable */

#define US_PAR				(0x07 << 9)		/* Parity Mode */
#define US_PAR_EVEN			(0x00 << 9)		/* Even Parity */
#define US_PAR_ODD			(0x01 << 9)		/* Odd Parity */
#define US_PAR_SPACE		(0x02 << 9)		/* Space Parity to 0 */
#define US_PAR_MARK			(0x03 << 9)		/* Marked Parity to 1 */
#define US_PAR_NO			(0x04 << 9)		/* No Parity */
#define US_PAR_MULTIDROP	(0x06 << 9)		/* Multi-drop Mode */

#define US_NBSTOP			(0x03 << 12)	/* Stop Bit Number */
#define US_NBSTOP_1			(0x00 << 12)	/* 1 Stop Bit */
#define US_NBSTOP_1_5		(0x01 << 12)	/* 1.5 Stop Bits */
#define US_NBSTOP_2			(0x02 << 12)	/* 2 Stop Bits */

#define US_CHMODE					(0x03 << 14)	/* Channel Mode */
#define US_CHMODE_NORMAL			(0x00 << 14)	/* Normal Mode */
#define US_CHMODE_AUTOMATIC_ECHO	(0x01 << 14)	/* Automatic Echo */
#define US_CHMODE_LOCAL_LOOPBACK	(0x02 << 14)	/* Local Loopback */
#define US_CHMODE_REMOTE_LOOPBACK	(0x03 << 14)	/* Remote Loopback */

#define US_MSBF				(1 << 16)		/* Bit order: MSB First */
#define US_MODE9			(1 << 17)		/* 9 Bit Mode */
#define US_CLKO				(1 << 18)		/* Clock Output Select */
#define US_OVER				(1 << 19)		/* Over Sampling Mode */
#define US_INACK			(1 << 20)		/* Inhibit Non Acknowledge */
#define US_DSNACK			(1 << 21)		/* Disable Successive NACK */
#define US_MAX_ITER			(1 << 24)		/* Number of Repetitions */
#define US_FILTER			(1 << 28)		/* Receive Line Filter */
#define US_MAN				(1 << 29)		/* Manchester Encoder/Decoder Enable */
#define US_ONEBIT			(1u << 31)		/* Start Frame Delimiter Selector */

/* Mode Register model */

/* Standard Asynchronous Mode : 8 bits , 1 stop , no parity */
#define US_ASYNC_MODE		( US_CHMODE_NORMAL + \
							  US_NBSTOP_1 + \
							  US_PAR_NO + \
							  US_CHRL_8 + \
							  US_CLKS_CLOCK )

/* Standard External Asynchronous Mode : 8 bits , 1 stop , no parity */
#define US_ASYNC_EXT_MODE	( US_CHMODE_NORMAL + \
							  US_NBSTOP_1 + \
							  US_PAR_NO + \
							  US_CHRL_8 + \
							  US_CLKS_EXT )

/* Standard Synchronous Mode : 8 bits , 1 stop , no parity */
#define US_SYNC_MODE		( US_SYNC + \
							  US_CHMODE_NORMAL + \
							  US_NBSTOP_1 + \
							  US_PAR_NO + \
							  US_CHRL_8 + \
							  US_CLKS_CLOCK )

/* SCK used Label */
#define SCK_USED			(US_CLKO | US_CLKS_EXT)

/*---------------------------------------------------------------*/
/* US_IER, US_IDR, US_IMR, US_CSR: Status and Interrupt Register */
/*---------------------------------------------------------------*/

#define US_RXRDY			(1 <<  0)	/* Receiver Ready */
#define US_TXRDY			(1 <<  1)	/* Transmitter Ready */
#define US_RXBRK			(1 <<  2)	/* Receiver Break */
#define US_ENDRX			(1 <<  3)	/* End of Receiver PDC Transfer */
#define US_ENDTX			(1 <<  4)	/* End of Transmitter PDC Transfer */
#define US_OVRE				(1 <<  5)	/* Overrun Error */
#define US_FRAME			(1 <<  6)	/* Framing Error */
#define US_PARE				(1 <<  7)	/* Parity Error */
#define US_TIMEOUT			(1 <<  8)	/* Receiver Timeout */
#define US_TXEMPTY			(1 <<  9)	/* Transmitter Empty */
#define US_ITERATION		(1 << 10)	/* Iteration */
#define US_TXBUFFE			(1 << 11)	/* TX Buffer Empty */
#define US_RXBUFF			(1 << 12)	/* RX Buffer Full */
#define US_NACK				(1 << 13)	/* Non Acknowledge */
#define US_RIIC				(1 << 16)	/* Ring Indicator Input Change */
#define US_DSRIC			(1 << 17)	/* Data Set Ready Input Change */
#define US_DCDIC			(1 << 18)	/* Data Carrier Detect Input Change */
#define US_CTSIC			(1 << 19)	/* Clear To Send Input Change */
#define US_MANE				(1 << 20)	/* Manchester Error */

#define US_MASK_IRQ_TX		(US_TXRDY | US_ENDTX | US_TXEMPTY)
#define US_MASK_IRQ_RX		(US_RXRDY | US_ENDRX | US_TIMEOUT)
#define US_MASK_IRQ_ERROR	(US_PARE | US_FRAME | US_OVRE | US_RXBRK)




/*---------------------------------------*/
/* Analog to Digital Converter Registers */
/*---------------------------------------*/
#define ADC_BASE		0xFFFD8000
#define PDC_ADC_BASE	(ADC_BASE + 0x100)

#define ADC_CR		__REG32(ADC_BASE + 0x000)	/* Control Register */
#define ADC_MR		__REG32(ADC_BASE + 0x004)	/* Mode Register */
#define ADC_CHER	__REG32(ADC_BASE + 0x010)	/* Channel Enable Register */
#define ADC_CHDR	__REG32(ADC_BASE + 0x014)	/* Channel Disable Register */
#define ADC_CHSR	__REG32(ADC_BASE + 0x018)	/* Channel Status Register */
#define ADC_SR		__REG32(ADC_BASE + 0x01C)	/* Status Register */
#define ADC_LCDR	__REG32(ADC_BASE + 0x020)	/* Last Converted Data Register */
#define ADC_IER		__REG32(ADC_BASE + 0x024)	/* Interrupt Enable Register */
#define ADC_IDR		__REG32(ADC_BASE + 0x028)	/* Interrupt Disable Register */
#define ADC_IMR		__REG32(ADC_BASE + 0x02C)	/* Interrupt Mask Register */
#define ADC_CDR0	__REG32(ADC_BASE + 0x030)	/* Channel Data Register 0 */
#define ADC_CDR1	__REG32(ADC_BASE + 0x034)	/* Channel Data Register 1 */
#define ADC_CDR2	__REG32(ADC_BASE + 0x038)	/* Channel Data Register 2 */
#define ADC_CDR3	__REG32(ADC_BASE + 0x03C)	/* Channel Data Register 3 */
#define ADC_CDR4	__REG32(ADC_BASE + 0x040)	/* Channel Data Register 4 */
#define ADC_CDR5	__REG32(ADC_BASE + 0x044)	/* Channel Data Register 5 */
#define ADC_CDR6	__REG32(ADC_BASE + 0x048)	/* Channel Data Register 6 */
#define ADC_CDR7	__REG32(ADC_BASE + 0x04C)	/* Channel Data Register 7 */
#define ADC_RPR		__REG32(ADC_BASE + 0x100)	/* Receive Pointer Register */
#define ADC_RCR		__REG32(ADC_BASE + 0x104)	/* Receive Counter Register */
#define ADC_TPR		__REG32(ADC_BASE + 0x108)	/* Transmit Pointer Register */
#define ADC_TCR		__REG32(ADC_BASE + 0x10C)	/* Transmit Counter Register */
#define ADC_RNPR	__REG32(ADC_BASE + 0x110)	/* Receive Next Pointer Register */
#define ADC_RNCR	__REG32(ADC_BASE + 0x114)	/* Receive Next Counter Register */
#define ADC_TNPR	__REG32(ADC_BASE + 0x118)	/* Transmit Next Pointer Register */
#define ADC_TNCR	__REG32(ADC_BASE + 0x11C)	/* Transmit Next Counter Register */
#define ADC_PTCR	__REG32(ADC_BASE + 0x120)	/* PDC Transfer Control Register */
#define ADC_PTSR	__REG32(ADC_BASE + 0x124)	/* PDC Transfer Status Register */


/* ADC_CR: Control Register Bits Definition */
#define ADC_SWRST		(1 << 0)		/* ADC Software Reset */
#define ADC_START		(1 << 1)		/* ADC Start Conversion */

/* ADC_MR: Mode Register Bits Definition */
#define ADC_TRGEN_DIS	(0 << 0)			/* Trigger Disable */
#define ADC_TRGEN_EN	(1 << 0)			/* Trigger Enable */
#define ADC_TTRGSEL		(0x07 << 1)		/* Trigger Selection */
#define ADC_TRGSEL_TIOA0	(0x0 << 1)		/* TIOA0 Trigger Selection */
#define ADC_TRGSEL_TIOA1	(0x1 << 1)		/* TIOA1 Trigger Selection */
#define ADC_TRGSEL_TIOA2	(0x2 << 1)		/* TIOA2 Trigger Selection */
#define ADC_TRGSEL_TIOA3	(0x3 << 1)		/* TIOA3 Trigger Selection */
#define ADC_TRGSEL_TIOA4	(0x4 << 1)		/* TIOA4 Trigger Selection */
#define ADC_TRGSEL_TIOA5	(0x5 << 1)		/* TIOA5 Trigger Selection */
#define ADC_TRGSEL_EXT		(0x6 << 1)		/* External Trigger Selection */
#define ADC_LOWRES		(1 << 4)		/* Resolution */
#define ADC_LOWRES_10_BIT	(0 << 4)		/* 10 bits ADC Resolution */
#define ADC_LOWRES_8_BIT	(1 << 4)		/*  8 bits ADC Resolution */
#define ADC_SLEEP		(1 << 5)		/* Sleep Mode */
#define ADC_SLEEP_NORMAL_MODE	(0 << 5)	/* Normal Mode */
#define ADC_SLEEP_MODE			(1 << 5)	/* Sleep Mode */
#define ADC_PRESCAL		(0x3F << 8)		/* Prescaler rate selection */
#define ADC_B_PRESCAL	8				/* Prescaler bit shift */
#define ADC_STARTUP		(0x1F << 16)	/* Startup Time */
#define ADC_B_STARTUP	16				/* Startup Time bit shift */
#define ADC_SHTIM		(0x0F << 24)	/* Sample & Hold Time */
#define ADC_B_SHTIM		24				/* Sample & Hold Time bit shift */

/* ADC_CHER,ADC_CHDR,ADC_CHSR: Channel Enable, Disable and Status Registers */
#define ADC_CH0			(1 << 0)		/* Channel 0 */
#define ADC_CH1			(1 << 1)		/* Channel 1 */
#define ADC_CH2			(1 << 2)		/* Channel 2 */
#define ADC_CH3			(1 << 3)		/* Channel 3 */
#define ADC_CH4			(1 << 4)		/* Channel 4 */
#define ADC_CH5			(1 << 5)		/* Channel 5 */
#define ADC_CH6			(1 << 6)		/* Channel 6 */
#define ADC_CH7			(1 << 7)		/* Channel 7 */

/* ADC_SR,ADC_IER,ADC_IDR,ADC_IMR: Status, Enable, Disable, Mask Registers */
#define ADC_EOC0		(1 <<  0)		/* End of Conversion Channel 0 */
#define ADC_EOC1		(1 <<  1)		/* End of Conversion Channel 1 */
#define ADC_EOC2		(1 <<  2)		/* End of Conversion Channel 2 */
#define ADC_EOC3		(1 <<  3)		/* End of Conversion Channel 3 */
#define ADC_EOC4		(1 <<  4)		/* End of Conversion Channel 4 */
#define ADC_EOC5		(1 <<  5)		/* End of Conversion Channel 5 */
#define ADC_EOC6		(1 <<  6)		/* End of Conversion Channel 6 */
#define ADC_EOC7		(1 <<  7)		/* End of Conversion Channel 7 */
#define ADC_OVRE0		(1 <<  8)		/* Overrun Error Channel 0 */
#define ADC_OVRE1		(1 <<  9)		/* Overrun Error Channel 1 */
#define ADC_OVRE2		(1 << 10)		/* Overrun Error Channel 2 */
#define ADC_OVRE3		(1 << 11)		/* Overrun Error Channel 3 */
#define ADC_OVRE4		(1 << 12)		/* Overrun Error Channel 4 */
#define ADC_OVRE5		(1 << 13)		/* Overrun Error Channel 5 */
#define ADC_OVRE6		(1 << 14)		/* Overrun Error Channel 6 */
#define ADC_OVRE7		(1 << 15)		/* Overrun Error Channel 7 */
#define ADC_DRDY		(1 << 16)		/* Data Ready */
#define ADC_GOVRE		(1 << 17)		/* General Overrun Error */
#define ADC_ENDRX		(1 << 18)		/* End of Receiver Transfer */
#define ADC_RXBUFF		(1 << 19)		/* RXBUFF Interrupt */

/* ADC_CDRx, ADC_LCDR: Convert Data Register */
#define ADC_DATA_10BITS	0x3FF
#define ADC_DATA_8BITS	0x0FF


/*---------------------------------------*/
/* Serial Peripheral Interface Registers */
/*---------------------------------------*/
#define SPI_BASE		0xFFFE0000
#define PDC_SPI_BASE	(SPI_BASE + 0x100)

#define SPI_CR		__REG32(SPI_BASE + 0x000)	/* Control Register */
#define SPI_MR		__REG32(SPI_BASE + 0x004)	/* Mode Register */
#define SPI_RDR		__REG32(SPI_BASE + 0x008)	/* Receive Data Register */
#define SPI_TDR		__REG32(SPI_BASE + 0x00C)	/* Transmit Data Register */
#define SPI_SR		__REG32(SPI_BASE + 0x010)	/* Status Register */
#define SPI_IER		__REG32(SPI_BASE + 0x014)	/* Interrupt Enable Register */
#define SPI_IDR		__REG32(SPI_BASE + 0x018)	/* Interrupt Disable Register */
#define SPI_IMR		__REG32(SPI_BASE + 0x01C)	/* Interrupt Mask Register */
#define SPI_CSR0	__REG32(SPI_BASE + 0x030)	/* Chip Select Register 0 */
#define SPI_CSR1	__REG32(SPI_BASE + 0x034)	/* Chip Select Register 1 */
#define SPI_CSR2	__REG32(SPI_BASE + 0x038)	/* Chip Select Register 2 */
#define SPI_CSR3	__REG32(SPI_BASE + 0x03C)	/* Chip Select Register 3 */

#define SPI_RPR		__REG32(SPI_BASE + 0x100)	/* Receive Pointer Register */
#define SPI_RCR		__REG32(SPI_BASE + 0x104)	/* Receive Counter Register */
#define SPI_TPR		__REG32(SPI_BASE + 0x108)	/* Transmit Pointer Register */
#define SPI_TCR		__REG32(SPI_BASE + 0x10C)	/* Transmit Counter Register */
#define SPI_RNPR	__REG32(SPI_BASE + 0x110)	/* Receive Next Pointer Register */
#define SPI_RNCR	__REG32(SPI_BASE + 0x114)	/* Receive Next Counter Register */
#define SPI_TNPR	__REG32(SPI_BASE + 0x118)	/* Transmit Next Pointer Register */
#define SPI_TNCR	__REG32(SPI_BASE + 0x11C)	/* Transmit Next Counter Register */
#define SPI_PTCR	__REG32(SPI_BASE + 0x120)	/* PDC Transfer Control Register */
#define SPI_PTSR	__REG32(SPI_BASE + 0x124)	/* PDC Transfer Status Register */

/* SPI_CR: Control Register */
#define SPI_CR_SPIEN		(1 <<  0)	/* SPI Enable */
#define SPI_CR_SPIDIS		(1 <<  1)	/* SPI Disable */
#define SPI_CR_SWRST		(1 <<  7)	/* SPI Software Reset */
#define SPI_CR_LASTXFER		(1 << 24)	/* SPI Last Transfer */

/* SPI_MR: Mode Register */
#define SPI_MR_MSTR			(1 <<  0)	/* Master/Slave Mode (0 = Slave, 1 = Master) */
#define SPI_MR_PS			(1 <<  1)	/* Peripheral Select (0 = Fixed, 1 = Variable) */
#define SPI_MR_PCSDEV		(1 <<  2)	/* Chip Select Decode (0 = direct, 1 = decoded) */
#define SPI_MR_MODFDIS		(1 <<  4)	/* Mode Fault Detection (0 = enabled, 1 = disabled) */
#define SPI_MR_LLB			(1 <<  7)	/* Local Loopback Enable */
#define SPI_MR_PCS_SHFT		16
#define SPI_MR_PCS_MSK		(0x0F << 16)	/* Peripheral Chip Select (for Fixed PS only) */
#define SPI_MR_DLYBCS_SHFT	24
#define SPI_MR_DLYBCS_MSK	(0xFFu << 24)	/* Delay Between Chip Selects */

/* SPI_RDR: Receive Data Register */
#define SPI_RDR_RD_MSK		(0xFFFF << 0)	/* Receive Data */
#define SPI_RDR_PCS_MSK		(0x0F << 16)	/* Peripheral Chip Select (NPCS pin) */

/* SPI_TDR: Transmit Data Register */
#define SPI_TDR_TD_MSK		(0xFFFF << 0)	/* Transmit Data */
#define SPI_TDR_PCS_MSK		(0x0F << 16)	/* Peripheral Chip Select (for Variable PS only) */
#define SPI_TDR_LASTXFER	(1 << 24)		/* Last Transfer (for Variable PS only) */

/* SPI_SR: Status Register */
#define SPI_SR_RDRF			(1 <<  0)	/* Receive Data Register Full */
#define SPI_SR_TDRE			(1 <<  1)	/* Transmit Data Register Empty */
#define SPI_SR_MODF			(1 <<  2)	/* Mode Fault Error */
#define SPI_SR_OVRES		(1 <<  3)	/* Overrun Error Status */
#define SPI_SR_ENDRX		(1 <<  4)	/* End of RX buffer */
#define SPI_SR_ENDTX		(1 <<  5)	/* End of TX buffer */
#define SPI_SR_RXBUFF		(1 <<  6)	/* RX Buffer Full */
#define SPI_SR_TXBUFE		(1 <<  7)	/* TX Buffer Empty */
#define SPI_SR_NSSR			(1 <<  8)	/* NSS Rising */
#define SPI_SR_TXEMPTY		(1 <<  9)	/* Transmission Registers Empty */
#define SPI_SR_SPIENS		(1 << 16)	/* SPI Enable Status */

/* SPI_IER, SPI_IDR, SPI_IMR: Interrupt xxx Register */
#define SPI_IR_RDRF			(1 <<  0)	/* Receive Data Register Full */
#define SPI_IR_TDRE			(1 <<  1)	/* Transmit Data Register Empty */
#define SPI_IR_MODF			(1 <<  2)	/* Mode Fault Error */
#define SPI_IR_OVRES		(1 <<  3)	/* Overrun Error Status */
#define SPI_IR_ENDRX		(1 <<  4)	/* End of RX buffer */
#define SPI_IR_ENDTX		(1 <<  5)	/* End of TX buffer */
#define SPI_IR_RXBUFF		(1 <<  6)	/* RX Buffer Full */
#define SPI_IR_TXBUFE		(1 <<  7)	/* TX Buffer Empty */
#define SPI_IR_NSSR			(1 <<  8)	/* NSS Rising */
#define SPI_IR_TXEMPTY		(1 <<  9)	/* Transmission Registers Empty */

/* SPI_CSRx: SPI Chip Select Registers */
#define SPI_CSR_CPOL		(1 <<  0)	/* Clock Polarity */
#define SPI_CSR_NCPHA		(1 <<  1)	/* Clock Phase */
#define SPI_CSR_CSAAT		(1 <<  3)	/* Chip Select Active After Transfer */
#define SPI_CSR_BITS_SHFT	4
#define SPI_CSR_BITS_MSK	(0x0F << 4)	/* Bits Per Transfer */
#define SPI_CSR_BITS_8		(0x00 << 4)
#define SPI_CSR_BITS_9		(0x01 << 4)
#define SPI_CSR_BITS_10		(0x02 << 4)
#define SPI_CSR_BITS_11		(0x03 << 4)
#define SPI_CSR_BITS_12		(0x04 << 4)
#define SPI_CSR_BITS_13		(0x05 << 4)
#define SPI_CSR_BITS_14		(0x06 << 4)
#define SPI_CSR_BITS_15		(0x07 << 4)
#define SPI_CSR_BITS_16		(0x08 << 4)
#define SPI_CSR_SCBR_SHFT	8
#define SPI_CSR_SCBR_MSK	(0xFF <<  8)	/* Serial Clock Baud Rate */
#define SPI_CSR_DLYBS_SHFT	16
#define SPI_CSR_DLYBS_MSK	(0xFF << 16)	/* Delay Before SPCK */
#define SPI_CSR_DLYBCT_SHFT	24
#define SPI_CSR_DLYBCT_MSK	(0xFFu << 24)	/* Delay Between Consecutive Transfers */

/* SPI_PTCR: PDC Transfer Control Register */
#define SPI_PTCR_RXTEN		(1 << 0)	/* Receiver Transfer Enable */
#define SPI_PTCR_RXTDIS		(1 << 1)	/* Receiver Transfer Disable */
#define SPI_PTCR_TXTEN		(1 << 8)	/* Transmitter Transfer Enable */
#define SPI_PTCR_TXTDIS		(1 << 9)	/* Transmitter Transfer Disable */

/* SPI_PTSR: PDC Transfer Status Register */
#define SPI_PTSR_RXTEN		(1 << 0)	/* Receiver Transfer Enable */
#define SPI_PTSR_TXTEN		(1 << 8)	/* Transmitter Transfer Enable */


/*-------------------------------*/
/* Synchronous Serial Controller */
/*-------------------------------*/
#define SSC_BASE		0xFFFD4000
#define PDC_SSC_BASE	(SSC_BASE + 0x100)

#define SSC_CR		__REG32(SSC_BASE + 0x000)	/* Control Register */
#define SSC_CMR		__REG32(SSC_BASE + 0x004)	/* Clock Mode Register */
#define SSC_RCMR	__REG32(SSC_BASE + 0x010)	/* Receive Clock Mode Register */
#define SSC_RFMR	__REG32(SSC_BASE + 0x014)	/* Receive Frame Mode Register */
#define SSC_TCMR	__REG32(SSC_BASE + 0x018)	/* Transmit Clock Mode Register */
#define SSC_TFMR	__REG32(SSC_BASE + 0x01C)	/* Transmit Frame Mode Register */
#define SSC_RHR		__REG32(SSC_BASE + 0x020)	/* Receive Holding Register */
#define SSC_THR		__REG32(SSC_BASE + 0x024)	/* Transmit Holding Register */
#define SSC_RSHR	__REG32(SSC_BASE + 0x030)	/* Receive Sync Holding Register */
#define SSC_TSHR	__REG32(SSC_BASE + 0x034)	/* Transmit Sync Holding Register */
#define SSC_RC0R	__REG32(SSC_BASE + 0x038)	/* Receive Compare 0 Register */
#define SSC_RC1R	__REG32(SSC_BASE + 0x03C)	/* Receive Compare 1 Register */
#define SSC_SR		__REG32(SSC_BASE + 0x040)	/* Status Register */
#define SSC_IER		__REG32(SSC_BASE + 0x044)	/* Interrupt Enable Register */
#define SSC_IDR		__REG32(SSC_BASE + 0x048)	/* Interrupt Disable Register */
#define SSC_IMR		__REG32(SSC_BASE + 0x04C)	/* Interrupt Mask Register */
#define SSC_RPR		__REG32(SSC_BASE + 0x100)	/* Receive Pointer Register */
#define SSC_RCR		__REG32(SSC_BASE + 0x104)	/* Receive Counter Register */
#define SSC_TPR		__REG32(SSC_BASE + 0x108)	/* Transmit Pointer Register */
#define SSC_TCR		__REG32(SSC_BASE + 0x10C)	/* Transmit Counter Register */
#define SSC_RNPR	__REG32(SSC_BASE + 0x110)	/* Receive Next Pointer Register */
#define SSC_RNCR	__REG32(SSC_BASE + 0x114)	/* Receive Next Counter Register */
#define SSC_TNPR	__REG32(SSC_BASE + 0x118)	/* Transmit Next Pointer Register */
#define SSC_TNCR	__REG32(SSC_BASE + 0x11C)	/* Transmit Next Counter Register */
#define SSC_PTCR	__REG32(SSC_BASE + 0x120)	/* PDC Transfer Control Register */
#define SSC_PTSR	__REG32(SSC_BASE + 0x124)	/* PDC Transfer Status Register */

/* SSC_CR: SSC Control Register */
#define SSC_CR_RXEN		(1 <<  0)		/* Receive Enable */
#define SSC_CR_RXDIS	(1 <<  1)		/* Receive Disable */
#define SSC_CR_TXEN		(1 <<  8)		/* Transmit Enable */
#define SSC_CR_TXDIS	(1 <<  9)		/* Transmit Disable */
#define SSC_CR_SWRST	(1 << 15)		/* Software Reset */

/* SSC_CMR: SSC Clock Mode Register */
#define SSC_CMR_DIV		(0xFFF << 0)	/* Clock Divider */

/* SSC_RCMR: SSC Receive Clock Mode Register */
#define SSC_RCMR_CKS	(0x03 <<  0)	/* Receive Clock Selection */
#define SSC_RCMR_CKS_DIVC	(0 << 0)		/* Divided Clock */
#define SSC_RCMR_CKS_TK		(1 << 0)		/* TK Clock signal */
#define SSC_RCMR_CKS_RK		(2 << 0)		/* RK pin */
#define SSC_RCMR_CKO	(0x07 <<  2)	/* Receive Clock Output Mode Selection */
#define SSC_RCMR_CKO_NONE	(0 << 2)		/* None (RK pin input-only) */
#define SSC_RCMR_CKO_CONT	(1 << 2)		/* Continuous Receive Clock */
#define SSC_RCMR_CKO_TRAN	(2 << 2)		/* Receive Clock only during data transfers */
#define SSC_RCMR_CKI	(1 << 5)		/* Receive Clock Inversion */
#define SSC_RCMR_CKG	(0x03 <<  6)	/* Receive Clock Gating */
#define SSC_RCMR_CKG_NONE	(0 << 6)		/* None, continuous clock */
#define SSC_RCMR_CKG_RFL	(1 << 6)		/* Receive Clock enabled only if RF Low */
#define SSC_RCMR_CKG_RFH	(2 << 6)		/* Receive Clock enabled only if RF High */
#define SSC_RCMR_START	(0x0F <<  8)	/* Receive Start Selection */
#define SSC_RCMR_START_CONT	(0 << 8)		/* Continuous, as soon as receiver is enabled */
#define SSC_RCMR_START_TS	(1 << 8)		/* Transmit start */
#define SSC_RCMR_START_LL	(2 << 8)		/* Low Level on RF signal */
#define SSC_RCMR_START_HL	(3 << 8)		/* High Level on RF signal */
#define SSC_RCMR_START_FE	(4 << 8)		/* Falling Edge on RF signal */
#define SSC_RCMR_START_RE	(5 << 8)		/* Rising Edge on RF signal */
#define SSC_RCMR_START_AL	(6 << 8)		/* Any Level change on RF signal */
#define SSC_RCMR_START_AE	(7 << 8)		/* Any Edge on RF signal */
#define SSC_RCMR_START_CMP0	(8 << 8)		/* Compare 0 */
#define SSC_RCMR_STOP	(1 << 12)		/* Receive Stop Selection */
#define SSC_RCMR_STDDLY	(0xFF << 16)	/* Receive Start Delay */
#define SSC_RCMR_STDDLY_SHFT	16		/* bit position of field STDDLY */
#define SSC_RCMR_PERIOD	(0xFFu << 24)	/* Receive Period Divider Selection */
#define SSC_RCMR_PERIOD_SHFT	24		/* bit position of field PERIOD */

/* SSC_RFMR: SSC Receive Frame Mode Register */
#define SSC_RFMR_DATLEN	(0x1F << 0)		/* Data Length */
#define SSC_RFMR_LOOP	(1 << 5)		/* Loop Mode (0=normal) */
#define SSC_RFMR_MSBF	(1 << 7)		/* Most Significant Bit First */
#define SSC_RFMR_DATNB	(0x0F << 8)		/* Data Number per Frame */
#define SSC_RFMR_FSLEN	(0x0F << 16)	/* Receive Frame Sync Length */
#define SSC_RFMR_FSOS	(0x07 << 20)	/* Receive Frame Sync Output Selection */
#define SSC_RFMR_FSEDGE	(1 << 24)		/* Frame Sync Edge Detection */

/* SSC_TCMR: SSC Transmit Clock Mode Register */
#define SSC_TCMR_CKS	(0x03 <<  0)	/* Transmit Clock Selection */
#define SSC_TCMR_CKS_DIVC	(0 << 0)		/* Divided Clock */
#define SSC_TCMR_CKS_RK		(1 << 0)		/* RK Clock signal */
#define SSC_TCMR_CKS_TK		(2 << 0)		/* TK pin */
#define SSC_TCMR_CKO	(0x07 <<  2)	/* Transmit Clock Output Mode Selection */
#define SSC_TCMR_CKO_NONE	(0 << 2)		/* None (TK pin input-only) */
#define SSC_TCMR_CKO_CONT	(1 << 2)		/* Continuous Transmit Clock */
#define SSC_TCMR_CKO_TRAN	(2 << 2)		/* Transmit Clock only during data transfers */
#define SSC_TCMR_CKI	(1 << 5)		/* Transmit Clock Inversion */
#define SSC_TCMR_CKG	(0x03 <<  6)	/* Transmit Clock Gating */
#define SSC_TCMR_CKG_NONE	(0 << 6)		/* None, continuous clock */
#define SSC_TCMR_CKG_TFL	(1 << 6)		/* Transmit Clock enabled only if TF Low */
#define SSC_TCMR_CKG_TFH	(2 << 6)		/* Transmit Clock enabled only if TF High */
#define SSC_TCMR_START	(0x0F <<  8)	/* Transmit Start Selection */
#define SSC_TCMR_START_CONT	(0 << 8)		/* Continuous, as soon as SSC_THR is written */
#define SSC_TCMR_START_RS	(1 << 8)		/* Receive start */
#define SSC_TCMR_START_LL	(2 << 8)		/* Low Level on TF signal */
#define SSC_TCMR_START_HL	(3 << 8)		/* High Level on TF signal */
#define SSC_TCMR_START_FE	(4 << 8)		/* Falling Edge on TF signal */
#define SSC_TCMR_START_RE	(5 << 8)		/* Rising Edge on TF signal */
#define SSC_TCMR_START_AL	(6 << 8)		/* Any Level change on TF signal */
#define SSC_TCMR_START_AE	(7 << 8)		/* Any Edge on TF signal */
#define SSC_TCMR_STDDLY	(0xFF << 16)	/* Transmit Start Delay */
#define SSC_TCMR_STDDLY_SHFT	16		/* bit position of field STDDLY */
#define SSC_TCMR_PERIOD	(0xFF << 24)	/* Transmit Period Divider Selection */
#define SSC_TCMR_PERIOD_SHFT	24		/* bit position of field PERIOD */

/* SSC_TFMR: SSC Transmit Frame Mode Register */
#define SSC_TFMR_DATLEN	(0x1F << 0)		/* Data Length */
#define SSC_TFMR_DATDEF	(1 << 5)		/* Data Default Value */
#define SSC_TFMR_MSBF	(1 << 7)		/* Most Significant Bit First */
#define SSC_TFMR_DATNB	(0x0F << 8)		/* Data Number per Frame */
#define SSC_TFMR_FSLEN	(0x0F << 16)	/* Transmit Frame Sync Length */
#define SSC_TFMR_FSOS	(0x07 << 20)	/* Transmit Frame Sync Output Selection */
#define SSC_TFMR_FSDEN	(1 << 23)		/* Frame Sync Data Enable */
#define SSC_TFMR_FSEDGE	(1 << 24)		/* Frame Sync Edge Detection */

/* SSC_SR: SSC Status Register */
#define SSC_SR_TXRDY	(1 <<  0)		/* Transmit Ready */
#define SSC_SR_TXEMPTY	(1 <<  1)		/* Transmit Empty */
#define SSC_SR_ENDTX	(1 <<  2)		/* End of Transmission */
#define SSC_SR_TXBUFE	(1 <<  3)		/* Transmit Buffer Empty */
#define SSC_SR_RXRDY	(1 <<  4)		/* Receive Ready */
#define SSC_SR_OVRUN	(1 <<  5)		/* Receive Overrun */
#define SSC_SR_ENDRX	(1 <<  6)		/* End of Reception */
#define SSC_SR_RXBUFF	(1 <<  7)		/* Receive Buffer Full */
#define SSC_SR_CP0		(1 <<  8)		/* Compare 0 */
#define SSC_SR_CP1		(1 <<  9)		/* Compare 1 */
#define SSC_SR_TXSYN	(1 << 10)		/* Transmit Sync */
#define SSC_SR_RXSYN	(1 << 11)		/* Receive Sync */
#define SSC_SR_TXEN		(1 << 16)		/* Transmit Enable */
#define SSC_SR_RXEN		(1 << 17)		/* Receive Enable */

/* SSC_IR: SSC Interrupt Register (IER,IDR,IMR) */
#define SSC_IR_TXRDY	(1 <<  0)		/* Transmit Ready */
#define SSC_IR_TXEMPTY	(1 <<  1)		/* Transmit Empty */
#define SSC_IR_ENDTX	(1 <<  2)		/* End of Transmission */
#define SSC_IR_TXBUFE	(1 <<  3)		/* Transmit Buffer Empty */
#define SSC_IR_RXRDY	(1 <<  4)		/* Receive Ready */
#define SSC_IR_OVRUN	(1 <<  5)		/* Receive Overrun */
#define SSC_IR_ENDRX	(1 <<  6)		/* End of Reception */
#define SSC_IR_RXBUFF	(1 <<  7)		/* Receive Buffer Full */
#define SSC_IR_CP0		(1 <<  8)		/* Compare 0 */
#define SSC_IR_CP1		(1 <<  9)		/* Compare 1 */
#define SSC_IR_TXSYN	(1 << 10)		/* Transmit Sync */
#define SSC_IR_RXSYN	(1 << 11)		/* Receive Sync */


/*--------------------*/
/* Two-Wire Interface */
/*--------------------*/
#define TWI_BASE	0xFFFB8000
#define TWI_CR		__REG32(TWI_BASE + 0x00)	/* Control Register */
#define TWI_MMR		__REG32(TWI_BASE + 0x04)	/* Master Mode Register */
#define TWI_IADR	__REG32(TWI_BASE + 0x0C)	/* Internal Address Register */
#define TWI_CWGR	__REG32(TWI_BASE + 0x10)	/* Clock Waveform Generator Register */
#define TWI_SR		__REG32(TWI_BASE + 0x20)	/* Status Register */
#define TWI_IER		__REG32(TWI_BASE + 0x24)	/* Interrupt Enable Register */
#define TWI_IDR		__REG32(TWI_BASE + 0x28)	/* Interrupt Disable Register */
#define TWI_IMR		__REG32(TWI_BASE + 0x2C)	/* Interrupt Mask Register */
#define TWI_RHR		__REG32(TWI_BASE + 0x30)	/* Receive Holding Register */
#define TWI_THR		__REG32(TWI_BASE + 0x34)	/* Transmit Holding Register */

/* TWI_CR: TWI Control Register */
#define TWI_CR_START	(1 << 0)		/* Send a START Condition */
#define TWI_CR_STOP		(1 << 1)		/* Send a STOP Condition */
#define TWI_CR_MSEN		(1 << 2)		/* TWI Master Transfer Enabled */
#define TWI_CR_MSDIS	(1 << 3)		/* TWI Master Transfer Disabled */
#define TWI_CR_SWRST	(1 << 7)		/* Software Reset */

/* TWI_MMR: TWI Master Mode Register */
#define TWI_MMR_IADRSZ	(0x03 <<  8)	/* Internal Device Address Size */
#define TWI_MMR_IADRSZ_0	(0 << 8)		/* No internal device address (byte command) */
#define TWI_MMR_IADRSZ_1	(1 << 8)		/* One-byte internal device address */
#define TWI_MMR_IADRSZ_2	(2 << 8)		/* Two-byte internal device address */
#define TWI_MMR_IADRSZ_3	(3 << 8)		/* Three-byte internal device address */
#define TWI_MMR_MREAD	(1 << 12)		/* Master Read Direction (0=Write,1=Read) */
#define TWI_MMR_DADR	(0x7F << 16)	/* Device Address */
#define TWI_MMR_DADR_SHFT	16			/* bit position of DADR field */

/* TWI_CWGR: TWI Clock Waveform Generator Register */
#define TWI_CWGR_CLDIV	(0xFF <<  0)	/* Clock Low Divider (SCL low period) */
#define TWI_CWGR_CHDIV	(0xFF <<  8)	/* Clock High Divider (SCL high period) */
#define TWI_CWGR_CHDIV_SHFT	8
#define TWI_CWGR_CKDIV	(0x07 << 16)	/* Clock Divider */
#define TWI_CWGR_CKDIV_SHFT	16

/* TWI_SR: TWI Status Register (also: TWI_IER,TWI_IDR,TWI_IMR) */
#define TWI_SR_TXCOMP	(1 << 0)		/* Transmission Completed */
#define TWI_SR_RXRDY	(1 << 1)		/* Receive Holding Register Ready */
#define TWI_SR_TXRDY	(1 << 2)		/* Transmit Holding Register Ready */
#define TWI_SR_OVRE		(1 << 6)		/* Overrun Error */
#define TWI_SR_UNRE		(1 << 7)		/* Underrun Error */
#define TWI_SR_NACK		(1 << 8)		/* Not Acknowledged */


/*-----------------*/
/* USB Device Port */
/*-----------------*/
#define UDP_BASE	0xFFFB0000
#define UDP_FRM_NUM		__REG32(UDP_BASE + 0x000)	/* Frame Number Register */
#define UDP_GLB_STAT	__REG32(UDP_BASE + 0x004)	/* Global State Register */
#define UDP_FADDR		__REG32(UDP_BASE + 0x008)	/* Function Address Register */
#define UDP_IER			__REG32(UDP_BASE + 0x010)	/* Interrupt Enable Register */
#define UDP_IDR			__REG32(UDP_BASE + 0x014)	/* Interrupt Disable Register */
#define UDP_IMR			__REG32(UDP_BASE + 0x018)	/* Interrupt Mask Register */
#define UDP_ISR			__REG32(UDP_BASE + 0x01C)	/* Interrupt Status Register */
#define UDP_ICR			__REG32(UDP_BASE + 0x020)	/* Interrupt Clear Register */
#define UDP_RST_EP		__REG32(UDP_BASE + 0x028)	/* Reset Endpoint Register */
#define UDP_CSR0		__REG32(UDP_BASE + 0x030)	/* EP0 Control and Status Register */
#define UDP_CSR1		__REG32(UDP_BASE + 0x034)	/* EP1 Control and Status Register */
#define UDP_CSR2		__REG32(UDP_BASE + 0x038)	/* EP2 Control and Status Register */
#define UDP_CSR3		__REG32(UDP_BASE + 0x03C)	/* EP3 Control and Status Register */
#define UDP_FDR0		__REG32(UDP_BASE + 0x050)	/* EP0 FIFO Data Register */
#define UDP_FDR1		__REG32(UDP_BASE + 0x054)	/* EP1 FIFO Data Register */
#define UDP_FDR2		__REG32(UDP_BASE + 0x058)	/* EP2 FIFO Data Register */
#define UDP_FDR3		__REG32(UDP_BASE + 0x05C)	/* EP3 FIFO Data Register */
#define UDP_TXVC		__REG32(UDP_BASE + 0x074)	/* Transceiver Control Register */

/* UDP_FRM_NUM: Frame Number Register */
#define UDP_FRM_NUM_FRM_NUM	(0x7FF << 0)	/* Frame Number */
#define UDP_FRM_NUM_FRM_ERR	(1 << 16)		/* Frame Error */
#define UDP_FRM_NUM_FRM_OK	(1 << 17)		/* Frame OK */

/* UDP_GLB_STAT: Global State Register */
#define UDP_GLB_STAT_FADDEN		(1 << 0)	/* Function Address Enable */
#define UDP_GLB_STAT_CONFG		(1 << 1)	/* Configured */
#define UDP_GLB_STAT_ESR		(1 << 2)	/* Enable Send Resume */
#define UDP_GLB_STAT_RSMINPR	(1 << 3)	/* Resume in process */
#define UDP_GLB_STAT_RMWUPE		(1 << 4)	/* Remote Wake Up Enable */

/* UDP_FADDR: Function Address Register */
#define UDP_FADDR_FADD		(0x7F << 0)		/* Function Address Value */
#define UDP_FADDR_FEN		(1 << 8)		/* Function Enable */

/* UDP_IER,IDR,IMR,ISR,ICR: Interrupt Register */
/* ENDBUSRES only in ISR and ICR */
/* EPxINT not in ICR */
#define UDP_EP0INT		(1 <<  0)			/* EP0 interrupt */
#define UDP_EP1INT		(1 <<  1)			/* EP1 interrupt */
#define UDP_EP2INT		(1 <<  2)			/* EP2 interrupt */
#define UDP_EP3INT		(1 <<  3)			/* EP3 interrupt */
#define UDP_RXSUSP		(1 <<  8)			/* UDP Suspend interrupt */
#define UDP_RXRSM		(1 <<  9)			/* UDP Resume interrupt */
#define UDP_EXTRSM		(1 << 10)			/* UDP External Resume interrupt */
#define UDP_SOFINT		(1 << 11)			/* UDP Start of Frame interrupt */
#define UDP_ENDBUSRES	(1 << 12)			/* UDP End of BUS Reset interrupt */
#define UDP_WAKEUP		(1 << 13)			/* UDP Wakeup interrupt */

/* UDP_RST_EP: UDP Reset Endpoint Register */
#define UDP_RST_EP_EP0	(1 << 0)
#define UDP_RST_EP_EP1	(1 << 1)
#define UDP_RST_EP_EP2	(1 << 2)
#define UDP_RST_EP_EP3	(1 << 3)

/* UDP_CSRx: UDP Endpoint Control and Status Register */
#define UDP_CSR_TXCOMP		(1 <<  0)		/* Transmit Complete */
#define UDP_CSR_RX_DATA_BK0	(1 <<  1)		/* Receive Data Bank 0 */
#define UDP_CSR_RXSETUP		(1 <<  2)		/* SETUP packet (only Control EPs) */
#define UDP_CSR_STALLSENT	(1 <<  3)		/* Stall Sent */
#define UDP_CSR_TXPKTRDY	(1 <<  4)		/* Transmit Packet Ready */
#define UDP_CSR_FORCESTALL	(1 <<  5)		/* Force Stall */
#define UDP_CSR_RX_DATA_BK1	(1 <<  6)		/* Receive Data Bank 1 */
#define UDP_CSR_DIR			(1 <<  7)		/* Transfer Direction (only Control EPs) */
#define UDP_CSR_EPTYPE		(0x07 << 8)		/* Endpoint Type */
#define UDP_CSR_EPTYPE_CONTROL	(0 << 8)	/* Control Endpoint */
#define UDP_CSR_EPTYPE_ISO_OUT	(1 << 8)	/* Isochronous OUT Endpoint */
#define UDP_CSR_EPTYPE_ISO_IN	(5 << 8)	/* Isochronous IN Endpoint */
#define UDP_CSR_EPTYPE_BULK_OUT	(2 << 8)	/* Bulk OUT Endpoint */
#define UDP_CSR_EPTYPE_BULK_IN	(6 << 8)	/* Bulk IN Endpoint */
#define UDP_CSR_EPTYPE_INT_OUT	(3 << 8)	/* Interrupt OUT Endpoint */
#define UDP_CSR_EPTYPE_INT_IN	(7 << 8)	/* Interrupt IN Endpoint */
#define UDP_CSR_DTGLE		(1 << 11)		/* Data Toggle */
#define UDP_CSR_EPEDS		(1 << 15)		/* Endpoint Enable Disable */
#define UDP_CSR_RXBYTECNT	(0x7FF << 16)	/* Number of Bytes Available in FIFO */

/* UDP_FDRx: UDP FIFO Data Register */
#define UDP_FDR_FIFO_DATA	(0xFF << 0)		/* FIFO Data Value */

/* UDP_TXVC: UDP Transceiver Control Register */
#define UDP_TXVC_TXVDIS		(1 << 8)		/* Transceiver Disable */


/*----------------*/
/* PWM Controller */
/*----------------*/
#define PWMC_BASE	0xFFFCC000
#define PWM_MR		__REG32(PWMC_BASE + 0x000)	/* PWM Mode Register */
#define PWM_ENA		__REG32(PWMC_BASE + 0x004)	/* PWM Enable Register */
#define PWM_DIS		__REG32(PWMC_BASE + 0x008)	/* PWM Disable Register */
#define PWM_SR		__REG32(PWMC_BASE + 0x00C)	/* PWM Status Register */
#define PWM_IER		__REG32(PWMC_BASE + 0x010)	/* PWM Interrupt Enable Register */
#define PWM_IDR		__REG32(PWMC_BASE + 0x014)	/* PWM Interrupt Disable Register */
#define PWM_IMR		__REG32(PWMC_BASE + 0x018)	/* PWM Interrupt Mask Register */
#define PWM_ISR		__REG32(PWMC_BASE + 0x01C)	/* PWM Interrupt Status Register */
#define PWM_CMR0	__REG32(PWMC_BASE + 0x200)	/* Channel 0 Mode Register */
#define PWM_CDTY0	__REG32(PWMC_BASE + 0x204)	/* Channel 0 Duty Cycle Register */
#define PWM_CPRD0	__REG32(PWMC_BASE + 0x208)	/* Channel 0 Period Register */
#define PWM_CCNT0	__REG32(PWMC_BASE + 0x20C)	/* Channel 0 Counter Register */
#define PWM_CUPD0	__REG32(PWMC_BASE + 0x210)	/* Channel 0 Update Register */
#define PWM_CMR1	__REG32(PWMC_BASE + 0x220)	/* Channel 1 Mode Register */
#define PWM_CDTY1	__REG32(PWMC_BASE + 0x224)	/* Channel 1 Duty Cycle Register */
#define PWM_CPRD1	__REG32(PWMC_BASE + 0x228)	/* Channel 1 Period Register */
#define PWM_CCNT1	__REG32(PWMC_BASE + 0x22C)	/* Channel 1 Counter Register */
#define PWM_CUPD1	__REG32(PWMC_BASE + 0x230)	/* Channel 1 Update Register */
#define PWM_CMR2	__REG32(PWMC_BASE + 0x240)	/* Channel 2 Mode Register */
#define PWM_CDTY2	__REG32(PWMC_BASE + 0x244)	/* Channel 2 Duty Cycle Register */
#define PWM_CPRD2	__REG32(PWMC_BASE + 0x248)	/* Channel 2 Period Register */
#define PWM_CCNT2	__REG32(PWMC_BASE + 0x24C)	/* Channel 2 Counter Register */
#define PWM_CUPD2	__REG32(PWMC_BASE + 0x250)	/* Channel 2 Update Register */
#define PWM_CMR3	__REG32(PWMC_BASE + 0x260)	/* Channel 3 Mode Register */
#define PWM_CDTY3	__REG32(PWMC_BASE + 0x264)	/* Channel 3 Duty Cycle Register */
#define PWM_CPRD3	__REG32(PWMC_BASE + 0x268)	/* Channel 3 Period Register */
#define PWM_CCNT3	__REG32(PWMC_BASE + 0x26C)	/* Channel 3 Counter Register */
#define PWM_CUPD3	__REG32(PWMC_BASE + 0x270)	/* Channel 3 Update Register */

/* PWM_MR: PWM Mode Register */
#define PWM_MR_DIVA		(0xFF <<  0)	/* CLKA Divide Factor */
#define PWM_MR_PREA		(0x0F <<  8)	/* CLKA Divider Input Clock */
#define PWM_MR_DIVA_SHFT	0
#define PWM_MR_PREA_SHFT	8
#define PWM_MR_DIVB		(0xFF << 16)	/* CLKB Divide Factor */
#define PWM_MR_PREB		(0x0F << 24)	/* CLKB Divider Input Clock */
#define PWM_MR_DIVB_SHFT	16
#define PWM_MR_PREB_SHFT	24
/* Divider Input Clock values */
#define PWM_MR_PRE_MCK		(0x00)		/* MCK */
#define PWM_MR_PRE_MCK2		(0x01)		/* MCK/2 */
#define PWM_MR_PRE_MCK4		(0x02)		/* MCK/4 */
#define PWM_MR_PRE_MCK8		(0x03)		/* MCK/8 */
#define PWM_MR_PRE_MCK16	(0x04)		/* MCK/16 */
#define PWM_MR_PRE_MCK32	(0x05)		/* MCK/32 */
#define PWM_MR_PRE_MCK64	(0x06)		/* MCK/64 */
#define PWM_MR_PRE_MCK128	(0x07)		/* MCK/128 */
#define PWM_MR_PRE_MCK256	(0x08)		/* MCK/256 */
#define PWM_MR_PRE_MCK512	(0x09)		/* MCK/512 */
#define PWM_MR_PRE_MCK1024	(0x0A)		/* MCK/1024 */

/* Channel IDs (PWM_ENA, PWM_DIS, PWM_SR, PWM_IER, PWM_IDR, PWM_IMR, PWM_ISR) */
#define PWM_CHID0		(1 << 0)		/* PWM channel 0 */
#define PWM_CHID1		(1 << 1)		/* PWM channel 1 */
#define PWM_CHID2		(1 << 2)		/* PWM channel 2 */
#define PWM_CHID3		(1 << 3)		/* PWM channel 3 */

/* PWM Channel Mode Registers (PWM_CMRx) */
#define PWM_CMR_CPRE		(0x0F << 0)	/* Channel Pre-scaler */
#define PWM_CMR_CPRE_MCK		(0x00)		/* MCK */
#define PWM_CMR_CPRE_MCK2		(0x01)		/* MCK/2 */
#define PWM_CMR_CPRE_MCK4		(0x02)		/* MCK/4 */
#define PWM_CMR_CPRE_MCK8		(0x03)		/* MCK/8 */
#define PWM_CMR_CPRE_MCK16		(0x04)		/* MCK/16 */
#define PWM_CMR_CPRE_MCK32		(0x05)		/* MCK/32 */
#define PWM_CMR_CPRE_MCK64		(0x06)		/* MCK/64 */
#define PWM_CMR_CPRE_MCK128		(0x07)		/* MCK/128 */
#define PWM_CMR_CPRE_MCK256		(0x08)		/* MCK/256 */
#define PWM_CMR_CPRE_MCK512		(0x09)		/* MCK/512 */
#define PWM_CMR_CPRE_MCK1024	(0x0A)		/* MCK/1024 */
#define PWM_CMR_CPRE_CLKA		(0x0B)		/* CLKA */
#define PWM_CMR_CPRE_CLKB		(0x0C)		/* CLKB */
#define PWM_CMR_CALG		(1 <<  8)	/* Channel Alignment (1=Center,0=Left) */
#define PWM_CMR_CPOL		(1 <<  9)	/* Channel Polarity (1=High,0=Low) */
#define PWM_CMR_CPD			(1 << 10)	/* Channel Update Period (0=update Duty Cycle) */


#endif /* __AT91SAM7SX_H__ */
