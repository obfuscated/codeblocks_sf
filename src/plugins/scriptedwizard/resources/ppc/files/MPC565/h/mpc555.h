/*====================================================================
* Project: Board Support Package (BSP)
* Developed using:
* Function: SFR definitions for MPC5xx
*           The names with double underscore are used in assembler parts
*           the other can be used to access these registers in C/C++
*
* Copyright HighTec EDV-Systeme GmbH 1982-2007
*====================================================================*/
#ifndef __MPC555_H__
#define __MPC555_H__

#if defined HIMO_DEBUG
#define INTERNAL_RAM_BASE	0x1000000
#else
#define INTERNAL_RAM_BASE	0x1000000
#endif

#define __SIUMCR	(INTERNAL_RAM_BASE + 0x2fc000)
#define __SYPCR		(INTERNAL_RAM_BASE + 0x2fc004)
#define __SIPEND	(INTERNAL_RAM_BASE + 0x2fc010)
#define __SIMASK	(INTERNAL_RAM_BASE + 0x2fc014)
#define __SIEL		(INTERNAL_RAM_BASE + 0x2fc018)
#define __SIVEC		(INTERNAL_RAM_BASE + 0x2fc01c)
#define __BR0		(INTERNAL_RAM_BASE + 0x2fc100)
#define __OR0		(INTERNAL_RAM_BASE + 0x2fc104)
#define __BR1		(INTERNAL_RAM_BASE + 0x2fc108)
#define __OR1		(INTERNAL_RAM_BASE + 0x2fc10c)
#define __BR2		(INTERNAL_RAM_BASE + 0x2fc110)
#define __OR2		(INTERNAL_RAM_BASE + 0x2fc114)
#define __BR3		(INTERNAL_RAM_BASE + 0x2fc118)
#define __OR3		(INTERNAL_RAM_BASE + 0x2fc11c)
#define __DMBR		(INTERNAL_RAM_BASE + 0x2fc140)
#define __DMOR		(INTERNAL_RAM_BASE + 0x2fc144)
#define __PISCR		(INTERNAL_RAM_BASE + 0x2fc240)
#define __PITC		(INTERNAL_RAM_BASE + 0x2fc244)
#define __PITR		(INTERNAL_RAM_BASE + 0x2fc248)
#define __SCCR		(INTERNAL_RAM_BASE + 0x2fc280)
#define __PLPRCR	(INTERNAL_RAM_BASE + 0x2fc284)
#define __PLPRCRK	(INTERNAL_RAM_BASE + 0x2fc384)
#define __TBSCR		(INTERNAL_RAM_BASE + 0x2fc200)

#define __DPTMCR	(INTERNAL_RAM_BASE + 0x300000)
#define __RAMBAR	(INTERNAL_RAM_BASE + 0x300004)
#define __PORTQS	(INTERNAL_RAM_BASE + 0x305014)
#define __PQSPAR	(INTERNAL_RAM_BASE + 0x305016)
#define __MPIOSMDR	(INTERNAL_RAM_BASE + 0x306100)
#define __MPIOSMDDR	(INTERNAL_RAM_BASE + 0x306102)
#define __MIOS1TPCR	(INTERNAL_RAM_BASE + 0x306800)
#define __SRAMMCR	(INTERNAL_RAM_BASE + 0x380000)
#define __SGPIODT1	(INTERNAL_RAM_BASE + 0x2fc024)
#define __SGPIODT2	(INTERNAL_RAM_BASE + 0x2fc028)
#define __SGPIOCR	(INTERNAL_RAM_BASE + 0x2fc02c)
#define __EMCR		(INTERNAL_RAM_BASE + 0x2fc030)
#define __UMCR		(INTERNAL_RAM_BASE + 0x307f80)
#define __QDSCI_IL	(INTERNAL_RAM_BASE + 0x305004)

#define SIUMCR		(* (volatile unsigned int *)	__SIUMCR)
#define SYPCR		(* (volatile unsigned int *)	__SYPCR)
#define SIPEND		(* (volatile unsigned short *)	__SIPEND)
#define SIMASK		(* (volatile unsigned short *)	__SIMASK)
#define SIEL		(* (volatile unsigned short *)	__SIEL)
#define SIVEC		(* (volatile unsigned char *)	__SIVEC)
#define BR0			(* (volatile unsigned int *)	__BR0)
#define OR0			(* (volatile unsigned int *)	__OR0)
#define BR1			(* (volatile unsigned int *)	__BR1)
#define OR1			(* (volatile unsigned int *)	__OR1)
#define BR2			(* (volatile unsigned int *)	__BR2)
#define OR2			(* (volatile unsigned int *)	__OR2)
#define BR3			(* (volatile unsigned int *)	__BR3)
#define OR3			(* (volatile unsigned int *)	__OR3)
#define DMBR		(* (volatile unsigned int *)	__DMBR)
#define DMOR		(* (volatile unsigned int *)	__DMOR)
#define PISCR		(* (volatile unsigned short *)	__PISCR)
#define PITC		(* (volatile unsigned short *)	__PITC)
#define PITR		(* (volatile unsigned short *)	__PITR)
#define SCCR		(* (volatile unsigned int *)	__SCCR)
#define PLPRCR		(* (volatile unsigned int *)	__PLPRCR)
#define PLPRCRK		(* (volatile unsigned int *)	__PLPRCRK)
#define TBSCR		(* (volatile unsigned short *)  __TBSCR)

#define DPTMCR		(* (volatile unsigned int *)	__DPTMCR)
#define RAMBAR		(* (volatile unsigned int *)	__RAMBAR)
#define PORTQS		(* (volatile unsigned int *)	__PORTQS)
#define PQSPAR		(* (volatile unsigned short *)	__PQSPAR)
#define MPIOSMDR	(* (volatile unsigned short *)  __MPIOSMDR)
#define MPIOSMDDR	(* (volatile unsigned short *)  __MPIOSMDDR)
#define MIOS1TPCR	(* (volatile unsigned int *)	__MIOS1TPCR)
#define SRAMMCR		(* (volatile unsigned int *)	__SRAMMCR)
#define SGPIODT1	(* (volatile unsigned int *)	__SGPIODT1)
#define SGPIODT2	(* (volatile unsigned int *)	__SGPIODT2)
#define SGPIOCR		(* (volatile unsigned int *)	__SGPIOCR)
#define EMCR		(* (volatile unsigned int *)	__EMCR)
#define UMCR		(* (volatile unsigned int *)	__UMCR)
	/* Dual SCI Interrupt Level */
#define QDSCI_IL	(*(volatile short *)__QDSCI_IL)

//SPR defines
#define SPR_SRR1			27
#define SPR_IMMR			638
#define SPR_BBCMCR			560

/* Refer to MPC555 User's Manual March 1, 1999 revision
 	section 6.13.2.1 and 6.13.2.4 for more info */

/* SIPEND (SIU Interrupt Pending Register ) */
#define SIPEND_IRQ0		0x8000		/* IRQ 0 */
#define SIPEND_LVL0		0x4000		/* LVL 0 */
#define SIPEND_IRQ1		0x2000		/* IRQ 1 */
#define SIPEND_LVL1		0x1000		/* LVL 1 */
#define SIPEND_IRQ2		0x0800		/* IRQ 2 */
#define SIPEND_LVL2		0x0400		/* LVL 2 */
#define SIPEND_IRQ3		0x0200		/* IRQ 3 */
#define SIPEND_LVL3		0x0100		/* LVL 3 */
#define SIPEND_IRQ4		0x0080		/* IRQ 4 */
#define SIPEND_LVL4		0x0040		/* LVL 4 */
#define SIPEND_IRQ5		0x0020		/* IRQ 5 */
#define SIPEND_LVL5		0x0010		/* LVL 5 */
#define SIPEND_IRQ6		0x0008		/* IRQ 6 */
#define SIPEND_LVL6		0x0004		/* LVL 6 */
#define SIPEND_IRQ7		0x0002		/* IRQ 7 */
#define SIPEND_LVL7		0x0001		/* LVL 7 */
#define SIPEND_IRQ(x)	(0x8000>>(x<<1))	/* IRQ x */
#define SIPEND_LVL(x)	(0x4000>>(x<<1))	/* LVL x */

/* SIEL (SIU Interrupt Edge Level Register) */
#define SIEL_ED0		0x8000
#define SIEL_WM0		0x4000
#define SIEL_ED1		0x2000
#define SIEL_WM1		0x1000
#define SIEL_ED2		0x0800
#define SIEL_WM2		0x0400
#define SIEL_ED3		0x0200
#define SIEL_WM3		0x0100
#define SIEL_ED4		0x0080
#define SIEL_WM4		0x0040
#define SIEL_ED5		0x0020
#define SIEL_WM5		0x0010
#define SIEL_ED6		0x0008
#define SIEL_WM6		0x0004
#define SIEL_ED7		0x0002
#define SIEL_WM7		0x0001
#define SIEL_ED(x)		(0x8000>>(x<<1))
#define SIEL_WM(x)		(0x4000>>(x<<1))

/* SIVEC (SIU Interrupt Vector) */
#define SIVEC_IRQ0		0x00		/* IRQ 0 */
#define SIVEC_LVL0		0x04		/* LVL 0 */
#define SIVEC_IRQ1		0x08		/* IRQ 1 */
#define SIVEC_LVL1		0x0c		/* LVL 1 */
#define SIVEC_IRQ2		0x10		/* IRQ 2 */
#define SIVEC_LVL2		0x14		/* LVL 2 */
#define SIVEC_IRQ3		0x18		/* IRQ 3 */
#define SIVEC_LVL3		0x1c		/* LVL 3 */
#define SIVEC_IRQ4		0x20		/* IRQ 4 */
#define SIVEC_LVL4		0x24		/* LVL 4 */
#define SIVEC_IRQ5		0x28		/* IRQ 5 */
#define SIVEC_LVL5		0x2c		/* LVL 5 */
#define SIVEC_IRQ6		0x30		/* IRQ 6 */
#define SIVEC_LVL6		0x34		/* LVL 6 */
#define SIVEC_IRQ7		0x38		/* IRQ 7 */
#define SIVEC_LVL7		0x3c		/* LVL 7 */
#define SIVEC_IRQ(x)	(x<<3)		/* IRQ x */
#define SIVEC_LVL(x)	((x<<3)+0x4)/* LVL x */



#define LEDOFF(x)		(MPIOSMDR |= (1<<(x)))
#define LEDON(x)		(MPIOSMDR &= ~(1<<(x)))
#define LEDTOGGLE(x)	(MPIOSMDR ^= (1<<(x)))

#endif /* __MPC555_H__*/
