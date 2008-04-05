#ifndef __EVALUATOR7T_H__
#define __EVALUATOR7T_H__
/*====================================================================
* Project: Board Support Package (BSP)
* Developed using:
* Function: Standard definitions for ARM eval board EVALUATOR-7T
*
* Copyright HighTec EDV-Systeme GmbH 1982-2006
*====================================================================*/

#define PLATFORM_ID				0x0062A


/* memory size */
/*  EVALUATOR7T has been fitted with 512K bytes of SRAM
 */
#define SZ_512K					0x00080000
#define EVALUATOR_SSRAM_BASE	(0)
#define EVALUATOR_SSRAM_SIZE	(SZ_512K)
#define EVALUATOR_FLASH_BASE	(0x01800000)
#define EVALUATOR_FLASH_SIZE	(SZ_512K)

/*	System Manager Group
 */
#define SYSCFG					0x03FF0000
#define EXTDBWTH				((volatile unsigned *)(SYSCFG + 0x3010))
#define ROMCON1					((volatile unsigned *)(SYSCFG + 0x3018))

/* Interrupt Controller	Group */
#define INTMOD					((volatile unsigned *)(SYSCFG + 0x4000))
#define INTPND					((volatile unsigned *)(SYSCFG + 0x4004))
#define INTMSK					((volatile unsigned *)(SYSCFG + 0x4008))

#define INT_EXT0				(0)		/* External Interrupt 0 */
#define INT_EXT1				(1)		/* External Interrupt 1 */
#define INT_EXT2				(2)		/* External Interrupt 2 */
#define INT_EXT3				(3)		/* External Interrupt 3 */
#define INT_UART0_TX			(4)		/* UART0 Transmit Interrupt */
#define INT_UART0_RX			(5)		/* UART0 Receive & Error Interrupt */
#define INT_UART1_TX			(6)		/* UART1 Transmit Interrupt */
#define INT_UART1_RX			(7)		/* UART1 Receive & Error Interrupt */
#define INT_GDMA0				(8)		/* General DMA channel 0 Interrupt */
#define INT_GDMA1				(9)		/* General DMA channel 1 Interrupt */
#define INT_TIMER0				(10)	/* Timer 0 Interrupt */
#define INT_TIMER1				(11)	/* Timer 1 Interrupt */
#define INT_HDLCA_TX			(12)	/* HDLC channel A Transmit Interrupt */
#define INT_HDLCA_RX			(13)	/* HDLC channel A Receive  Interrupt */
#define INT_HDLCB_TX			(14)	/* HDLC channel B Transmit Interrupt */
#define INT_HDLCB_RX			(15)	/* HDLC channel B Receive  Interrupt */
#define INT_BDMA_TX				(16)	/* Ethernet controller BDMA Tx Interrupt */
#define INT_BDMA_RX				(17)	/* Ethernet controller BDMA Rx Interrupt */
#define INT_MAC_TX				(18)	/* Ethernet controller MAC Tx Interrupt */
#define INT_MAC_RX				(19)	/* Ethernet controller MAC Rx Interrupt */
#define INT_I2C					(20)	/* I2C Interrupt */
#define INT_GLOBAL				(21)	/* Global Interrupt Enable / no IRQ source */

#define ISR_MIN					 0
#define ISR_MAX					21
#define ISR_COUNT				22

#define MAXIRQNUM				21
#define MAXFIQNUM				21
#define MAXSWINUM				15

#define NR_IRQS					(MAXIRQNUM + 1)


/* Timer Registers */
#define TMOD					((volatile unsigned *)(SYSCFG + 0x6000))
#define TDATA0					((volatile unsigned *)(SYSCFG + 0x6004))
#define TDATA1					((volatile unsigned *)(SYSCFG + 0x6008))
#define TCNT0					((volatile unsigned *)(SYSCFG + 0x600C))
#define TCNT1					((volatile unsigned *)(SYSCFG + 0x6010))

/* Timer Mode Register bits */
#define TMR_TE0					(1<<0)
#define TMR_TMD0				(1<<1)
#define TMR_TCLR0				(1<<2)
#define TMR_TE1					(1<<3)
#define TMR_TMD1				(1<<4)
#define TMR_TCLR1				(1<<5)


/*  50 MHz clock => 50,000 ticks / mSec
 *  Max	period is a bit larger than 85 s
 */
#define mSEC_1					(0xC350)
#define MAX_PERIOD				(0xFFFFFFFF)

/* LEDs */
/*  LEDs are accessed through the I/O Port
 *  I/O Ports
 */
#define IOPMOD					((volatile unsigned *)(SYSCFG + 0x5000))
#define IOPCON					((volatile unsigned *)(SYSCFG + 0x5004))
#define IOPDATA					((volatile unsigned *)(SYSCFG + 0x5008))

/* UART */
#define UART0_BASE				(SYSCFG + 0xD000)
#define UART1_BASE				(SYSCFG + 0xE000)

#define ULCON					0x00
#define UCON					0x04
#define USTAT					0x08
#define UTXBUF					0x0C
#define URXBUF					0x10
#define UBRDIV					0x14

/*
 * Line Control Register bits
 */
#define ULCR5BITS				(0)
#define ULCR6BITS				(1)
#define ULCR7BITS				(2)
#define ULCR8BITS				(3)
#define ULCRS1STOPBIT			(0)
#define ULCRS2STOPBIT			(4)
#define ULCRNOPARITY			(0)
#define ULCRODDPARITY			(4 << 3)
#define ULCREVENPARITY			(5 << 3)
#define ULCRMARKPARITY			(6 << 3)
#define ULCRSPACEPARITY			(7 << 3)

/*
 * UART Control Register bits
 */
#define UCRRXM					(1)			/* enable received data available irq */
#define UCRRXSI					(1 << 2)	/* enable receiver line status irq */
#define UCRTXM					(1 << 3)	/* enable THR empty interrupt */
#define UCRLPB					(1 << 7)	/* enable loopback mode */


/*
 * UART Status Register bits
 */
#define USROVERRUN				(1 << 0)	/* overrun error */
#define USRPARITY				(1 << 1)	/* parity error */
#define USRFRAMING				(1 << 2)	/* framing error */
#define USRBREAK				(1 << 3)	/* break interrupt */
#define USRDTR					(1 << 4)
#define USRRXDATA				(1 << 5)	/* data ready */
#define USRTXHOLDEMPTY			(1 << 6)	/* transmitter holding register empty */
#define USRTXEMPTY				(1 << 7)	/* transmitter register empty */

/* baud rate values at 50 MHz */
#define BAUD_2400				(650 << 4)
#define BAUD_4800				(324 << 4)
#define BAUD_9600				(162 << 4)
#define BAUD_14400				(121 << 4)
#define BAUD_19200				( 80 << 4)
#define BAUD_38400				( 40 << 4)
#define BAUD_57600				( 26 << 4)
#define BAUD_115200				( 13 << 4)
#define BAUD_230400				(  6 << 4)


/* UART primitives */
#define GET_STATUS(p)			(*(volatile unsigned *)((p) + USTAT))
#define RX_DATA(s)				((s) & USRRXDATA)
#define GET_CHAR(p)				(*(volatile unsigned *)((p) + URXBUF))
#define TX_READY(s)				((s) & USRTXHOLDEMPTY)
#define PUT_CHAR(p,c)			(*(unsigned *)((p) + UTXBUF) = (unsigned)(c))


#endif /* __EVALUATOR7T_H__ */
