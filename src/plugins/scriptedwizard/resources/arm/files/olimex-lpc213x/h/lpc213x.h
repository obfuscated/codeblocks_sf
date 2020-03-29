/*====================================================================
* Project: Board Support Package (BSP)
* Developed using:
* Function: Standard definitions for PHILIPS ARM7TDMI-S controller LPC213x
*
* Copyright HighTec EDV-Systeme GmbH 1982-2007
*====================================================================*/

#ifndef __LPC213X_H__
#define __LPC213X_H__

/* general register definition macro */
#define __REG32(x)	((volatile unsigned int *)(x))
#define __REG16(x)	((volatile unsigned short *)(x))
#define __REG8(x)	((volatile unsigned char *)(x))


/*-----------------------------*/
/* Interrupt Source Identifier */
/*-----------------------------*/

#define WDT_ID		 0		/* Watchdog Interrupt */

#define SW_ID		 1		/* Reserved for software interrupts only */

#define COMMRX_ID	 2		/* RX Debug Communication Channel Interrupt */
#define COMMTX_ID	 3		/* TX Debug Communication Channel Interrupt */

#define TIMER0_ID	 4		/* Timer 0 Interrupts */
#define TIMER1_ID	 5		/* Timer 1 Interrupts */

#define UART0_ID	 6		/* UART0 Interrupts */
#define UART1_ID	 7		/* UART1 Interrupts */

#define PWM0_ID		 8		/* PWM Interrupts */
#define I2C0_ID		 9		/* I2C0 Interrupt */

#define SPI0_ID		10		/* SPI0 Interrupts */
#define SPI1_ID		11		/* SPI1 Interrupts */

#define PLL_ID		12		/* PLL Interrupt (PLOCK) */

#define RTC_ID		13		/* Real Time Clock Interrupts */

#define EINT0_ID	14		/* External Interrupt 0 */
#define EINT1_ID	15		/* External Interrupt 1 */
#define EINT2_ID	16		/* External Interrupt 2 */
#define EINT3_ID	17		/* External Interrupt 3 */

#define ADC0_ID		18		/* Analog to Digital Converter 0 Interrupt */
#define I2C1_ID		19		/* I2C1 Interrupt */
#define BOD_ID		20		/* Brown Out Detect Interrupt */
#define ADC1_ID		21		/* Analog to Digital Converter 1 Interrupt */

#define ISR_VALID_MASK	0x003FFFFF


#define ISR_MIN		 0
#define ISR_MAX		21
#define ISR_COUNT	21

#define MAXIRQNUM	(ISR_MAX)
#define MAXFIQNUM	(MAXIRQNUM)
#define MAXSWINUM	15

#define NR_IRQS		(MAXIRQNUM + 1)


/*------------------------------*/
/*    System Control Block      */
/*------------------------------*/
#define SCB_BASE	0xE01FC000

#define MAMCR		__REG32(SCB_BASE + 0x000)	/* MAM Control Register */
#define MAMTIM		__REG32(SCB_BASE + 0x004)	/* MAM Timing Control */

#define MEMMAP		__REG32(SCB_BASE + 0x040)	/* Memory Mapping Control */

#define PLLCON		__REG32(SCB_BASE + 0x080)	/* PLL Control Register */
#define PLLCFG		__REG32(SCB_BASE + 0x084)	/* PLL Configuration Register */
#define PLLSTAT		__REG32(SCB_BASE + 0x088)	/* PLL Status Register (RO) */
#define PLLFEED		__REG32(SCB_BASE + 0x08C)	/* PLL Feed Register (WO) */

#define PCON		__REG32(SCB_BASE + 0x0C0)	/* Power Control Register */
#define PCONP		__REG32(SCB_BASE + 0x0C4)	/* Power Control Peripherals */

#define APBDIV		__REG32(SCB_BASE + 0x100)	/* APB Divider Control */

#define EXTINT		__REG32(SCB_BASE + 0x140)	/* Ext. Interrupt Flag */
#define INTWAKE		__REG32(SCB_BASE + 0x144)	/* Ext. Interrupt Wakeup */
#define EXTMODE		__REG32(SCB_BASE + 0x148)	/* Ext. Interrupt Mode */
#define EXTPOLAR	__REG32(SCB_BASE + 0x14C)	/* Ext. Interrupt Polarity */

#define RSID		__REG32(SCB_BASE + 0x180)	/* Reset Source Identification Register */
#define CSPR		__REG32(SCB_BASE + 0x184)	/* Code Security Protection Register */
#define SCS			__REG32(SCB_BASE + 0x1A0)	/* System Controls and Status */


/* Memory Mapping Control Register Bits Definition */
#define MEMMAP_BOOT		0x00	/* vectors mapped to Boot Block */
#define MEMMAP_FLASH	0x01	/* vectors at 0 (internal Flash) */
#define MEMMAP_ISRAM	0x02	/* vectors mapped to internal SRAM */

#define ISRAM_START		0x40000000	/* start of internal SRAM */

/* PLL Control Register Bits Definition */
#define PLLCON_PLLE		(1 << 0)	/* PLL Enable */
#define PLLCON_PLLC		(1 << 1)	/* PLL Connect */

/* PLL Configuration Register Bits Definition */
#define PLLCFG_MSEL		(0x1F << 0)	/* PLL Multiplier value "M" */
#define PLLCFG_PSEL		(0x03 << 5)	/* PLL Divider value "P" */

/* PLL Status Register Bits Definition */
#define PLLSTAT_MSEL	(0x1F << 0)	/* PLL Multiplier value "M" */
#define PLLSTAT_PSEL	(0x03 << 5)	/* PLL Divider value "P" */
#define PLLSTAT_PLLE	(1 <<  8)	/* Status of PLL Enable */
#define PLLSTAT_PLLC	(1 <<  9)	/* Status of PLL Connect */
#define PLLSTAT_PLOCK	(1 << 10)	/* PLL Lock Status */

#define PLLFEED_PW0		0xAA
#define PLLFEED_PW1		0x55

/* Power Control Register Bits Definition */
#define PCON_IDL		(1 << 0)		/* Idle mode control */
#define PCON_PD			(1 << 1)		/* Power-Down mode control */
#define PCON_BODPDM		(1 << 2)		/* Brown-Out Power-Down mode control */
#define PCON_BOGD		(1 << 3)		/* Brown-Out Global Disable */
#define PCON_BORD		(1 << 4)		/* Brown-Out Reset Disable */

/* Power Control Peripherals Bits Definition */
#define PCTIM0			(1U <<  1)		/* Timer 0 */
#define PCTIM1			(1U <<  2)		/* Timer 1 */
#define PCURT0			(1U <<  3)		/* UART 0 */
#define PCURT1			(1U <<  4)		/* UART 1 */
#define PCPWM0			(1U <<  5)		/* PWM unit */
#define PCI2C0			(1U <<  7)		/* I2C interface 0 */
#define PCSPI0			(1U <<  8)		/* SPI 0 */
#define PCRTC			(1U <<  9)		/* RTC */
#define PCSPI1			(1U << 10)		/* SPI 1 */
#define PCAD0			(1U << 12)		/* Analog to Digital Converter 0 */
#define PCI2C1			(1U << 19)		/* I2C interface 1 */
#define PCAD1			(1U << 20)		/* Analog to Digital Converter 1 */

/* Reset Source Identification Register Bits Definition */
#define RSID_POR		(1 << 0)		/* Power On Reset */
#define RSID_EXTR		(1 << 1)		/* External /RESET */
#define RSID_WDTR		(1 << 2)		/* Watchdog Timer Reset */
#define RSID_BODR		(1 << 3)		/* Brown-Out Detect Reset */

/* APB Divider values */
#define APBDIV_MSK		0x03
#define APBDIV_4		0	/* peripherals clock == CPU/4 clock */
#define APBDIV_2		2	/* peripherals clock == CPU/2 clock */
#define APBDIV_1		1	/* peripherals clock == CPU clock */


/*-------------------------------------------------------*/
/* Vector Interrupt Controller Module Bits Definitions   */
/*-------------------------------------------------------*/

/*------------------------------------------*/
/* Vector Interrupt Controller Register     */
/*------------------------------------------*/

#define VIC_BASE	0xFFFFF000

#define VICIRQSTAT	__REG32(VIC_BASE + 0x000)	/* IRQ Status Register */
#define VICFIQSTAT	__REG32(VIC_BASE + 0x004)	/* FIQ Status Register */
#define VICRIR		__REG32(VIC_BASE + 0x008)	/* Raw Interrupt Status */
#define VICISR		__REG32(VIC_BASE + 0x00C)	/* Interrupt Select Reg */
#define VICIEN		__REG32(VIC_BASE + 0x010)	/* Interrupt Enable Reg */
#define VICIEC		__REG32(VIC_BASE + 0x014)	/* Interrupt Enable Clear */
#define VICSIR		__REG32(VIC_BASE + 0x018)	/* Software Interrupt Reg */
#define VICSIC		__REG32(VIC_BASE + 0x01C)	/* SW Interrupt Clear Reg */
#define VICPRO		__REG32(VIC_BASE + 0x020)	/* Protection Enable Reg */
#define VICVADDR	__REG32(VIC_BASE + 0x030)	/* Vector Address Reg */
#define VICDEFVADDR	__REG32(VIC_BASE + 0x034)	/* Default Vector Address */

/* Vector Address Register 0 - 15 */
#define VICVADDR0	__REG32(VIC_BASE + 0x100)
#define VICVADDR1	__REG32(VIC_BASE + 0x104)
#define VICVADDR2	__REG32(VIC_BASE + 0x108)
#define VICVADDR3	__REG32(VIC_BASE + 0x10C)
#define VICVADDR4	__REG32(VIC_BASE + 0x110)
#define VICVADDR5	__REG32(VIC_BASE + 0x114)
#define VICVADDR6	__REG32(VIC_BASE + 0x118)
#define VICVADDR7	__REG32(VIC_BASE + 0x11C)
#define VICVADDR8	__REG32(VIC_BASE + 0x120)
#define VICVADDR9	__REG32(VIC_BASE + 0x124)
#define VICVADDR10	__REG32(VIC_BASE + 0x128)
#define VICVADDR11	__REG32(VIC_BASE + 0x12C)
#define VICVADDR12	__REG32(VIC_BASE + 0x130)
#define VICVADDR13	__REG32(VIC_BASE + 0x134)
#define VICVADDR14	__REG32(VIC_BASE + 0x138)
#define VICVADDR15	__REG32(VIC_BASE + 0x13C)

/* Vector Control Register 0 - 15: 0 = highest, 15 lowest prio */
#define VICVCR0		__REG32(VIC_BASE + 0x200)
#define VICVCR1		__REG32(VIC_BASE + 0x204)
#define VICVCR2		__REG32(VIC_BASE + 0x208)
#define VICVCR3		__REG32(VIC_BASE + 0x20C)
#define VICVCR4		__REG32(VIC_BASE + 0x210)
#define VICVCR5		__REG32(VIC_BASE + 0x214)
#define VICVCR6		__REG32(VIC_BASE + 0x218)
#define VICVCR7		__REG32(VIC_BASE + 0x21C)
#define VICVCR8		__REG32(VIC_BASE + 0x220)
#define VICVCR9		__REG32(VIC_BASE + 0x224)
#define VICVCR10	__REG32(VIC_BASE + 0x228)
#define VICVCR11	__REG32(VIC_BASE + 0x22C)
#define VICVCR12	__REG32(VIC_BASE + 0x230)
#define VICVCR13	__REG32(VIC_BASE + 0x234)
#define VICVCR14	__REG32(VIC_BASE + 0x238)
#define VICVCR15	__REG32(VIC_BASE + 0x23C)

/*---------------------------------------------*/
/* Pin Connect Block Bits Definitions          */
/*---------------------------------------------*/

/*----------------------------*/
/* Pin Connect Block Register */
/*----------------------------*/

#define PCB_BASE	0xE002C000
#define PINSEL0		__REG32(PCB_BASE + 0x00)	/* Pin function select register 0 */
#define PINSEL1		__REG32(PCB_BASE + 0x04)	/* Pin function select register 1 */
#define PINSEL2		__REG32(PCB_BASE + 0x14)	/* Pin function select register 2 */

/*----------- PINSEL0 Bit Definitions ---------*/

/*-- GPIO 0.0 --*/
#define GPIO00			(0 << 0)
#define GPIO00TXD0		(1 << 0)
#define GPIO00PWM1		(2 << 0)

/*-- GPIO 0.1 --*/
#define GPIO01			(0 << 2)
#define GPIO01RXD0		(1 << 2)
#define GPIO01PWM3		(2 << 2)
#define GPIO01EINT0		(3 << 2)

/*-- GPIO 0.2 --*/
#define GPIO02			(0 << 4)
#define GPIO02SCL0		(1 << 4)	/* I2C0 */
#define GPIO02CR00		(2 << 4)	/* Capture 0.0 (Timer 0) */

/*-- GPIO 0.3 --*/
#define GPIO03			(0 << 6)
#define GPIO03SDA0		(1 << 6)	/* I2C0 */
#define GPIO03MR00		(2 << 6)	/* Match 0.0 (Timer 0) */
#define GPIO03EINT1		(3 << 6)

/*-- GPIO 0.4 --*/
#define GPIO04			(0 << 8)
#define GPIO04SCK0		(1 << 8)	/* SPI0 */
#define GPIO04CR01		(2 << 8)	/* Capture 0.1 (Timer 0) */
#define GPIO04AD06		(3 << 8)	/* AD0.6 */

/*-- GPIO 0.5 --*/
#define GPIO05			(0 << 10)
#define GPIO05MISO0		(1 << 10)	/* SPI0 */
#define GPIO05MR01		(2 << 10)	/* Match 0.1 (Timer 0) */
#define GPIO05AD07		(3 << 10)	/* AD0.7 */

/*-- GPIO 0.6 --*/
#define GPIO06			(0 << 12)
#define GPIO06MOSI0		(1 << 12)	/* SPI0 */
#define GPIO06CR02		(2 << 12)	/* Capture 0.2 (Timer 0) */
#define GPIO06AD10		(3 << 12)	/* AD1.0 */

/*-- GPIO 0.7 --*/
#define GPIO07			(0 << 14)
#define GPIO07SSEL0		(1 << 14)	/* SPI0 */
#define GPIO07PWM2		(2 << 14)
#define GPIO07EINT2		(3 << 14)

/*-- GPIO 0.8 --*/
#define GPIO08			(0 << 16)
#define GPIO08TXD1		(1 << 16)
#define GPIO08PWM4		(2 << 16)
#define GPIO08AD11		(3 << 16)	/* AD1.1 */

/*-- GPIO 0.9 --*/
#define GPIO09			(0 << 18)
#define GPIO09RXD1		(1 << 18)
#define GPIO09PWM6		(2 << 18)
#define GPIO09EINT3		(3 << 18)

/*-- GPIO 0.10 --*/
#define GPIO010			(0 << 20)
#define GPIO010RTS1		(1 << 20)
#define GPIO010CR10		(2 << 20)	/* Capture 1.0 (Timer 1) */
#define GPIO010AD12		(3 << 20)	/* AD1.2 */

/*-- GPIO 0.11 --*/
#define GPIO011			(0 << 22)
#define GPIO011CTS1		(1 << 22)
#define GPIO011CR11		(2 << 22)	/* Capture 1.1 (Timer 1) */
#define GPIO011SCL1		(3 << 22)	/* I2C1 */

/*-- GPIO 0.12 --*/
#define GPIO012			(0 << 24)
#define GPIO012DSR1		(1 << 24)
#define GPIO012MR10		(2 << 24)	/* Match 1.0 (Timer 1) */
#define GPIO012AD13		(3 << 24)	/* AD1.3 */

/*-- GPIO 0.13 --*/
#define GPIO013			(0 << 26)
#define GPIO013DTR1		(1 << 26)
#define GPIO013MR11		(2 << 26)	/* Match 1.1 (Timer 1) */
#define GPIO013AD14		(3 << 26)	/* AD1.4 */

/*-- GPIO 0.14 --*/
#define GPIO014			(0 << 28)
#define GPIO014DCD1		(1 << 28)
#define GPIO014EINT1	(2 << 28)
#define GPIO014SDA1		(3 << 28)	/* I2C1 */

/*-- GPIO 0.15 --*/
#define GPIO015			(0u << 30)
#define GPIO015RI1		(1u << 30)
#define GPIO015EINT2	(2u << 30)
#define GPIO015AD15		(3u << 30)	/* AD1.5 */

/*----------- PINSEL1 Bit Definitions ---------*/

/*-- GPIO 0.16 --*/
#define GPIO016			(0 << 0)
#define GPIO016EINT0	(1 << 0)
#define GPIO016MR02		(2 << 0)	/* Match 0.2 (Timer 0) */
#define GPIO016CR02		(3 << 0)	/* Capture 0.2 (Timer 0) */

/*-- GPIO 0.17 --*/
#define GPIO017			(0 << 2)
#define GPIO017CR12		(1 << 2)	/* Capture 1.2 (Timer 1) */
#define GPIO017SCK1		(2 << 2)	/* SPI1 (SSP) */
#define GPIO017MR12		(3 << 2)	/* Match 1.2 (Timer 1) */

/*-- GPIO 0.18 --*/
#define GPIO018			(0 << 4)
#define GPIO018CR13		(1 << 4)	/* Capture 1.3 (Timer 1) */
#define GPIO018MISO1	(2 << 4)	/* SPI1 (SSP) */
#define GPIO018MR13		(3 << 4)	/* Match 1.3 (Timer 1) */

/*-- GPIO 0.19 --*/
#define GPIO019			(0 << 6)
#define GPIO019MR12		(1 << 6)	/* Match 1.2 (Timer 1) */
#define GPIO019MOSI1	(2 << 6)	/* SPI1 (SSP) */
#define GPIO019CR12		(3 << 6)	/* Capture 1.2 (Timer 1) */

/*-- GPIO 0.20 --*/
#define GPIO020			(0 << 8)
#define GPIO020MR13		(1 << 8)	/* Match 1.3 (Timer 1) */
#define GPIO020SSEL1	(2 << 8)	/* SPI1 (SSP) */
#define GPIO020EINT3	(3 << 8)

/*-- GPIO 0.21 --*/
#define GPIO021			(0 << 10)
#define GPIO021PWM5		(1 << 10)
#define GPIO021AD16		(2 << 10)	/* AD1.6 */
#define GPIO021CR13		(3 << 10)	/* Capture 1.3 (Timer 1) */

/*-- GPIO 0.22--*/
#define GPIO022			(0 << 12)
#define GPIO022AD17		(1 << 12)	/* AD1.7 */
#define GPIO022CR00		(2 << 12)	/* Capture 0.0 (Timer 0) */
#define GPIO022MR00		(3 << 12)	/* Match 0.0 (Timer 0) */

/*-- GPIO 0.23 --*/
#define GPIO023			(0 << 14)

/*-- GPIO 0.24 --*/
#define GPIO024			(0 << 16)

/*-- GPIO 0.25 --*/
#define GPIO025			(0 << 18)
#define GPIO025AD04		(1 << 18)	/* AD0.4 */
#define GPIO025DAC		(2 << 18)	/* Aout DAC */

/*-- GPIO 0.26 --*/
#define GPIO026			(0 << 20)
#define GPIO026AD04		(1 << 20)	/* AD0.5 */

/*-- GPIO 0.27 --*/
#define GPIO027			(0 << 22)
#define GPIO027AD00		(1 << 22)	/* AD0.0 */
#define GPIO027CR01		(2 << 22)	/* Capture 0.1 (Timer 0) */
#define GPIO027MR01		(3 << 22)	/* Match 0.1 (Timer 0) */

/*-- GPIO 0.28 --*/
#define GPIO028			(0 << 24)
#define GPIO028AD01		(1 << 24)	/* AD0.1 */
#define GPIO028CR02		(2 << 24)	/* Capture 0.2 (Timer 0) */
#define GPIO028MR02		(3 << 24)	/* Match 0.2 (Timer 0) */

/*-- GPIO 0.29 --*/
#define GPIO029			(0 << 26)
#define GPIO029AD02		(1 << 26)	/* AD0.2 */
#define GPIO029CR03		(2 << 26)	/* Capture 0.3 (Timer 0) */
#define GPIO029MR03		(3 << 26)	/* Match 0.3 (Timer 0) */

/*-- GPIO 0.30 --*/
#define GPIO030			(0 << 28)
#define GPIO030AD03		(1 << 28)	/* AD0.3 */
#define GPIO030EINT3	(2 << 28)
#define GPIO030CR00		(3 << 28)	/* Capture 0.0 (Timer 0) */

/*-- GPIO 0.31 --*/
#define GPIO031			(0 << 30)

/*----------- PINSEL2 Bit Definitions ---------*/

#define GPIO_DEBUG		(1 << 2)	/* P1.31-26 used for Debug */
#define GPIO_TRACE		(1 << 3)	/* P1.25-16 used for Trace */


/*---------------------------*/
/* GPIO Register Definitions */
/*---------------------------*/

#define GPIO_BASE	0xE0028000
#define IO0PIN		__REG32(GPIO_BASE + 0x00)	/* Pin Value Register */
#define IO0SET		__REG32(GPIO_BASE + 0x04)	/* Output Set Register */
#define IO0DIR		__REG32(GPIO_BASE + 0x08)	/* Direction Control Register */
#define IO0CLR		__REG32(GPIO_BASE + 0x0C)	/* Output Clear Register */

#define IO1PIN		__REG32(GPIO_BASE + 0x10)	/* Pin Value Register */
#define IO1SET		__REG32(GPIO_BASE + 0x14)	/* Output Set Register */
#define IO1DIR		__REG32(GPIO_BASE + 0x18)	/* Direction Control Register */
#define IO1CLR		__REG32(GPIO_BASE + 0x1C)	/* Output Clear Register */


/*-----------------------*/
/* UART Bits Definitions */
/*-----------------------*/

/* bits in UxIER */
#define UIER_RBR	(1 << 0)	/* Receive Buffer Ready */
#define UIER_THRE	(1 << 1)	/* Transmit Hold Register Empty */
#define UIER_RLS	(1 << 2)	/* Rx Line Status */

/* bits in UxIIR */
#define UIIR_NOPEND	0x01	/* No Pending Interrupts */
#define UIIR_THRE	0x02	/* Tx Hold Register Empty */
#define UIIR_RDA	0x04	/* Receive Data Available */
#define UIIR_RLS	0x06	/* Receive Line Status */
#define UIIR_CTI	0x0C	/* Character Timeout Indicator */
#define UIIR_IMSK	0x0F	/* Mask for Interrupt cause */

/* bits in UxLCR */
#define ULCR_CHRL		(0x00 << 0)	/* Byte Length */
#define ULCR_CHRL_5		(0x00 << 0)	/* 5 bits */
#define ULCR_CHRL_6		(0x01 << 0)	/* 6 bits */
#define ULCR_CHRL_7		(0x02 << 0)	/* 7 bits */
#define ULCR_CHRL_8		(0x03 << 0)	/* 8 bits */

#define ULCR_NBSTOP		(1 << 2)	/* Stop Bit Number */
#define ULCR_NBSTOP_1	(0 << 2)	/* 1 Stop Bit */
#define ULCR_NBSTOP_1_5	(1 << 2)	/* 1.5 Stop Bits at 5 bits */
#define ULCR_NBSTOP_2	(1 << 2)	/* 2 Stop Bits */

#define ULCR_PAREN		(1 << 3)	/* Parity Enable */
#define ULCR_PODD		(0x00 << 4)	/* Odd parity */
#define ULCR_PEVEN		(0x01 << 4)	/* Even parity */
#define ULCR_FORC1		(0x02 << 4)	/* Forced "1" stick parity */
#define ULCR_FORC0		(0x03 << 4)	/* Forced "0" stick parity */

#define ULCR_BREAK		(1 << 6)	/* Enable break transmission */

#define ULCR_DLAB		(1 << 7)	/* Divisor Latch Access Bit */

#define DATA8BIT		ULCR_CHRL_8

/* bits in UxLSR */
#define ULSR_RDR		(1 << 0)	/* Receiver Data Ready */
#define ULSR_OE			(1 << 1)	/* Overrun Error */
#define ULSR_PE			(1 << 2)	/* Parity Error */
#define ULSR_FE			(1 << 3)	/* Framing Error */
#define ULSR_BI			(1 << 4)	/* Break Interrupt */
#define ULSR_THRE		(1 << 5)	/* Transmitter Holding Register Empty */
#define ULSR_TEMT		(1 << 6)	/* Transmitter Empty */
#define ULSR_RXFE		(1 << 7)	/* Error in Rx FIFO */

#define ULSR_MASK_ERROR	(ULSR_OE | ULSR_PE | ULSR_FE)

/* bits in UxFCR */
#define FIFOEN		(1 << 0)	/* FIFO Enable */
#define RXFIFOR		(1 << 1)	/* Rx FIFO Reset */
#define TXFIFOR		(1 << 2)	/* Tx FIFO Reset */

#define TRLVL0		(0x00 << 6)
#define TRLVL1		(0x01 << 6)
#define TRLVL2		(0x02 << 6)
#define TRLVL3		(0x03 << 6)

/*---------------------------*/
/* UART Register Definitions */
/*---------------------------*/

/* UART 0 */
#define UART0_BASE	0xE000C000

#define U0RBR		__REG32(UART0_BASE + 0x00)	/* Receiver Buffer Register */
#define U0THR		__REG32(UART0_BASE + 0x00)	/* Transmit Holding Register */
#define U0DLL		__REG32(UART0_BASE + 0x00)	/* Divisor Latch LSB */
#define U0IER		__REG32(UART0_BASE + 0x04)	/* Interrupt Enable Register */
#define U0DLM		__REG32(UART0_BASE + 0x04)	/* Divisor Latch MSB */
#define U0IIR		__REG32(UART0_BASE + 0x08)	/* Interrupt ID Register */
#define U0FCR		__REG32(UART0_BASE + 0x08)	/* FIFO Control Register */
#define U0LCR		__REG32(UART0_BASE + 0x0C)	/* Line Control Register */
#define U0LSR		__REG32(UART0_BASE + 0x14)	/* Line Status Register */
#define U0SCR		__REG32(UART0_BASE + 0x1C)	/* Scratch Pad Register */

/* UART 1 */
#define UART1_BASE	0xE0010000

#define U1RBR		__REG32(UART1_BASE + 0x00)	/* Receiver Buffer Register */
#define U1THR		__REG32(UART1_BASE + 0x00)	/* Transmit Holding Register */
#define U1DLL		__REG32(UART1_BASE + 0x00)	/* Divisor Latch LSB */
#define U1IER		__REG32(UART1_BASE + 0x04)	/* Interrupt Enable Register */
#define U1DLM		__REG32(UART1_BASE + 0x04)	/* Divisor Latch MSB */
#define U1IIR		__REG32(UART1_BASE + 0x08)	/* Interrupt ID Register */
#define U1FCR		__REG32(UART1_BASE + 0x08)	/* FIFO Control Register */
#define U1LCR		__REG32(UART1_BASE + 0x0C)	/* Line Control Register */
#define U1MCR		__REG32(UART1_BASE + 0x10)	/* Modem Control Register */
#define U1LSR		__REG32(UART1_BASE + 0x14)	/* Line Status Register */
#define U1MSR		__REG32(UART1_BASE + 0x18)	/* Modem Status Register */
#define U1SCR		__REG32(UART1_BASE + 0x1C)	/* Scratch Pad Register */



/*--------------------------*/
/* I2C Register Definitions */
/*--------------------------*/
#define I2C0_BASE	0xE001C000

#define I2C0CONSET		__REG32(I2C0_BASE + 0x00)	/* Control Set Register */
#define I2C0STAT		__REG32(I2C0_BASE + 0x04)	/* Status Register */
#define I2C0DAT			__REG32(I2C0_BASE + 0x08)	/* Data Register */
#define I2C0ADR			__REG32(I2C0_BASE + 0x0C)	/* Slave Address Register */
#define I2C0SCLH		__REG32(I2C0_BASE + 0x10)	/* SCL Duty Cycle High Word */
#define I2C0SCLL		__REG32(I2C0_BASE + 0x14)	/* SCL Duty Cycle Low Word */
#define I2C0CONCLR		__REG32(I2C0_BASE + 0x18)	/* Control Clear Register */

#define I2C1_BASE	0xE005C000

#define I2C1CONSET		__REG32(I2C1_BASE + 0x00)	/* Control Set Register */
#define I2C1STAT		__REG32(I2C1_BASE + 0x04)	/* Status Register */
#define I2C1DAT			__REG32(I2C1_BASE + 0x08)	/* Data Register */
#define I2C1ADR			__REG32(I2C1_BASE + 0x0C)	/* Slave Address Register */
#define I2C1SCLH		__REG32(I2C1_BASE + 0x10)	/* SCL Duty Cycle High Word */
#define I2C1SCLL		__REG32(I2C1_BASE + 0x14)	/* SCL Duty Cycle Low Word */
#define I2C1CONCLR		__REG32(I2C1_BASE + 0x18)	/* Control Clear Register */

/*---------------------------------------*/
/* Serial Peripheral Interface Registers */
/*---------------------------------------*/
#define SPI0_BASE	0xE0020000	/* SPI 0 */

#define S0SPCR		__REG32(SPI0_BASE + 0x00)	/* Control Register */
#define S0SPSR		__REG32(SPI0_BASE + 0x04)	/* Status Register */
#define S0SPDR		__REG32(SPI0_BASE + 0x08)	/* Data Register */
#define S0SPCCR		__REG32(SPI0_BASE + 0x0C)	/* Clock Count Register */
#define S0SPINT		__REG32(SPI0_BASE + 0x1C)	/* Interrupt Flag */

#define SSP_BASE	0xE0068000	/* SSP (SPI 1) */

#define SSPCR0		__REG32(SSP_BASE + 0x00)	/* Control Register 0 */
#define SSPCR1		__REG32(SSP_BASE + 0x04)	/* Control Register 1 */
#define SSPDR		__REG32(SSP_BASE + 0x08)	/* Data Register */
#define SSPSR		__REG32(SSP_BASE + 0x0C)	/* Status Register */
#define SSPCPSR		__REG32(SSP_BASE + 0x10)	/* Clock Prescale Register */
#define SSPIMSC		__REG32(SSP_BASE + 0x14)	/* Interrupt Mask Set and Clear Register */
#define SSPRIS		__REG32(SSP_BASE + 0x18)	/* Raw Interrupt Status Register */
#define SSPMIS		__REG32(SSP_BASE + 0x1C)	/* Masked Interrupt Status Register (RO) */
#define SSPICR		__REG32(SSP_BASE + 0x20)	/* Interrupt Clear Register (WO) */


/*----------------------------*/
/* Timer Register Definitions */
/*----------------------------*/

/* Timer 0 */
#define TIMER0_BASE	0xE0004000
#define T0_BASE		TIMER0_BASE

#define T0IR		__REG32(TIMER0_BASE + 0x00)	/* Interrupt Register */
#define T0TCR		__REG32(TIMER0_BASE + 0x04)	/* Control Register */
#define T0TC		__REG32(TIMER0_BASE + 0x08)	/* Counter value */
#define T0PR		__REG32(TIMER0_BASE + 0x0C)	/* Prescale Register */
#define T0PC		__REG32(TIMER0_BASE + 0x10)	/* Prescale Counter */
#define T0MCR		__REG32(TIMER0_BASE + 0x14)	/* Match Control Register */
#define T0MR0		__REG32(TIMER0_BASE + 0x18)	/* Match Register 0 */
#define T0MR1		__REG32(TIMER0_BASE + 0x1C)	/* Match Register 1 */
#define T0MR2		__REG32(TIMER0_BASE + 0x20)	/* Match Register 2 */
#define T0MR3		__REG32(TIMER0_BASE + 0x24)	/* Match Register 3 */
#define T0CCR		__REG32(TIMER0_BASE + 0x28)	/* Capture Control Register */
#define T0CR0		__REG32(TIMER0_BASE + 0x2C)	/* Capture Register 0 */
#define T0CR1		__REG32(TIMER0_BASE + 0x30)	/* Capture Register 1 */
#define T0CR2		__REG32(TIMER0_BASE + 0x34)	/* Capture Register 2 */
#define T0CR3		__REG32(TIMER0_BASE + 0x38)	/* Capture Register 3 */
#define T0EMR		__REG32(TIMER0_BASE + 0x3C)	/* External Match Register */

#define T0CTCR		__REG32(TIMER0_BASE + 0x70)	/* Count Control Register */

/* Timer 1 */
#define TIMER1_BASE	0xE0008000
#define T1_BASE		TIMER1_BASE

#define T1IR		__REG32(TIMER1_BASE + 0x00)	/* Interrupt Register */
#define T1TCR		__REG32(TIMER1_BASE + 0x04)	/* Control Register */
#define T1TC		__REG32(TIMER1_BASE + 0x08)	/* Counter value */
#define T1PR		__REG32(TIMER1_BASE + 0x0C)	/* Prescale Register */
#define T1PC		__REG32(TIMER1_BASE + 0x10)	/* Prescale Counter */
#define T1MCR		__REG32(TIMER1_BASE + 0x14)	/* Match Control Register */
#define T1MR0		__REG32(TIMER1_BASE + 0x18)	/* Match Register 0 */
#define T1MR1		__REG32(TIMER1_BASE + 0x1C)	/* Match Register 1 */
#define T1MR2		__REG32(TIMER1_BASE + 0x20)	/* Match Register 2 */
#define T1MR3		__REG32(TIMER1_BASE + 0x24)	/* Match Register 3 */
#define T1CCR		__REG32(TIMER1_BASE + 0x28)	/* Capture Control Register */
#define T1CR0		__REG32(TIMER1_BASE + 0x2C)	/* Capture Register 0 */
#define T1CR1		__REG32(TIMER1_BASE + 0x30)	/* Capture Register 1 */
#define T1CR2		__REG32(TIMER1_BASE + 0x34)	/* Capture Register 2 */
#define T1CR3		__REG32(TIMER1_BASE + 0x38)	/* Capture Register 3 */
#define T1EMR		__REG32(TIMER1_BASE + 0x3C)	/* External Match Register */

#define T1CTCR		__REG32(TIMER1_BASE + 0x70)	/* Count Control Register */

/* Timer Interrupt Register bits */
#define TIR_MR0		(1 << 0)	/* Match Channel 0 */
#define TIR_MR1		(1 << 1)	/* Match Channel 1 */
#define TIR_MR2		(1 << 2)	/* Match Channel 2 */
#define TIR_MR3		(1 << 3)	/* Match Channel 3 */
#define TIR_CR0		(1 << 4)	/* Capture Channel 0 */
#define TIR_CR1		(1 << 5)	/* Capture Channel 1 */
#define TIR_CR2		(1 << 6)	/* Capture Channel 2 */
#define TIR_CR3		(1 << 7)	/* Capture Channel 3 */

/* Timer Control Register bits */
#define TCR_CTEN	(1 << 0)	/* Counter Enable */
#define TCR_CTRST	(1 << 1)	/* Counter Reset */

/* Timer Match Control Register bits */
#define TMCR_MR0I	(1 << 0)	/* Interrupt on MR0 */
#define TMCR_MR0R	(1 << 1)	/* Reset on MR0 */
#define TMCR_MR0S	(1 << 2)	/* Stop on MR0 */
#define TMCR_MR1I	(1 << 3)	/* Interrupt on MR1 */
#define TMCR_MR1R	(1 << 4)	/* Reset on MR1 */
#define TMCR_MR1S	(1 << 5)	/* Stop on MR1 */
#define TMCR_MR2I	(1 << 6)	/* Interrupt on MR2 */
#define TMCR_MR2R	(1 << 7)	/* Reset on MR2 */
#define TMCR_MR2S	(1 << 8)	/* Stop on MR2 */
#define TMCR_MR3I	(1 << 9)	/* Interrupt on MR3 */
#define TMCR_MR3R	(1 << 10)	/* Reset on MR3 */
#define TMCR_MR3S	(1 << 11)	/* Stop on MR3 */

/* Timer Capture Control Register bits */
#define TCCR_CAP0RE	(1 << 0)	/* CR0 Rising Edge */
#define TCCR_CAP0FE	(1 << 1)	/* CR0 Falling Edge */
#define TCCR_CAP0I	(1 << 2)	/* Interrupt on CR0 */
#define TCCR_CAP1RE	(1 << 3)	/* CR1 Rising Edge */
#define TCCR_CAP1FE	(1 << 4)	/* CR1 Falling Edge */
#define TCCR_CAP1I	(1 << 5)	/* Interrupt on CR1 */
#define TCCR_CAP2RE	(1 << 6)	/* CR2 Rising Edge */
#define TCCR_CAP2FE	(1 << 7)	/* CR2 Falling Edge */
#define TCCR_CAP2I	(1 << 8)	/* Interrupt on CR2 */
#define TCCR_CAP3RE	(1 << 9)	/* CR3 Rising Edge */
#define TCCR_CAP3FE	(1 << 10)	/* CR3 Falling Edge */
#define TCCR_CAP3I	(1 << 11)	/* Interrupt on CR3 */


/*--------------------------*/
/* PWM Register Definitions */
/*--------------------------*/

#define PWM_BASE	0xE0014000

#define PWMIR		__REG32(PWM_BASE + 0x00)	/* Interrupt Register */
#define PWMTCR		__REG32(PWM_BASE + 0x04)	/* Timer Control Register */
#define PWMTC		__REG32(PWM_BASE + 0x08)	/* Timer Counter */
#define PWMPR		__REG32(PWM_BASE + 0x0C)	/* Prescale Register */
#define PWMPC		__REG32(PWM_BASE + 0x10)	/* Prescale Counter */
#define PWMMCR		__REG32(PWM_BASE + 0x14)	/* Match Control Register */
#define PWMMR0		__REG32(PWM_BASE + 0x18)	/* Match Register 0 */
#define PWMMR1		__REG32(PWM_BASE + 0x1C)	/* Match Register 1 */
#define PWMMR2		__REG32(PWM_BASE + 0x20)	/* Match Register 2 */
#define PWMMR3		__REG32(PWM_BASE + 0x24)	/* Match Register 3 */
#define PWMMR4		__REG32(PWM_BASE + 0x40)	/* Match Register 4 */
#define PWMMR5		__REG32(PWM_BASE + 0x44)	/* Match Register 5 */
#define PWMMR6		__REG32(PWM_BASE + 0x48)	/* Match Register 6 */
#define PWMPCR		__REG32(PWM_BASE + 0x4C)	/* Control Register */
#define PWMLER		__REG32(PWM_BASE + 0x50)	/* Latch Enable Register */


/*--------------------------*/
/* ADC Register Definitions */
/*--------------------------*/
#define ADC0_BASE	0xE0034000	/* Analog to Digital Converter 0 */

#define AD0CR		__REG32(ADC0_BASE + 0x00)	/* Control Register */
#define AD0GDR		__REG32(ADC0_BASE + 0x04)	/* Global Data Register */
#define AD0GSR		__REG32(ADC0_BASE + 0x08)	/* Global Start Register */
#define AD0INTEN	__REG32(ADC0_BASE + 0x0C)	/* Interrupt Enable Register */
#define AD0DR0		__REG32(ADC0_BASE + 0x10)	/* Channel 0 Data Register */
#define AD0DR1		__REG32(ADC0_BASE + 0x14)	/* Channel 1 Data Register */
#define AD0DR2		__REG32(ADC0_BASE + 0x18)	/* Channel 2 Data Register */
#define AD0DR3		__REG32(ADC0_BASE + 0x1C)	/* Channel 3 Data Register */
#define AD0DR4		__REG32(ADC0_BASE + 0x20)	/* Channel 4 Data Register */
#define AD0DR5		__REG32(ADC0_BASE + 0x24)	/* Channel 5 Data Register */
#define AD0DR6		__REG32(ADC0_BASE + 0x28)	/* Channel 6 Data Register */
#define AD0DR7		__REG32(ADC0_BASE + 0x2C)	/* Channel 7 Data Register */
#define AD0STAT		__REG32(ADC0_BASE + 0x30)	/* Status Register */

#define ADC1_BASE	0xE0060000	/* Analog to Digital Converter 1 */

#define AD1CR		__REG32(ADC1_BASE + 0x00)	/* Control Register */
#define AD1GDR		__REG32(ADC1_BASE + 0x04)	/* Global Data Register */
#define AD1INTEN	__REG32(ADC1_BASE + 0x0C)	/* Interrupt Enable Register */
#define AD1DR0		__REG32(ADC1_BASE + 0x10)	/* Channel 0 Data Register */
#define AD1DR1		__REG32(ADC1_BASE + 0x14)	/* Channel 1 Data Register */
#define AD1DR2		__REG32(ADC1_BASE + 0x18)	/* Channel 2 Data Register */
#define AD1DR3		__REG32(ADC1_BASE + 0x1C)	/* Channel 3 Data Register */
#define AD1DR4		__REG32(ADC1_BASE + 0x20)	/* Channel 4 Data Register */
#define AD1DR5		__REG32(ADC1_BASE + 0x24)	/* Channel 5 Data Register */
#define AD1DR6		__REG32(ADC1_BASE + 0x28)	/* Channel 6 Data Register */
#define AD1DR7		__REG32(ADC1_BASE + 0x2C)	/* Channel 7 Data Register */
#define AD1STAT		__REG32(ADC1_BASE + 0x30)	/* Status Register */


/*--------------------------*/
/* DAC Register Definitions */
/*--------------------------*/
#define DAC_BASE	0xE006C000	/* Digital to Analog Converter */

#define DAC_DACR	__REG32(DAC_BASE + 0x00)	/* DAC Register */


/*----------------------------*/
/* Real Time Clock Registers  */
/*----------------------------*/
#define RTC_BASE	0xE0024000

#define RTC_ILR		__REG32(RTC_BASE + 0x00)	/* Interrupt Location Reg */
#define RTC_CTC		__REG32(RTC_BASE + 0x04)	/* Clock Tick Counter */
#define RTC_CCR		__REG32(RTC_BASE + 0x08)	/* Clock Control Register */
#define RTC_CIIR	__REG32(RTC_BASE + 0x0C)	/* Counter Incr. Interrpt Reg */
#define RTC_AMR		__REG32(RTC_BASE + 0x10)	/* Alarm Mask Register */
#define RTC_CTIME0	__REG32(RTC_BASE + 0x14)	/* Time Register 0 */
#define RTC_CTIME1	__REG32(RTC_BASE + 0x18)	/* Time Register 1 */
#define RTC_CTIME2	__REG32(RTC_BASE + 0x1C)	/* Time Register 2 */
#define RTC_SEC		__REG32(RTC_BASE + 0x20)	/* Seconds Register */
#define RTC_MIN		__REG32(RTC_BASE + 0x24)	/* Minutes Register */
#define RTC_HOUR	__REG32(RTC_BASE + 0x28)	/* Hours Register */
#define RTC_DOM		__REG32(RTC_BASE + 0x2C)	/* Day Of Month Register */
#define RTC_DOW		__REG32(RTC_BASE + 0x30)	/* Day Of Week Register */
#define RTC_DOY		__REG32(RTC_BASE + 0x34)	/* Day Of Year Register */
#define RTC_MONTH	__REG32(RTC_BASE + 0x38)	/* Months Register */
#define RTC_YEAR	__REG32(RTC_BASE + 0x3C)	/* Year Register */
#define RTC_ALSEC	__REG32(RTC_BASE + 0x60)	/* Alarm Seconds Register */
#define RTC_ALMIN	__REG32(RTC_BASE + 0x64)	/* Alarm Minutes Register */
#define RTC_ALHOUR	__REG32(RTC_BASE + 0x68)	/* Alarm Hours Register */
#define RTC_ALDOM	__REG32(RTC_BASE + 0x6C)	/* Alarm Day Of Month Reg */
#define RTC_ALDOW	__REG32(RTC_BASE + 0x70)	/* Alarm Day Of Week Register */
#define RTC_ALDOY	__REG32(RTC_BASE + 0x74)	/* Alarm Day Of Year Register */
#define RTC_ALMONTH	__REG32(RTC_BASE + 0x78)	/* Alarm Months Register */
#define RTC_ALYEAR	__REG32(RTC_BASE + 0x7C)	/* Alarm Year Register */
#define RTC_PREINT	__REG32(RTC_BASE + 0x80)	/* Prescale Value, Integer */
#define RTC_PREFRAC	__REG32(RTC_BASE + 0x84)	/* Prescale Value, Fraction */


/*--------------------------*/
/* WDT Register Definitions */
/*--------------------------*/

#define WDT_BASE	0xE0000000

#define WDMOD		__REG32(WD_BASE + 0x00)	/* Mode Register */
#define WDTC		__REG32(WD_BASE + 0x04)	/* Timer Constant Register */
#define WDFEED		__REG32(WD_BASE + 0x08)	/* Feed Sequence Register */
#define WDTV		__REG32(WD_BASE + 0x0C)	/* Timer Value Register */

#endif /* __LPC213X_H__ */
