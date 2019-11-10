/*====================================================================
* Project: Board Support Package (BSP)
* Developed using:
* Function: Standard definitions for Intel XScale PXA255
*
* Copyright HighTec EDV-Systeme GmbH 1982-2006
*====================================================================*/

#ifndef __PXA255REGS_H__
#define __PXA255REGS_H__

/* general register definition macro */
#define __REG(x)	((volatile unsigned *)(x))


/* bits and fields for MMU */
#define CP15_1_MMU		(1 << 0)	/* MMU */
#define CP15_1_ALIGN	(1 << 1)	/* alignment fault */
#define CP15_1_DCACHE	(1 << 2)	/* Data Cache */
#define CP15_1_NOP		0x78		/* Read/Write as 0b1111 */
#define CP15_1_SYSP		(1 << 8)	/* System protection */
#define CP15_1_ROMP		(1 << 9)	/* ROM protection */
#define CP15_1_BTB		(1 << 11)	/* Branch Target Buffer */
#define CP15_1_ICACHE	(1 << 12)	/* Instruction Cache */
#define CP15_1_VECTREL	(1 << 13)	/* Exception Vector Relocation */

/* Memory layout */
#define PXA2X0_CS0_BASE				(0x00000000)
#define PXA2X0_CS1_BASE				(0x04000000)
#define PXA2X0_CS2_BASE				(0x08000000)
#define PXA2X0_CS3_BASE				(0x0c000000)
#define PXA2X0_CS4_BASE				(0x10000000)
#define PXA2X0_CS5_BASE				(0x14000000)

#define PXA2X0_PCMCIA0_BASE			(0x20000000)
#define PXA2X0_PCMCIA1_BASE			(0x30000000)

#define PXA2X0_PERIPHERALS_BASE		(0x40000000)
#define PXA2X0_LCD_BASE				(0x44000000)
#define PXA2X0_MEMORY_CTL_BASE		(0x48000000)

#define PXA2X0_RAM_BANK0_BASE		(0xA0000000)
#define PXA2X0_RAM_BANK1_BASE		(0xA4000000)
#define PXA2X0_RAM_BANK2_BASE		(0xA8000000)
#define PXA2X0_RAM_BANK3_BASE		(0xAc000000)

#define PXA2X0_CACHE_FLUSH_BASE		(0xc0000000)

#define DCACHE_FLUSH_AREA			0xc0000000

/* DMA Controller */
#define DMA_CTL_BASE	(PXA2X0_PERIPHERALS_BASE + 0x0000000)
#define DCSR0			__REG(DMA_CTL_BASE+0x0000)  /* DMA Control / Status Register for Channel 0 */
#define DCSR1			__REG(DMA_CTL_BASE+0x0004)  /* DMA Control / Status Register for Channel 1 */
#define DCSR2			__REG(DMA_CTL_BASE+0x0008)  /* DMA Control / Status Register for Channel 2 */
#define DCSR3			__REG(DMA_CTL_BASE+0x000c)  /* DMA Control / Status Register for Channel 3 */
#define DCSR4			__REG(DMA_CTL_BASE+0x0010)  /* DMA Control / Status Register for Channel 4 */
#define DCSR5			__REG(DMA_CTL_BASE+0x0014)  /* DMA Control / Status Register for Channel 5 */
#define DCSR6			__REG(DMA_CTL_BASE+0x0018)  /* DMA Control / Status Register for Channel 6 */
#define DCSR7			__REG(DMA_CTL_BASE+0x001c)  /* DMA Control / Status Register for Channel 7 */
#define DCSR8			__REG(DMA_CTL_BASE+0x0020)  /* DMA Control / Status Register for Channel 8 */
#define DCSR9			__REG(DMA_CTL_BASE+0x0024)  /* DMA Control / Status Register for Channel 9 */
#define DCSR10			__REG(DMA_CTL_BASE+0x0028)  /* DMA Control / Status Register for Channel 10 */
#define DCSR11			__REG(DMA_CTL_BASE+0x002c)  /* DMA Control / Status Register for Channel 11 */
#define DCSR12			__REG(DMA_CTL_BASE+0x0030)  /* DMA Control / Status Register for Channel 12 */
#define DCSR13			__REG(DMA_CTL_BASE+0x0034)  /* DMA Control / Status Register for Channel 13 */
#define DCSR14			__REG(DMA_CTL_BASE+0x0038)  /* DMA Control / Status Register for Channel 14 */
#define DCSR15			__REG(DMA_CTL_BASE+0x003c)  /* DMA Control / Status Register for Channel 15 */

#define DCSR_RUN		(1 << 31)	/* Run Bit (read / write) */
#define DCSR_NODESC		(1 << 30)	/* No-Descriptor Fetch (read / write) */
#define DCSR_STOPIRQEN	(1 << 29)	/* Stop Interrupt Enable (read / write) */
#define DCSR_REQPEND	(1 << 8)	/* Request Pending (read-only) */
#define DCSR_STOPSTATE	(1 << 3)	/* Stop State (read-only) */
#define DCSR_ENDINTR	(1 << 2)	/* End Interrupt (read / write) */
#define DCSR_STARTINTR	(1 << 1)	/* Start Interrupt (read / write) */
#define DCSR_BUSERR		(1 << 0)	/* Bus Error Interrupt (read / write) */


#define DINT			__REG(DMA_CTL_BASE+0x00f0)  /* DMA Interrupt Register */

#define DRCMR0			__REG(DMA_CTL_BASE+0x0100)  /* Request to Channel Map Register for DREQ 0 */
#define DRCMR1			__REG(DMA_CTL_BASE+0x0104)  /* Request to Channel Map Register for DREQ 1 */
#define DRCMR2			__REG(DMA_CTL_BASE+0x0108)  /* Request to Channel Map Register for I2S receive Request */
#define DRCMR3			__REG(DMA_CTL_BASE+0x010c)  /* Request to Channel Map Register for I2S transmit Request */
#define DRCMR4			__REG(DMA_CTL_BASE+0x0110)  /* Request to Channel Map Register for BTUART receive Request */
#define DRCMR5			__REG(DMA_CTL_BASE+0x0114)  /* Request to Channel Map Register for BTUART transmit Request. */
#define DRCMR6			__REG(DMA_CTL_BASE+0x0118)  /* Request to Channel Map Register for FFUART receive Request */
#define DRCMR7			__REG(DMA_CTL_BASE+0x011c)  /* Request to Channel Map Register for FFUART transmit Request */
#define DRCMR8			__REG(DMA_CTL_BASE+0x0120)  /* Request to Channel Map Register for AC97 microphone Request */
#define DRCMR9			__REG(DMA_CTL_BASE+0x0124)  /* Request to Channel Map Register for AC97 modem receive Request */
#define DRCMR10			__REG(DMA_CTL_BASE+0x0128)  /* Request to Channel Map Register for AC97 modem transmit Request */
#define DRCMR11			__REG(DMA_CTL_BASE+0x012c)  /* Request to Channel Map Register for AC97 audio receive Request */
#define DRCMR12			__REG(DMA_CTL_BASE+0x0130)  /* Request to Channel Map Register for AC97 audio transmit Request */
#define DRCMR13			__REG(DMA_CTL_BASE+0x0134)  /* Request to Channel Map Register for SSP receive Request */
#define DRCMR14			__REG(DMA_CTL_BASE+0x0138)  /* Request to Channel Map Register for SSP transmit Request */
#define DRCMR15			__REG(DMA_CTL_BASE+0x013c)  /* Reserved */
#define DRCMR16			__REG(DMA_CTL_BASE+0x0140)  /* Reserved */
#define DRCMR17			__REG(DMA_CTL_BASE+0x0144)  /* Request to Channel Map Register for ICP receive Request */
#define DRCMR18			__REG(DMA_CTL_BASE+0x0148)  /* Request to Channel Map Register for ICP transmit Request */
#define DRCMR19			__REG(DMA_CTL_BASE+0x014c)  /* Request to Channel Map Register for STUART receive Request */
#define DRCMR20			__REG(DMA_CTL_BASE+0x0150)  /* Request to Channel Map Register for STUART transmit Request */
#define DRCMR21			__REG(DMA_CTL_BASE+0x0154)  /* Request to Channel Map Register for MMC receive Request */
#define DRCMR22			__REG(DMA_CTL_BASE+0x0158)  /* Request to Channel Map Register for MMC transmit Request */
#define DRCMR23			__REG(DMA_CTL_BASE+0x015c)  /* Reserved */
#define DRCMR24			__REG(DMA_CTL_BASE+0x0160)  /* Reserved */
#define DRCMR25			__REG(DMA_CTL_BASE+0x0164)  /* Request to Channel Map Register for USB endpoint 1 Request */
#define DRCMR26			__REG(DMA_CTL_BASE+0x0168)  /* Request to Channel Map Register for USB endpoint 2 Request */
#define DRCMR27			__REG(DMA_CTL_BASE+0x016c)  /* Request to Channel Map Register for USB endpoint 3 Request */
#define DRCMR28			__REG(DMA_CTL_BASE+0x0170)  /* Request to Channel Map Register for USB endpoint 4 Request */
#define DRCMR29			__REG(DMA_CTL_BASE+0x0174)  /* Reserved */
#define DRCMR30			__REG(DMA_CTL_BASE+0x0178)  /* Request to Channel Map Register for USB endpoint 6 Request */
#define DRCMR31			__REG(DMA_CTL_BASE+0x017c)  /* Request to Channel Map Register for USB endpoint 7 Request */
#define DRCMR32			__REG(DMA_CTL_BASE+0x0180)  /* Request to Channel Map Register for USB endpoint 8 Request */
#define DRCMR33			__REG(DMA_CTL_BASE+0x0184)  /* Request to Channel Map Register for USB endpoint 9 Request */
#define DRCMR34			__REG(DMA_CTL_BASE+0x0188)  /* Reserved */
#define DRCMR35			__REG(DMA_CTL_BASE+0x018c)  /* Request to Channel Map Register for USB endpoint 11 Request */
#define DRCMR36			__REG(DMA_CTL_BASE+0x0190)  /* Request to Channel Map Register for USB endpoint 12 Request */
#define DRCMR37			__REG(DMA_CTL_BASE+0x0194)  /* Request to Channel Map Register for USB endpoint 13 Request */
#define DRCMR38			__REG(DMA_CTL_BASE+0x0198)  /* Request to Channel Map Register for USB endpoint 14 Request */
#define DRCMR39			__REG(DMA_CTL_BASE+0x019c)  /* Reserved */

#define DRCMRRXSADR		DRCMR2
#define DRCMRTXSADR		DRCMR3
#define DRCMRRXBTRBR	DRCMR4
#define DRCMRTXBTTHR	DRCMR5
#define DRCMRRXFFRBR	DRCMR6
#define DRCMRTXFFTHR	DRCMR7
#define DRCMRRXMCDR		DRCMR8
#define DRCMRRXMODR		DRCMR9
#define DRCMRTXMODR		DRCMR10
#define DRCMRRXPCDR		DRCMR11
#define DRCMRTXPCDR		DRCMR12
#define DRCMRRXSSDR		DRCMR13
#define DRCMRTXSSDR		DRCMR14
#define DRCMRRXICDR		DRCMR17
#define DRCMRTXICDR		DRCMR18
#define DRCMRRXSTRBR	DRCMR19
#define DRCMRTXSTTHR	DRCMR20
#define DRCMRRXMMC		DRCMR21
#define DRCMRTXMMC		DRCMR22

#define DRCMR_MAPVLD	(1 << 7)	/* Map Valid (read / write) */
#define DRCMR_CHLNUM	0x0f		/* mask for Channel Number (read / write) */

#define DDADR0			__REG(DMA_CTL_BASE+0x0200)  /* DMA Descriptor Address Register Channel 0 */
#define DSADR0			__REG(DMA_CTL_BASE+0x0204)  /* DMA Source Address Register Channel 0 */
#define DTADR0			__REG(DMA_CTL_BASE+0x0208)  /* DMA Target Address Register Channel 0 */
#define DCMD0			__REG(DMA_CTL_BASE+0x020c)  /* DMA Command Address Register Channel 0 */
#define DDADR1			__REG(DMA_CTL_BASE+0x0210)  /* DMA Descriptor Address Register Channel 1 */
#define DSADR1			__REG(DMA_CTL_BASE+0x0214)  /* DMA Source Address Register Channel 1 */
#define DTADR1			__REG(DMA_CTL_BASE+0x0218)  /* DMA Target Address Register Channel 1 */
#define DCMD1			__REG(DMA_CTL_BASE+0x021c)  /* DMA Command Address Register Channel 1 */
#define DDADR2			__REG(DMA_CTL_BASE+0x0220)  /* DMA Descriptor Address Register Channel 2 */
#define DSADR2			__REG(DMA_CTL_BASE+0x0224)  /* DMA Source Address Register Channel 2 */
#define DTADR2			__REG(DMA_CTL_BASE+0x0228)  /* DMA Target Address Register Channel 2 */
#define DCMD2			__REG(DMA_CTL_BASE+0x022c)  /* DMA Command Address Register Channel 2 */
#define DDADR3			__REG(DMA_CTL_BASE+0x0230)  /* DMA Descriptor Address Register Channel 3 */
#define DSADR3			__REG(DMA_CTL_BASE+0x0234)  /* DMA Source Address Register Channel 3 */
#define DTADR3			__REG(DMA_CTL_BASE+0x0238)  /* DMA Target Address Register Channel 3 */
#define DCMD3			__REG(DMA_CTL_BASE+0x023c)  /* DMA Command Address Register Channel 3 */
#define DDADR4			__REG(DMA_CTL_BASE+0x0240)  /* DMA Descriptor Address Register Channel 4 */
#define DSADR4			__REG(DMA_CTL_BASE+0x0244)  /* DMA Source Address Register Channel 4 */
#define DTADR4			__REG(DMA_CTL_BASE+0x0248)  /* DMA Target Address Register Channel 4 */
#define DCMD4			__REG(DMA_CTL_BASE+0x024c)  /* DMA Command Address Register Channel 4 */
#define DDADR5			__REG(DMA_CTL_BASE+0x0250)  /* DMA Descriptor Address Register Channel 5 */
#define DSADR5			__REG(DMA_CTL_BASE+0x0254)  /* DMA Source Address Register Channel 5 */
#define DTADR5			__REG(DMA_CTL_BASE+0x0258)  /* DMA Target Address Register Channel 5 */
#define DCMD5			__REG(DMA_CTL_BASE+0x025c)  /* DMA Command Address Register Channel 5 */
#define DDADR6			__REG(DMA_CTL_BASE+0x0260)  /* DMA Descriptor Address Register Channel 6 */
#define DSADR6			__REG(DMA_CTL_BASE+0x0264)  /* DMA Source Address Register Channel 6 */
#define DTADR6			__REG(DMA_CTL_BASE+0x0268)  /* DMA Target Address Register Channel 6 */
#define DCMD6			__REG(DMA_CTL_BASE+0x026c)  /* DMA Command Address Register Channel 6 */
#define DDADR7			__REG(DMA_CTL_BASE+0x0270)  /* DMA Descriptor Address Register Channel 7 */
#define DSADR7			__REG(DMA_CTL_BASE+0x0274)  /* DMA Source Address Register Channel 7 */
#define DTADR7			__REG(DMA_CTL_BASE+0x0278)  /* DMA Target Address Register Channel 7 */
#define DCMD7			__REG(DMA_CTL_BASE+0x027c)  /* DMA Command Address Register Channel 7 */
#define DDADR8			__REG(DMA_CTL_BASE+0x0280)  /* DMA Descriptor Address Register Channel 8 */
#define DSADR8			__REG(DMA_CTL_BASE+0x0284)  /* DMA Source Address Register Channel 8 */
#define DTADR8			__REG(DMA_CTL_BASE+0x0288)  /* DMA Target Address Register Channel 8 */
#define DCMD8			__REG(DMA_CTL_BASE+0x028c)  /* DMA Command Address Register Channel 8 */
#define DDADR9			__REG(DMA_CTL_BASE+0x0290)  /* DMA Descriptor Address Register Channel 9 */
#define DSADR9			__REG(DMA_CTL_BASE+0x0294)  /* DMA Source Address Register Channel 9 */
#define DTADR9			__REG(DMA_CTL_BASE+0x0298)  /* DMA Target Address Register Channel 9 */
#define DCMD9			__REG(DMA_CTL_BASE+0x029c)  /* DMA Command Address Register Channel 9 */
#define DDADR10			__REG(DMA_CTL_BASE+0x02a0)  /* DMA Descriptor Address Register Channel 10 */
#define DSADR10			__REG(DMA_CTL_BASE+0x02a4)  /* DMA Source Address Register Channel 10 */
#define DTADR10			__REG(DMA_CTL_BASE+0x02a8)  /* DMA Target Address Register Channel 10 */
#define DCMD10			__REG(DMA_CTL_BASE+0x02ac)  /* DMA Command Address Register Channel 10 */
#define DDADR11			__REG(DMA_CTL_BASE+0x02b0)  /* DMA Descriptor Address Register Channel 11 */
#define DSADR11			__REG(DMA_CTL_BASE+0x02b4)  /* DMA Source Address Register Channel 11 */
#define DTADR11			__REG(DMA_CTL_BASE+0x02b8)  /* DMA Target Address Register Channel 11 */
#define DCMD11			__REG(DMA_CTL_BASE+0x02bc)  /* DMA Command Address Register Channel 11 */
#define DDADR12			__REG(DMA_CTL_BASE+0x02c0)  /* DMA Descriptor Address Register Channel 12 */
#define DSADR12			__REG(DMA_CTL_BASE+0x02c4)  /* DMA Source Address Register Channel 12 */
#define DTADR12			__REG(DMA_CTL_BASE+0x02c8)  /* DMA Target Address Register Channel 12 */
#define DCMD12			__REG(DMA_CTL_BASE+0x02cc)  /* DMA Command Address Register Channel 12 */
#define DDADR13			__REG(DMA_CTL_BASE+0x02d0)  /* DMA Descriptor Address Register Channel 13 */
#define DSADR13			__REG(DMA_CTL_BASE+0x02d4)  /* DMA Source Address Register Channel 13 */
#define DTADR13			__REG(DMA_CTL_BASE+0x02d8)  /* DMA Target Address Register Channel 13 */
#define DCMD13			__REG(DMA_CTL_BASE+0x02dc)  /* DMA Command Address Register Channel 13 */
#define DDADR14			__REG(DMA_CTL_BASE+0x02e0)  /* DMA Descriptor Address Register Channel 14 */
#define DSADR14			__REG(DMA_CTL_BASE+0x02e4)  /* DMA Source Address Register Channel 14 */
#define DTADR14			__REG(DMA_CTL_BASE+0x02e8)  /* DMA Target Address Register Channel 14 */
#define DCMD14			__REG(DMA_CTL_BASE+0x02ec)  /* DMA Command Address Register Channel 14 */
#define DDADR15			__REG(DMA_CTL_BASE+0x02f0)  /* DMA Descriptor Address Register Channel 15 */
#define DSADR15			__REG(DMA_CTL_BASE+0x02f4)  /* DMA Source Address Register Channel 15 */
#define DTADR15			__REG(DMA_CTL_BASE+0x02f8)  /* DMA Target Address Register Channel 15 */
#define DCMD15			__REG(DMA_CTL_BASE+0x02fc)  /* DMA Command Address Register Channel 15 */

#define DDADR_DESCADDR	0xfffffff0	/* Address of next descriptor (mask) */
#define DDADR_STOP		(1 << 0)	/* Stop (read / write) */

#define DCMD_INCSRCADDR	(1 << 31)	/* Source Address Increment Setting. */
#define DCMD_INCTRGADDR	(1 << 30)	/* Target Address Increment Setting. */
#define DCMD_FLOWSRC	(1 << 29)	/* Flow Control by the source. */
#define DCMD_FLOWTRG	(1 << 28)	/* Flow Control by the target. */
#define DCMD_STARTIRQEN	(1 << 22)	/* Start Interrupt Enable */
#define DCMD_ENDIRQEN	(1 << 21)	/* End Interrupt Enable */
#define DCMD_ENDIAN		(1 << 18)	/* Device Endian-ness. */
#define DCMD_BURST8		(1 << 16)	/* 8 byte burst */
#define DCMD_BURST16	(2 << 16)	/* 16 byte burst */
#define DCMD_BURST32	(3 << 16)	/* 32 byte burst */
#define DCMD_WIDTH1		(1 << 14)	/* 1 byte width */
#define DCMD_WIDTH2		(2 << 14)	/* 2 byte width (HalfWord) */
#define DCMD_WIDTH4		(3 << 14)	/* 4 byte width (Word) */
#define DCMD_LENGTH		0x01fff		/* length mask (max = 8K - 1) */



/* UARTs */
#define FFUART_BASE		(PXA2X0_PERIPHERALS_BASE + 0x0100000)
#define BTUART_BASE		(PXA2X0_PERIPHERALS_BASE + 0x0200000)
#define STUART_BASE		(PXA2X0_PERIPHERALS_BASE + 0x0700000)
#define UART_RBR		0x00	/* Receive Buffer Register (read only) */
#define UART_THR		0x00	/* Transmit Holding Register (write only) */
#define UART_IER		0x04	/* Interrupt Enable Register */
#define UART_IIR		0x08	/* Interrupt ID Register (read only) */
#define UART_FCR		0x08	/* FIFO Control Register (write only) */
#define UART_LCR		0x0C	/* Line Control Register */
#define UART_MCR		0x10	/* Modem Control Register */
#define UART_LSR		0x14	/* Line Status Register (read only) */
#define UART_SPR		0x1C	/* Scratch Pad Register */
#define UART_ISR		0x20	/* Infrared Selection Register */
#define UART_DLL		0x00	/* Divisor Latch Low Register (DLAB = 1) */
#define UART_DLH		0x04	/* Divisor Latch High Register (DLAB = 1) */

/* The interrupt enable register bits. */
#define SIO_IER_RAVIE	0x01	/* enable received data available irq */
#define SIO_IER_TIE		0x02	/* enable transmit data request interrupt */
#define SIO_IER_RLSE	0x04	/* enable receiver line status irq */
#define SIO_IER_MIE		0x08	/* enable modem status interrupt */
#define SIO_IER_RTOIE	0x10	/* enable Rx timeout interrupt */
#define SIO_IER_NRZE	0x20	/* enable NRZ coding */
#define SIO_IER_UUE		0x40	/* enable the UART unit */
#define SIO_IER_DMAE	0x80	/* enable DMA requests */

/* The interrupt identification register bits. */
#define SIO_IIR_IP		0x01	/* 0 if interrupt pending */
#define SIO_IIR_ID_MASK	0xCF	/* mask for interrupt ID bits */
#define SIO_IIR_MDM		0x00	/* modem status change */
#define SIO_IIR_TX		0x02	/* transmit interrupt */
#define SIO_IIR_RX		0x04	/* receive interrupt */
#define SIO_IIR_RXERR	0x06	/* receive error */
#define SIO_IIR_TOD		0x08	/* character timeout detection */

/* The line status register bits. */
#define SIO_LSR_DR		0x01	/* data ready */
#define SIO_LSR_OE		0x02	/* overrun error */
#define SIO_LSR_PE		0x04	/* parity error */
#define SIO_LSR_FE		0x08	/* framing error */
#define SIO_LSR_BI		0x10	/* break interrupt */
#define SIO_LSR_THRE	0x20	/* transmitter holding register empty */
#define SIO_LSR_TEMT	0x40	/* transmitter holding and Tx shift registers empty */
#define SIO_LSR_ERR		0x80	/* any error condition (FIFOE) */

/* The modem status register bits. */
#define SIO_MSR_DCTS	0x01	/* delta clear to send */
#define SIO_MSR_DDSR	0x02	/* delta data set ready */
#define SIO_MSR_TERI	0x04	/* trailing edge ring indicator */
#define SIO_MSR_DDCD	0x08	/* delta data carrier detect */
#define SIO_MSR_CTS		0x10	/* clear to send */
#define SIO_MSR_DSR		0x20	/* data set ready */
#define SIO_MSR_RI		0x40	/* ring indicator */
#define SIO_MSR_DCD		0x80	/* data carrier detect */

/* The line control register bits. */
#define SIO_LCR_WLS0	0x01	/* word length select bit 0 */
#define SIO_LCR_WLS1	0x02	/* word length select bit 1 */
#define SIO_LCR_STB		0x04	/* number of stop bits */
#define SIO_LCR_PEN		0x08	/* parity enable */
#define SIO_LCR_EPS		0x10	/* even parity select */
#define SIO_LCR_SP		0x20	/* stick parity */
#define SIO_LCR_SB		0x40	/* set break */
#define SIO_LCR_DLAB	0x80	/* divisor latch access bit */

/* The modem control register bits. */
#define SIO_MCR_DTR		0x01	/* data terminal ready */
#define SIO_MCR_RTS		0x02	/* request to send */
#define SIO_MCR_OUT1	0x04	/* test bit for loopback mode only */
#define SIO_MCR_OUT2	0x08	/* connect UART interrupt to interrupt controller */
#define SIO_MCR_LOOP	0x10	/* loopback mode */

/* The FIFO control register */
#define SIO_FCR_FCR0		0x01	/* enable xmit and rcvr fifos */
#define SIO_FCR_FCR1		0x02	/* clear RCVR FIFO */
#define SIO_FCR_FCR2		0x04	/* clear XMIT FIFO */
#define SIO_FCR_ITL0		0x40	/* Interrupt trigger level (ITL) bit 0 */
#define SIO_FCR_ITL1		0x80	/* Interrupt trigger level (ITL) bit 1 */
#define SIO_FCR_ITL_1BYTE	0x00	/* 1 byte triggers interrupt */

/* Full Function UART (FFUART) */
#define FFUART	FFUART_BASE
#define FFRBR	__REG(FFUART_BASE+0x00)	/* Receive Buffer Register (read only) */
#define FFTHR	__REG(FFUART_BASE+0x00)	/* Transmit Holding Register (write only) */
#define FFIER	__REG(FFUART_BASE+0x04)	/* Interrupt Enable Register */
#define FFIIR	__REG(FFUART_BASE+0x08)	/* Interrupt ID Register (read only) */
#define FFFCR	__REG(FFUART_BASE+0x08)	/* FIFO Control Register (write only) */
#define FFLCR	__REG(FFUART_BASE+0x0C)	/* Line Control Register */
#define FFMCR	__REG(FFUART_BASE+0x10)	/* Modem Control Register */
#define FFLSR	__REG(FFUART_BASE+0x14)	/* Line Status Register (read only) */
#define FFMSR	__REG(FFUART_BASE+0x18)	/* Modem Status Register (read only) */
#define FFSPR	__REG(FFUART_BASE+0x1C)	/* Scratch Pad Register */
#define FFISR	__REG(FFUART_BASE+0x20)	/* Infrared Selection Register */
#define FFDLL	__REG(FFUART_BASE+0x00)	/* Divisor Latch Low Register (DLAB = 1) */
#define FFDLH	__REG(FFUART_BASE+0x04)	/* Divisor Latch High Register (DLAB = 1) */

/* Bluetooth UART (BTUART) */
#define BTUART	BTUART_BASE
#define BTRBR	__REG(BTUART_BASE+0x00)	/* Receive Buffer Register (read only) */
#define BTTHR	__REG(BTUART_BASE+0x00)	/* Transmit Holding Register (write only) */
#define BTIER	__REG(BTUART_BASE+0x04)	/* Interrupt Enable Register */
#define BTIIR	__REG(BTUART_BASE+0x08)	/* Interrupt ID Register (read only) */
#define BTFCR	__REG(BTUART_BASE+0x08)	/* FIFO Control Register (write only) */
#define BTLCR	__REG(BTUART_BASE+0x0C)	/* Line Control Register */
#define BTMCR	__REG(BTUART_BASE+0x10)	/* Modem Control Register */
#define BTLSR	__REG(BTUART_BASE+0x14)	/* Line Status Register (read only) */
#define BTMSR	__REG(BTUART_BASE+0x18)	/* Modem Status Register (read only) */
#define BTSPR	__REG(BTUART_BASE+0x1C)	/* Scratch Pad Register */
#define BTISR	__REG(BTUART_BASE+0x20)	/* Infrared Selection Register */
#define BTDLL	__REG(BTUART_BASE+0x00)	/* Divisor Latch Low Register (DLAB = 1) */
#define BTDLH	__REG(BTUART_BASE+0x04)	/* Divisor Latch High Register (DLAB = 1) */

/* Standard UART (STUART) */
#define STUART	STUART_BASE
#define STRBR	__REG(STUART_BASE+0x00)	/* Receive Buffer Register (read only) */
#define STTHR	__REG(STUART_BASE+0x00)	/* Transmit Holding Register (write only) */
#define STIER	__REG(STUART_BASE+0x04)	/* Interrupt Enable Register */
#define STIIR	__REG(STUART_BASE+0x08)	/* Interrupt ID Register (read only) */
#define STFCR	__REG(STUART_BASE+0x08)	/* FIFO Control Register (write only) */
#define STLCR	__REG(STUART_BASE+0x0C)	/* Line Control Register */
#define STMCR	__REG(STUART_BASE+0x10)	/* Modem Control Register */
#define STLSR	__REG(STUART_BASE+0x14)	/* Line Status Register (read only) */
#define STMSR	__REG(STUART_BASE+0x18)	/* Reserved */
#define STSPR	__REG(STUART_BASE+0x1C)	/* Scratch Pad Register */
#define STISR	__REG(STUART_BASE+0x20)	/* Infrared Selection Register */
#define STDLL	__REG(STUART_BASE+0x00)	/* Divisor Latch Low Register (DLAB = 1) */
#define STDLH	__REG(STUART_BASE+0x04)	/* Divisor Latch High Register (DLAB = 1) */

/* I2C */
#define I2C_BASE	(PXA2X0_PERIPHERALS_BASE + 0x0300000)
#define IBMR		__REG(I2C_BASE+0x1680)  /* I2C Bus Monitor Register */
#define IDBR		__REG(I2C_BASE+0x1688)  /* I2C Data Buffer Register */
#define ICR			__REG(I2C_BASE+0x1690)  /* I2C Control Register */
#define ISR			__REG(I2C_BASE+0x1698)  /* I2C Status Register */
#define ISAR		__REG(I2C_BASE+0x16a0)  /* I2C Slave Address Register */

#define ICR_START	(1 << 0)	/* start bit */
#define ICR_STOP	(1 << 1)	/* stop bit */
#define ICR_ACKNAK	(1 << 2)	/* send ACK(0) or NAK(1) */
#define ICR_TB		(1 << 3)	/* transfer byte bit */
#define ICR_MA		(1 << 4)	/* master abort */
#define ICR_SCLE	(1 << 5)	/* master clock enable */
#define ICR_IUE		(1 << 6)	/* unit enable */
#define ICR_GCD		(1 << 7)	/* general call disable */
#define ICR_ITEIE	(1 << 8)	/* enable tx interrupts */
#define ICR_IRFIE	(1 << 9)	/* enable rx interrupts */
#define ICR_BEIE	(1 << 10)	/* enable bus error ints */
#define ICR_SSDIE	(1 << 11)	/* slave STOP detected int enable */
#define ICR_ALDIE	(1 << 12)	/* enable arbitration interrupt */
#define ICR_SADIE	(1 << 13)	/* slave address detected int enable */
#define ICR_UR		(1 << 14)	/* unit reset */

#define ISR_RWM		(1 << 0)	/* read/write mode */
#define ISR_ACKNAK	(1 << 1)	/* ack/nak status */
#define ISR_UB		(1 << 2)	/* unit busy */
#define ISR_IBB		(1 << 3)	/* bus busy */
#define ISR_SSD		(1 << 4)	/* slave stop detected */
#define ISR_ALD		(1 << 5)	/* arbitration loss detected */
#define ISR_ITE		(1 << 6)	/* tx buffer empty */
#define ISR_IRF		(1 << 7)	/* rx buffer full */
#define ISR_GCAD	(1 << 8)	/* general call address detected */
#define ISR_SAD		(1 << 9)	/* slave address detected */
#define ISR_BED		(1 << 10)	/* bus error no ACK/NAK */


/* I2S: Serial Audio Controller */
#define I2S_BASE	(PXA2X0_PERIPHERALS_BASE + 0x0400000)
#define SACR0		__REG(I2S_BASE+0x0000)  /* Global Control Register */
#define SACR1		__REG(I2S_BASE+0x0004)  /* Serial Audio I 2 S/MSB-Justified Control Register */
#define SASR0		__REG(I2S_BASE+0x000c)  /* Serial Audio I 2 S/MSB-Justified Interface and FIFO Status Register */
#define SAIMR		__REG(I2S_BASE+0x0014)  /* Serial Audio Interrupt Mask Register */
#define SAICR		__REG(I2S_BASE+0x0018)  /* Serial Audio Interrupt Clear Register */
#define SADIV		__REG(I2S_BASE+0x0060)  /* Audio Clock Divider Register. */
#define SADR		__REG(I2S_BASE+0x0080)  /* Serial Audio Data Register (TX and RX FIFO access Register). */


/* AC97 */
#define AC97_BASE	(PXA2X0_PERIPHERALS_BASE + 0x0500000)
#define POCR					__REG(AC97_BASE+0x0000)
#define PICR					__REG(AC97_BASE+0x0004)
#define MCCR					__REG(AC97_BASE+0x0008)
#define GCR						__REG(AC97_BASE+0x000c)
#define POSR					__REG(AC97_BASE+0x0010)
#define PISR					__REG(AC97_BASE+0x0014)
#define MCSR					__REG(AC97_BASE+0x0018)
#define GSR						__REG(AC97_BASE+0x001c)
#define CAR						__REG(AC97_BASE+0x0020)
#define PCDR					__REG(AC97_BASE+0x0040)
#define MCDR					__REG(AC97_BASE+0x0060)
#define MOCR					__REG(AC97_BASE+0x0100)
#define MICR					__REG(AC97_BASE+0x0108)
#define MOSR					__REG(AC97_BASE+0x0110)
#define MISR					__REG(AC97_BASE+0x0118)
#define MODR					__REG(AC97_BASE+0x0140)
#define AC97_PRIM_AUDIO_BASE	__REG(AC97_BASE+0x0200)
#define AC97_SEC_AUDIO_BASE		__REG(AC97_BASE+0x0300)
#define AC97_PRIM_MODEM_BASE	__REG(AC97_BASE+0x0400)
#define AC97_SEC_MODEM_BASE		__REG(AC97_BASE+0x0500)


/* UDC USB Device Controller */
#define UDC_BASE	(PXA2X0_PERIPHERALS_BASE + 0x0600000)
#define UDCCR		__REG(UDC_BASE+0x0000)	/* UDC Control Register */
#define UDCCR_UDE	(1 << 0)	/* UDC enable */
#define UDCCR_UDA	(1 << 1)	/* UDC active */
#define UDCCR_RSM	(1 << 2)	/* Device resume */
#define UDCCR_RESIR	(1 << 3)	/* Resume interrupt request */
#define UDCCR_SUSIR	(1 << 4)	/* Suspend interrupt request */
#define UDCCR_SRM	(1 << 5)	/* Suspend/resume interrupt mask */
#define UDCCR_RSTIR	(1 << 6)	/* Reset interrupt request */
#define UDCCR_REM	(1 << 7)	/* Reset interrupt mask */

#define UDC_RES1	__REG(UDC_BASE+0x0004)	/* UDC Reserved 1 */
/* undocumented: control double buffering (EP1-EP4,EP6,EP7) */

#define UDCCFR		__REG(UDC_BASE+0x0008)	/* UDC Control Function Register */
#define UDCCFR_ACM	(1 << 2)	/* ACK Control Mode */
#define UDCCFR_AREN	(1 << 7)	/* ACK Response ENable */
/* undocumented: remaining bits control double buffering (EP8,EP9,EP11-EP14) */

#define UDC_RES3	__REG(UDC_BASE+0x000C)	/* UDC Reserved 3 */

#define UDCCS0		__REG(UDC_BASE+0x0010)  /* UDC Endpoint 0 Control/Status Register */
#define UDCCS0_OPR	(1 << 0)	/* OUT packet ready */
#define UDCCS0_IPR	(1 << 1)	/* IN packet ready */
#define UDCCS0_FTF	(1 << 2)	/* Flush Tx FIFO */
#define UDCCS0_DRWF	(1 << 3)	/* Device remote wakeup feature */
#define UDCCS0_SST	(1 << 4)	/* Sent stall */
#define UDCCS0_FST	(1 << 5)	/* Force stall */
#define UDCCS0_RNE	(1 << 6)	/* Receive FIFO no empty */
#define UDCCS0_SA	(1 << 7)	/* Setup active */

/* Bulk IN - Endpoint 1,6,11 */
#define UDCCS1		__REG(UDC_BASE+0x0014)  /* UDC Endpoint 1 (IN) Control/Status Register */
#define UDCCS6		__REG(UDC_BASE+0x0028)  /* UDC Endpoint 6 (IN) Control/Status Register */
#define UDCCS11		__REG(UDC_BASE+0x003c)  /* UDC Endpoint 11 (IN) Control/Status Register */

#define UDCCS_BI_TFS	(1 << 0)	/* Transmit FIFO service */
#define UDCCS_BI_TPC	(1 << 1)	/* Transmit packet complete */
#define UDCCS_BI_FTF	(1 << 2)	/* Flush Tx FIFO */
#define UDCCS_BI_TUR	(1 << 3)	/* Transmit FIFO underrun */
#define UDCCS_BI_SST	(1 << 4)	/* Sent stall */
#define UDCCS_BI_FST	(1 << 5)	/* Force stall */
#define UDCCS_BI_TSP	(1 << 7)	/* Transmit short packet */

/* Bulk OUT - Endpoint 2,7,12 */
#define UDCCS2		__REG(UDC_BASE+0x0018)  /* UDC Endpoint 2 (OUT) Control/Status Register */
#define UDCCS7		__REG(UDC_BASE+0x002c)  /* UDC Endpoint 7 (OUT) Control/Status Register */
#define UDCCS12		__REG(UDC_BASE+0x0040)  /* UDC Endpoint 12 (OUT) Control/Status Register */

#define UDCCS_BO_RFS	(1 << 0)	/* Receive FIFO service */
#define UDCCS_BO_RPC	(1 << 1)	/* Receive packet complete */
#define UDCCS_BO_DME	(1 << 3)	/* DMA enable */
#define UDCCS_BO_SST	(1 << 4)	/* Sent stall */
#define UDCCS_BO_FST	(1 << 5)	/* Force stall */
#define UDCCS_BO_RNE	(1 << 6)	/* Receive FIFO not empty */
#define UDCCS_BO_RSP	(1 << 7)	/* Receive short packet */

/* Isochronous IN - Endpoint 3,8,13 */
#define UDCCS3		__REG(UDC_BASE+0x001c)  /* UDC Endpoint 3 (IN) Control/Status Register */
#define UDCCS8		__REG(UDC_BASE+0x0030)  /* UDC Endpoint 8 (IN) Control/Status Register */
#define UDCCS13		__REG(UDC_BASE+0x0044)  /* UDC Endpoint 13 (IN) Control/Status Register */

#define UDCCS_II_TFS	(1 << 0)	/* Transmit FIFO service */
#define UDCCS_II_TPC	(1 << 1)	/* Transmit packet complete */
#define UDCCS_II_FTF	(1 << 2)	/* Flush Tx FIFO */
#define UDCCS_II_TUR	(1 << 3)	/* Transmit FIFO underrun */
#define UDCCS_II_TSP	(1 << 7)	/* Transmit short packet */

/* Isochronous OUT - Endpoint 4,9,14 */
#define UDCCS4		__REG(UDC_BASE+0x0020)  /* UDC Endpoint 4 (OUT) Control/Status Register */
#define UDCCS9		__REG(UDC_BASE+0x0034)  /* UDC Endpoint 9 (OUT) Control/Status Register */
#define UDCCS14		__REG(UDC_BASE+0x0048)  /* UDC Endpoint 14 (OUT) Control/Status Register */

#define UDCCS_IO_RFS	(1 << 0)	/* Receive FIFO service */
#define UDCCS_IO_RPC	(1 << 1)	/* Receive packet complete */
#define UDCCS_IO_ROF	(1 << 3)	/* Receive overflow */
#define UDCCS_IO_DME	(1 << 3)	/* DMA enable */
#define UDCCS_IO_RNE	(1 << 6)	/* Receive FIFO not empty */
#define UDCCS_IO_RSP	(1 << 7)	/* Receive short packet */

/* Interrupt IN - Endpoint 5,10,15 */
#define UDCCS5		__REG(UDC_BASE+0x0024)  /* UDC Endpoint 5 (Interrupt) Control/Status Register */
#define UDCCS10		__REG(UDC_BASE+0x0038)  /* UDC Endpoint 10 (Interrupt) Control/Status Register */
#define UDCCS15		__REG(UDC_BASE+0x004c)  /* UDC Endpoint 15 (Interrupt) Control/Status Register */

#define UDCCS_INT_TFS	(1 << 0)	/* Transmit FIFO service */
#define UDCCS_INT_TPC	(1 << 1)	/* Transmit packet complete */
#define UDCCS_INT_FTF	(1 << 2)	/* Flush Tx FIFO */
#define UDCCS_INT_TUR	(1 << 3)	/* Transmit FIFO underrun */
#define UDCCS_INT_SST	(1 << 4)	/* Sent stall */
#define UDCCS_INT_FST	(1 << 5)	/* Force stall */
#define UDCCS_INT_TSP	(1 << 7)	/* Transmit short packet */

#define UFNRH		__REG(UDC_BASE+0x0060)  /* UDC Frame Number Register High */
#define UFNRL		__REG(UDC_BASE+0x0064)  /* UDC Frame Number Register Low */
#define UBCR2		__REG(UDC_BASE+0x0068)  /* UDC Byte Count Reg 2 */
#define UBCR4		__REG(UDC_BASE+0x006c)  /* UDC Byte Count Reg 4 */
#define UBCR7		__REG(UDC_BASE+0x0070)  /* UDC Byte Count Reg 7 */
#define UBCR9		__REG(UDC_BASE+0x0074)  /* UDC Byte Count Reg 9 */
#define UBCR12		__REG(UDC_BASE+0x0078)  /* UDC Byte Count Reg 12 */
#define UBCR14		__REG(UDC_BASE+0x007c)  /* UDC Byte Count Reg 14 */
#define UDDR0		__REG(UDC_BASE+0x0080)  /* UDC Endpoint 0 Data Register */
#define UDDR1		__REG(UDC_BASE+0x0100)  /* UDC Endpoint 1 Data Register */
#define UDDR2		__REG(UDC_BASE+0x0180)  /* UDC Endpoint 2 Data Register */
#define UDDR3		__REG(UDC_BASE+0x0200)  /* UDC Endpoint 3 Data Register */
#define UDDR4		__REG(UDC_BASE+0x0400)  /* UDC Endpoint 4 Data Register */
#define UDDR5		__REG(UDC_BASE+0x00a0)  /* UDC Endpoint 5 Data Register */
#define UDDR6		__REG(UDC_BASE+0x0600)  /* UDC Endpoint 6 Data Register */
#define UDDR7		__REG(UDC_BASE+0x0680)  /* UDC Endpoint 7 Data Register */
#define UDDR8		__REG(UDC_BASE+0x0700)  /* UDC Endpoint 8 Data Register */
#define UDDR9		__REG(UDC_BASE+0x0900)  /* UDC Endpoint 9 Data Register */
#define UDDR10		__REG(UDC_BASE+0x00c0)  /* UDC Endpoint 10 Data Register */
#define UDDR11		__REG(UDC_BASE+0x0b00)  /* UDC Endpoint 11 Data Register */
#define UDDR12		__REG(UDC_BASE+0x0b80)  /* UDC Endpoint 12 Data Register */
#define UDDR13		__REG(UDC_BASE+0x0c00)  /* UDC Endpoint 13 Data Register */
#define UDDR14		__REG(UDC_BASE+0x0e00)  /* UDC Endpoint 14 Data Register */
#define UDDR15		__REG(UDC_BASE+0x00e0)  /* UDC Endpoint 15 Data Register */

#define UICR0		__REG(UDC_BASE+0x0050)  /* UDC Interrupt Control Register 0 */

#define UICR0_IM0	(1 << 0)	/* Interrupt mask ep 0 */
#define UICR0_IM1	(1 << 1)	/* Interrupt mask ep 1 */
#define UICR0_IM2	(1 << 2)	/* Interrupt mask ep 2 */
#define UICR0_IM3	(1 << 3)	/* Interrupt mask ep 3 */
#define UICR0_IM4	(1 << 4)	/* Interrupt mask ep 4 */
#define UICR0_IM5	(1 << 5)	/* Interrupt mask ep 5 */
#define UICR0_IM6	(1 << 6)	/* Interrupt mask ep 6 */
#define UICR0_IM7	(1 << 7)	/* Interrupt mask ep 7 */

#define UICR1		__REG(UDC_BASE+0x0054)  /* UDC Interrupt Control Register 1 */

#define UICR1_IM8	(1 << 0)	/* Interrupt mask ep 8 */
#define UICR1_IM9	(1 << 1)	/* Interrupt mask ep 9 */
#define UICR1_IM10	(1 << 2)	/* Interrupt mask ep 10 */
#define UICR1_IM11	(1 << 3)	/* Interrupt mask ep 11 */
#define UICR1_IM12	(1 << 4)	/* Interrupt mask ep 12 */
#define UICR1_IM13	(1 << 5)	/* Interrupt mask ep 13 */
#define UICR1_IM14	(1 << 6)	/* Interrupt mask ep 14 */
#define UICR1_IM15	(1 << 7)	/* Interrupt mask ep 15 */

#define USIR0		__REG(UDC_BASE+0x0058)  /* UDC Status Interrupt Register 0 */

#define USIR0_IR0	(1 << 0)	/* Interrupt request ep 0 */
#define USIR0_IR1	(1 << 1)	/* Interrupt request ep 1 */
#define USIR0_IR2	(1 << 2)	/* Interrupt request ep 2 */
#define USIR0_IR3	(1 << 3)	/* Interrupt request ep 3 */
#define USIR0_IR4	(1 << 4)	/* Interrupt request ep 4 */
#define USIR0_IR5	(1 << 5)	/* Interrupt request ep 5 */
#define USIR0_IR6	(1 << 6)	/* Interrupt request ep 6 */
#define USIR0_IR7	(1 << 7)	/* Interrupt request ep 7 */

#define USIR1		__REG(UDC_BASE+0x005c)  /* UDC Status Interrupt Register 1 */

#define USIR1_IR8	(1 << 0)	/* Interrupt request ep 8 */
#define USIR1_IR9	(1 << 1)	/* Interrupt request ep 9 */
#define USIR1_IR10	(1 << 2)	/* Interrupt request ep 10 */
#define USIR1_IR11	(1 << 3)	/* Interrupt request ep 11 */
#define USIR1_IR12	(1 << 4)	/* Interrupt request ep 12 */
#define USIR1_IR13	(1 << 5)	/* Interrupt request ep 13 */
#define USIR1_IR14	(1 << 6)	/* Interrupt request ep 14 */
#define USIR1_IR15	(1 << 7)	/* Interrupt request ep 15 */


/* ICP: Fast Infrared Communication Port */
#define ICP_BASE	(PXA2X0_PERIPHERALS_BASE + 0x0800000)
#define ICCR0		__REG(ICP_BASE+0x0000)  /* ICP Control Register 0 */
#define ICCR1		__REG(ICP_BASE+0x0004)  /* ICP Control Register 1 */
#define ICCR2		__REG(ICP_BASE+0x0008)  /* ICP Control Register 2 */
#define ICDR		__REG(ICP_BASE+0x000c)  /* ICP Data Register */
#define ICSR0		__REG(ICP_BASE+0x0014)  /* ICP Status Register 0 */
#define ICSR1		__REG(ICP_BASE+0x0018)  /* ICP Status Register 1 */

#define ICCR0_AME	(1 << 7)	/* Adress match enable */
#define ICCR0_TIE	(1 << 6)	/* Transmit FIFO interrupt enable */
#define ICCR0_RIE	(1 << 5)	/* Recieve FIFO interrupt enable */
#define ICCR0_RXE	(1 << 4)	/* Receive enable */
#define ICCR0_TXE	(1 << 3)	/* Transmit enable */
#define ICCR0_TUS	(1 << 2)	/* Transmit FIFO underrun select */
#define ICCR0_LBM	(1 << 1)	/* Loopback mode */
#define ICCR0_ITR	(1 << 0)	/* IrDA transmission */

#define ICSR0_FRE	(1 << 5)	/* Framing error */
#define ICSR0_RFS	(1 << 4)	/* Receive FIFO service request */
#define ICSR0_TFS	(1 << 3)	/* Transnit FIFO service request */
#define ICSR0_RAB	(1 << 2)	/* Receiver abort */
#define ICSR0_TUR	(1 << 1)	/* Trunsmit FIFO underun */
#define ICSR0_EIF	(1 << 0)	/* End/Error in FIFO */

#define ICSR1_ROR	(1 << 6)	/* Receiver FIFO underrun  */
#define ICSR1_CRE	(1 << 5)	/* CRC error */
#define ICSR1_EOF	(1 << 4)	/* End of frame */
#define ICSR1_TNF	(1 << 3)	/* Transmit FIFO not full */
#define ICSR1_RNE	(1 << 2)	/* Receive FIFO not empty */
#define ICSR1_TBY	(1 << 1)	/* Tramsmiter busy flag */
#define ICSR1_RSY	(1 << 0)	/* Recevier synchronized flag */


/* Real Time Clock */
#define RTC_BASE	(PXA2X0_PERIPHERALS_BASE + 0x0900000)
#define RCNR		__REG(RTC_BASE+0x00)	/* RTC Count Register */
#define RTAR		__REG(RTC_BASE+0x04)	/* RTC Alarm Register */
#define RTSR		__REG(RTC_BASE+0x08)	/* RTC Status Register */
#define RTTR		__REG(RTC_BASE+0x0C)	/* RTC Timer Trim Register */

#define RTSR_HZE	(1 << 3)	/* HZ interrupt enable */
#define RTSR_ALE	(1 << 2)	/* RTC alarm interrupt enable */
#define RTSR_HZ		(1 << 1)	/* HZ rising-edge detected */
#define RTSR_AL		(1 << 0)	/* RTC alarm detected */

/* OS Timer & Match Registers */
#define OST_BASE	(PXA2X0_PERIPHERALS_BASE + 0x0A00000)
#define OSMR0		__REG(OST_BASE+0x00)	/* OS Timer Match Register 0 */
#define OSMR1		__REG(OST_BASE+0x04)	/* OS Timer Match Register 1 */
#define OSMR2		__REG(OST_BASE+0x08)	/* OS Timer Match Register 2 */
#define OSMR3		__REG(OST_BASE+0x0C)	/* OS Timer Match Register 3 */
#define OSCR		__REG(OST_BASE+0x10)	/* OS Timer Counter Register */
#define OSSR		__REG(OST_BASE+0x14)	/* OS Timer Status Register */
#define OWER		__REG(OST_BASE+0x18)	/* OS Timer Watchdog Enable Register */
#define OIER		__REG(OST_BASE+0x1C)	/* OS Timer Interrupt Enable Register*/

#define OSSR_M3		(1 << 3)	/* Match status channel 3 */
#define OSSR_M2		(1 << 2)	/* Match status channel 2 */
#define OSSR_M1		(1 << 1)	/* Match status channel 1 */
#define OSSR_M0		(1 << 0)	/* Match status channel 0 */

#define OWER_WME	(1 << 0)	;/* Watchdog Match Enable */

#define OIER_E3		(1 << 3)	;/* Interrupt enable channel 3 */
#define OIER_E2		(1 << 2)	;/* Interrupt enable channel 2 */
#define OIER_E1		(1 << 1)	;/* Interrupt enable channel 1 */
#define OIER_E0		(1 << 0)	;/* Interrupt enable channel 0 */

/* PWM 0 */
#define PWM0_BASE		(PXA2X0_PERIPHERALS_BASE + 0x0b00000)
#define PWM_CTRL0		__REG(PWM0_BASE+0x0000)
#define PWM_PWDUTY0		__REG(PWM0_BASE+0x0004)
#define PWM_PERVAL0		__REG(PWM0_BASE+0x0008)

/* PWM 1 */
#define PWM1_BASE		(PXA2X0_PERIPHERALS_BASE + 0x0c00000)
#define PWM_CTRL1		__REG(PWM1_BASE+0x0000)
#define PWM_PWDUTY1		__REG(PWM1_BASE+0x0004)
#define PWM_PERVAL1		__REG(PWM1_BASE+0x0008)

/* Interrupt Controller */
#define IC_BASE	(PXA2X0_PERIPHERALS_BASE + 0x0d00000)
#define ICIP	__REG(IC_BASE+0x00)	/* IRQ Pending Register */
#define ICMR	__REG(IC_BASE+0x04)	/* Mask Register */
#define ICLR	__REG(IC_BASE+0x08)	/* Level Register */
#define ICFP	__REG(IC_BASE+0x0C)	/* FIQ Pending Register */
#define ICPR	__REG(IC_BASE+0x10)	/* Pending Register */
#define ICCR	__REG(IC_BASE+0x14)	/* Control Register */


/* General Purpose I/O */
#define GPIO_BASE		(PXA2X0_PERIPHERALS_BASE + 0x0e00000)
#define GPSR0_OFFSET	0x18	/* GPIO Pin Output Set Register GPIO<31:0> */
#define GPSR1_OFFSET	0x1C	/* GPIO Pin Output Set Register GPIO<63:32> */
#define GPSR2_OFFSET	0x20	/* GPIO Pin Output Set Register GPIO<84:64> */
#define GPCR0_OFFSET	0x24	/* GPIO Pin Output Clear Register GPIO<31:0> */
#define GPCR1_OFFSET	0x28	/* GPIO Pin Output Clear Register GPIO <63:32> */
#define GPCR2_OFFSET	0x2C	/* GPIO Pin Output Clear Register GPIO <84:64> */
#define GPDR0_OFFSET	0x0C	/* GPIO Pin Direction Register GPIO<31:0> */
#define GPDR1_OFFSET	0x10	/* GPIO Pin Direction Register GPIO<63:32> */
#define GPDR2_OFFSET	0x14	/* GPIO Pin Direction Register GPIO<84:64> */
#define GAFR0_L_OFFSET	0x54	/* GPIO Alternate Function Select Register GPIO<15:0> */
#define GAFR0_U_OFFSET	0x58	/* GPIO Alternate Function Select Register GPIO<31:16> */
#define GAFR1_L_OFFSET	0x5C	/* GPIO Alternate Function Select Register GPIO<47:32> */
#define GAFR1_U_OFFSET	0x60	/* GPIO Alternate Function Select Register GPIO<63:48> */
#define GAFR2_L_OFFSET	0x64	/* GPIO Alternate Function Select Register GPIO<79:64> */
#define GAFR2_U_OFFSET	0x68	/* GPIO Alternate Function Select Register GPIO<84:80> */

#define GPLR0	__REG(GPIO_BASE+0x00)	/* GPIO Pin-Level Register GPIO<31:0> */
#define GPLR1	__REG(GPIO_BASE+0x04)	/* GPIO Pin-Level Register GPIO<63:32> */
#define GPLR2	__REG(GPIO_BASE+0x08)	/* GPIO Pin-Level Register GPIO<84:64> */

#define GPDR0	__REG(GPIO_BASE+0x0C)	/* GPIO Pin Direction Register GPIO<31:0> */
#define GPDR1	__REG(GPIO_BASE+0x10)	/* GPIO Pin Direction Register GPIO<63:32> */
#define GPDR2	__REG(GPIO_BASE+0x14)	/* GPIO Pin Direction Register GPIO<84:64> */

#define GPSR0	__REG(GPIO_BASE+0x18)	/* GPIO Pin Output Set Register GPIO<31:0> */
#define GPSR1	__REG(GPIO_BASE+0x1C)	/* GPIO Pin Output Set Register GPIO<63:32> */
#define GPSR2	__REG(GPIO_BASE+0x20)	/* GPIO Pin Output Set Register GPIO<84:64> */

#define GPCR0	__REG(GPIO_BASE+0x24)	/* GPIO Pin Output Clear Register GPIO<31:0> */
#define GPCR1	__REG(GPIO_BASE+0x28)	/* GPIO Pin Output Clear Register GPIO <63:32> */
#define GPCR2	__REG(GPIO_BASE+0x2C)	/* GPIO Pin Output Clear Register GPIO <84:64> */

#define GRER0	__REG(GPIO_BASE+0x30)	/* GPIO Rising-Edge Detect Register GPIO<31:0> */
#define GRER1	__REG(GPIO_BASE+0x34)	/* GPIO Rising-Edge Detect Register GPIO<63:32> */
#define GRER2	__REG(GPIO_BASE+0x38)	/* GPIO Rising-Edge Detect Register GPIO<84:64> */

#define GFER0	__REG(GPIO_BASE+0x3C)	/* GPIO Falling-Edge Detect Register GPIO<31:0> */
#define GFER1	__REG(GPIO_BASE+0x40)	/* GPIO Falling-Edge Detect Register GPIO<63:32> */
#define GFER2	__REG(GPIO_BASE+0x44)	/* GPIO Falling-Edge Detect Register GPIO<84:64> */

#define GEDR0	__REG(GPIO_BASE+0x48)	/* GPIO Edge Detect Status Register GPIO<31:0> */
#define GEDR1	__REG(GPIO_BASE+0x4C)	/* GPIO Edge Detect Status Register GPIO<63:32> */
#define GEDR2	__REG(GPIO_BASE+0x50)	/* GPIO Edge Detect Status Register GPIO<84:64> */

#define GAFR0_L	__REG(GPIO_BASE+0x54)	/* GPIO Alternate Function Select Reg. GPIO<15:0> */
#define GAFR0_U	__REG(GPIO_BASE+0x58)	/* GPIO Alternate Function Select Reg. GPIO<31:16> */
#define GAFR1_L	__REG(GPIO_BASE+0x5C)	/* GPIO Alternate Function Select Reg. GPIO<47:32> */
#define GAFR1_U	__REG(GPIO_BASE+0x60)	/* GPIO Alternate Function Select Reg. GPIO<63:48> */
#define GAFR2_L	__REG(GPIO_BASE+0x64)	/* GPIO Alternate Function Select Reg. GPIO<79:64> */
#define GAFR2_U	__REG(GPIO_BASE+0x68)	/* GPIO Alternate Function Select Reg. GPIO<84:80> */

/* GPIO pin definitions */
#define GPIO11_3P6MHz	(1 << 11)
#define GPIO13_MBGNT	(1 << 13)
#define GPIO15_nCS1		(1 << 15)
#define GPIO23_SCLK		(1 << 23)
#define GPIO24_SFRM		(1 << 24)

#define GPIO33_nCS5		(1 << 1)
#define GPIO34_FFRXD	(1 << 2)
#define GPIO35_FFCTS	(1 << 3)
#define GPIO36_FFDCD	(1 << 4)
#define GPIO37_FFDSR	(1 << 5)
#define GPIO38_FFRI		(1 << 6)
#define GPIO39_FFTXD	(1 << 7)
#define GPIO40_FFDTR	(1 << 8)
#define GPIO41_FFRTS	(1 << 9)
#define GPIO42_BTRXD	(1 << 10)
#define GPIO43_BTTXD	(1 << 11)
#define GPIO46_STRXD	(1 << 14)
#define GPIO47_STTXD	(1 << 15)
#define GPIO48_nPOE		(1 << 16)
#define GPIO49_nPWE		(1 << 17)
#define GPIO50_nPIOR	(1 << 18)
#define GPIO51_nPIOW	(1 << 19)
#define GPIO52_nPCE1	(1 << 20)
#define GPIO53_nPCE2	(1 << 21)
#define GPIO54_nPSKTSEL	(1 << 22)
#define GPIO55_nPREG	(1 << 23)
#define GPIO56_nPWAIT	(1 << 24)
#define GPIO57_nIOIS16	(1 << 25)

#define GPIO78_nCS2		(1 << 14)
#define GPIO79_nCS3		(1 << 15)
#define GPIO80_nCS4		(1 << 16)

/* GPIO alternative functions */
#define GP1_GP_RST		(0x1 << 2)	/* Active low GP_reset */
#define GP6_MMCCLK		(0x1 << 12)	/* Multimedia Card (MMC) clock */
#define GP7_48MHz		(0x1 << 14)	/* 48 MHz clock output */
#define GP8_MMCCS0		(0x1 << 16)	/* MMC Chip Select 0 */
#define GP9_MMCCS1		(0x1 << 18)	/* MMC Chip Select 1 */
#define GP10_RTCCLK		(0x1 << 20)	/* Real Time Clock (1Hz) */
#define GP11_3P6MHz		(0x1 << 22)	/* 3.6 MHz oscillator out */
#define GP12_32kHz		(0x1 << 24)	/* 32 kHz oscillator out */
#define GP13_MBGNT		(0x2 << 26)	/* Memory Controller grant */
#define GP14_MBREQ		(0x1 << 28)	/* Mem.ctrl. alt. bus master request */
#define GP15_nCS1		(0x2 << 30)

#define GP16_PWM0		(0x2 << 0)	/* PWM0 output */
#define GP17_PWM1		(0x2 << 2)	/* PWM1 output */
#define GP18_RDY		(0x1 << 4)	/* Ext. bus Ready */
#define GP19_DREQ1		(0x1 << 6)	/* External DMA request */
#define GP20_DREQ0		(0x1 << 8)	/* External DMA request */
#define GP23_SCLK		(0x2 << 14)	/* SSP clock */
#define GP24_SRFM		(0x2 << 16)	/* SSP frame */
#define GP25_TXD		(0x2 << 18)	/* SSP transmit */
#define GP26_RXD		(0x1 << 20)	/* SSP receive */
#define GP27_EXTCLK		(0x1 << 22)	/* SSP ext_clk */
#define GP28_BITCLK		(0x1 << 24)	/* AC97 bit_clk(IN) */
#define GP29_SDATA_IN0	(0x1 << 26)	/* AC97 Sdata_in0 */
#define GP29_SDATA_IN	(0x2 << 26)	/* I2S Sdata_in */
#define GP30_SDATA_OUT0	(0x1 << 28)	/* I2S Sdata_out */
#define GP30_SDATA_OUT	(0x2 << 28)	/* AC97 Sdata_out */
#define GP31_SYNC0		(0x1 << 30)	/* I2S sync */
#define GP31_SYNC		(0x2 << 30)	/* AC97 sync */

#define GP32_SDATA_IN1	(0x1 << 0)	/* AC97 Sdata_in1 */
#define GP32_SYSCLK		(0x1 << 0)	/* I2S System Clock out */
#define GP33_nCS5		(0x2 << 2)
#define GP34_FFRXD		(0x1 << 4)	/* FFUART receive */
#define GP34_MMCCS0		(0x2 << 4)	/* MMC Chip Select 0 */
#define GP35_CTS		(0x1 << 6)	/* FFUART Clear To Send */
#define GP36_DCD		(0x1 << 8)	/* FFUART Data Carrier Detect */
#define GP37_DSR		(0x1 << 10)	/* FFUART Data Set Ready */
#define GP38_RI			(0x1 << 12)	/* FFUART Ring Indicator */
#define GP39_MMCCS1		(0x1 << 14)	/* MMC Chip Select 1 */
#define GP39_FFTXD		(0x2 << 14)	/* FFUART transmit */
#define GP40_DTR		(0x2 << 16)	/* FFUART Data Terminal Ready */
#define GP41_RTS		(0x2 << 18)	/* FFUART Request To Send */
#define GP42_BTRXD		(0x1 << 20)	/* BTUART receive */
#define GP42_HWRXD		(0x3 << 20)	/* HWUART receive */
#define GP43_BTTXD		(0x2 << 22)	/* BTUART transmit */
#define GP43_HWTXD		(0x3 << 22)	/* HWUART transmit */
#define GP46_ICP_RXD	(0x1 << 28)	/* ICP receive */
#define GP46_STRXD		(0x2 << 28)	/* STUART receive */
#define GP47_STTXD		(0x1 << 30)	/* STUART transmit */
#define GP47_ICP_TXD	(0x2 << 30)	/* ICP transmit */

#define GP48_nPOE		(0x2 << 0)	/* Output Enable for Card Space */
#define GP49_nPWE		(0x2 << 2)	/* Write Enable for Card Space */
#define GP50_nPIOR		(0x2 << 4)	/* I/O Read for Card Space */
#define GP51_nPIOW		(0x2 << 6)	/* I/O Write for Card Space */
#define GP52_nPCE1		(0x2 << 8)	/* Card Enable for Card Space */
#define GP53_nPCE2		(0x2 << 10)	/* Card Enable for Card Space */
#define GP54_nPSKTSEL	(0x2 << 12)	/* Socket Select for Card Space */
#define GP55_nPREG		(0x2 << 14)	/* Card Address Bit 26 */
#define GP56_nPWAIT		(0x1 << 16)	/* Wait signal for Card Space */
#define GP57_nIOIS16	(0x1 << 18)	/* Bus Width select for I/O Card Space */
#define GP58_LDD0		(0x2 << 20)	/* LCD data */
#define GP59_LDD1		(0x2 << 22)
#define GP60_LDD2		(0x2 << 24)
#define GP61_LDD3		(0x2 << 26)
#define GP62_LDD4		(0x2 << 28)
#define GP63_LDD5		(0x2 << 30)

#define GP64_LDD6		(0x2 << 0)
#define GP65_LDD7		(0x2 << 2)
#define GP66_LDD8		(0x2 << 4)
#define GP67_LDD9		(0x2 << 6)
#define GP68_LDD10		(0x2 << 8)
#define GP69_LDD11		(0x2 << 10)
#define GP70_LDD12		(0x2 << 12)
#define GP71_LDD13		(0x2 << 14)
#define GP72_LDD14		(0x2 << 16)
#define GP73_LDD15		(0x2 << 18)
#define GP74_FCLK		(0x2 << 20)	/* LCD Frame clock */
#define GP75_LCLK		(0x2 << 22)	/* LCD line clock */
#define GP76_PCLK		(0x2 << 24)	/* LCD pixel clock */
#define GP77_ACBIAS		(0x2 << 26)	/* LCD AC bias */
#define GP78_nCS2		(0x2 << 28)
#define GP79_nCS3		(0x2 << 30)

#define GP80_nCS4		(0x2 << 0)
#define GP81_NSSPSCLK	(0x1 << 2)	/* NSSP Serial clock */
#define GP82_NSSPSFRM	(0x1 << 4)	/* NSSP frame */
#define GP83_NSSPTXD	(0x1 << 6)	/* NSSP transmit */
#define GP83_NSSPRXD	(0x2 << 6)	/* NSSP receive */
#define GP84_NSSPTXD	(0x1 << 8)	/* NSSP transmit */
#define GP84_NSSPRXD	(0x2 << 8)	/* NSSP receive */


/* Power Manager and Reset Control */
#define PM_BASE			(PXA2X0_PERIPHERALS_BASE + 0x0f00000)
#define PMCR_OFFSET		0x00		/* PM Control Reg. */
#define PSSR_OFFSET		0x04		/* PM Sleep Status Reg. */
#define PWER_OFFSET		0x0C		/* PM Wake-up Enable Reg. */
#define PCFR_OFFSET		0x1C		/* PM General Configuration Reg. */

#define PCFR_OPDE		(1 << 0)
#define PCFR_FP			(1 << 1)
#define PCFR_FS			(1 << 2)
#define PCFR_DS			(1 << 3)
#define PWER_WE0		(1 << 0)
#define PWER_WE1		(1 << 1)
#define PWER_WERTC		(1 << 31)

#define PMCR	__REG(PM_BASE+0x00)	/* Power Manager Control Register */
#define PSSR	__REG(PM_BASE+0x04)	/* Power Manager Sleep Status Register */
#define PSPR	__REG(PM_BASE+0x08)	/* Power Manager Scratch Pad Register */
#define PWER	__REG(PM_BASE+0x0C)	/* Power Manager Wake-up Enable Register */
#define PRER	__REG(PM_BASE+0x10)	/* Power Manager GPIO Rising-Edge Detect Enable Reg */
#define PFER	__REG(PM_BASE+0x14)	/* Power Manager GPIO Falling-Edge Detect Enable Reg */
#define PEDR	__REG(PM_BASE+0x18)	/* Power Manager GPIO Edge Detect Status Register */
#define PCFR	__REG(PM_BASE+0x1C)	/* Power Manager General Configuration Register */
#define PGSR0	__REG(PM_BASE+0x20)	/* Power Manager GPIO Sleep State Reg for GP[31-0] */
#define PGSR1	__REG(PM_BASE+0x24)	/* Power Manager GPIO Sleep State Reg for GP[63-32] */
#define PGSR2	__REG(PM_BASE+0x28)	/* Power Manager GPIO Sleep State Reg for GP[84-64] */
#define RCSR	__REG(PM_BASE+0x30)	/* Reset Controller Status Register */

#define RCSR_ALL	0x0F
#define RCSR_GPR	(1 << 3)	/* GPIO reset */
#define RCSR_SMR	(1 << 2)	/* Sleep Mode */
#define RCSR_WDR	(1 << 1)	/* Watchdog reset */
#define RCSR_HWR	(1 << 0)	/* Hardware reset */

#define PSSR_MASK	0x37
#define PSSR_RDH	(1 << 5)	/* Read Disable Hold */
#define PSSR_PH		(1 << 4)	/* Peripherial Control Hold */
#define PSSR_VFS	(1 << 2)	/* VDD Fault Status */
#define PSSR_BFS	(1 << 1)	/* Battery Fault Status */
#define PSSR_SSS	(1 << 0)	/* Software Sleep Status */

/* SSP */
#define SSP_BASE	(PXA2X0_PERIPHERALS_BASE + 0x1000000)
#define SSCR0		__REG(SSP_BASE+0x0000)
#define SSCR1		__REG(SSP_BASE+0x0004)
#define SSSR		__REG(SSP_BASE+0x0008)
#define SSITR		__REG(SSP_BASE+0x000c)
#define SSDR		__REG(SSP_BASE+0x0010)

/* MMC MultiMedia Card Controller */
#define MMC_BASE		(PXA2X0_PERIPHERALS_BASE + 0x1100000)
#define MMC_STRPCL		__REG(MMC_BASE+0x0000)
#define MMC_STAT		__REG(MMC_BASE+0x0004)
#define MMC_CLKRT		__REG(MMC_BASE+0x0008)
#define MMC_SPI			__REG(MMC_BASE+0x000c)
#define MMC_CMDAT		__REG(MMC_BASE+0x0010)
#define MMC_RESTO		__REG(MMC_BASE+0x0014)
#define MMC_RDTO		__REG(MMC_BASE+0x0018)
#define MMC_BLKLEN		__REG(MMC_BASE+0x001c)
#define MMC_NOB			__REG(MMC_BASE+0x0020)
#define MMC_PRTBUF		__REG(MMC_BASE+0x0024)
#define MMC_I_MASK		__REG(MMC_BASE+0x0028)
#define MMC_I_REG		__REG(MMC_BASE+0x002c)
#define MMC_CMD			__REG(MMC_BASE+0x0030)
#define MMC_ARGH		__REG(MMC_BASE+0x0034)
#define MMC_ARGL		__REG(MMC_BASE+0x0038)
#define MMC_RES			__REG(MMC_BASE+0x003c)
#define MMC_RXFIFO		__REG(MMC_BASE+0x0040)
#define MMC_TXFIFO		__REG(MMC_BASE+0x0044)

/* Core Clock */
#define CCLK_BASE	(PXA2X0_PERIPHERALS_BASE + 0x1300000)
#define CCCR_OFFSET	0x00	/* Core Clock Configuration Register */
#define CKEN_OFFSET	0x04	/* Clock Enable Register */
#define OSCC_OFFSET	0x08	/* Oscillator Configuration Register */

#define CCCR		__REG(CCLK_BASE+0x00)	/* Core Clock Configuration Register */
#define CKEN		__REG(CCLK_BASE+0x04)	/* Clock Enable Register */
#define OSCC		__REG(CCLK_BASE+0x08)	/* Oscillator Configuration Register */

#define CCCR_N_MASK	0x0380	/* Run Mode Frequency to Turbo Mode Frequency Multiplier */
#define CCCR_M_MASK	0x0060	/* Memory Frequency to Run Mode Frequency Multiplier */
#define CCCR_L_MASK	0x001f	/* Crystal Frequency to Memory Frequency Multiplier */

#define CKEN16_LCD		(1 << 16)	/* LCD Unit Clock Enable */
#define CKEN14_I2C		(1 << 14)	/* I2C Unit Clock Enable */
#define CKEN13_FICP		(1 << 13)	/* FICP Unit Clock Enable */
#define CKEN12_MMC		(1 << 12)	/* MMC Unit Clock Enable */
#define CKEN11_USB		(1 << 11)	/* USB Unit Clock Enable */
#define CKEN8_I2S		(1 << 8)	/* I2S Unit Clock Enable */
#define CKEN7_BTUART	(1 << 7)	/* BTUART Unit Clock Enable */
#define CKEN6_FFUART	(1 << 6)	/* FFUART Unit Clock Enable */
#define CKEN5_STUART	(1 << 5)	/* STUART Unit Clock Enable */
#define CKEN3_SSP		(1 << 3)	/* SSP Unit Clock Enable */
#define CKEN2_AC97		(1 << 2)	/* AC97 Unit Clock Enable */
#define CKEN1_PWM1		(1 << 1)	/* PWM1 Clock Enable */
#define CKEN0_PWM0		(1 << 0)	/* PWM0 Clock Enable */

#define OSCC_OON		(1 << 1)	/* 32.768kHz OON (write-once only bit) */
#define OSCC_OOK		(1 << 0)	/* 32.768kHz OOK (read-only bit) */


/* LCD Controller */
#define LCD_BASE	PXA2X0_LCD_BASE
#define LCCR0		__REG(LCD_BASE+0x0000)  /* LCD Controller Control Register 0 */
#define LCCR1		__REG(LCD_BASE+0x0004)  /* LCD Controller Control Register 1 */
#define LCCR2		__REG(LCD_BASE+0x0008)  /* LCD Controller Control Register 2 */
#define LCCR3		__REG(LCD_BASE+0x000c)  /* LCD Controller Control Register 3 */
#define FBR0		__REG(LCD_BASE+0x0020)  /* DMA Channel 0 Frame Branch Register */
#define FBR1		__REG(LCD_BASE+0x0024)  /* DMA Channel 1 Frame Branch Register */
#define LCSR		__REG(LCD_BASE+0x0038)  /* LCD Controller Status Register */
#define LIIDR		__REG(LCD_BASE+0x003c)  /* LCD Controller Interrupt ID Register */
#define TRGBR		__REG(LCD_BASE+0x0040)  /* TMED RGB Seed Register */
#define TCR			__REG(LCD_BASE+0x0044)  /* TMED Control Register */

#define FDADR0		__REG(LCD_BASE+0x0200)  /* DMA Channel 0 Frame Descriptor Address Register */
#define FSADR0		__REG(LCD_BASE+0x0204)  /* DMA Channel 0 Frame Source Address Register */
#define FIDR0		__REG(LCD_BASE+0x0208)  /* DMA Channel 0 Frame ID Register */
#define LDCMD0		__REG(LCD_BASE+0x020c)  /* DMA Channel 0 Command Register */
#define FDADR1		__REG(LCD_BASE+0x0210)  /* DMA Channel 1 Frame Descriptor Address Register */
#define FSADR1		__REG(LCD_BASE+0x0214)  /* DMA Channel 1 Frame Source Address Register */
#define FIDR1		__REG(LCD_BASE+0x0218)  /* DMA Channel 1 Frame ID Register */
#define LDCMD1		__REG(LCD_BASE+0x021c)  /* DMA Channel 1 Command Register */


/* Memory controller */
#define MEMC_BASE		PXA2X0_MEMORY_CTL_BASE
#define MDCNFG_OFFSET	0x00	/* SDRAM Configuration Register 0 */
#define MDREFR_OFFSET	0x04	/* SDRAM Refresh Control Register */
#define MSC0_OFFSET		0x08	/* Static Memory Control Register 0 */
#define MSC1_OFFSET		0x0C	/* Static Memory Control Register 1 */
#define MSC2_OFFSET		0x10	/* Static Memory Control Register 2 */
#define MECR_OFFSET		0x14	/* Expansion Memory (PCMCIA/Compact Flash) Bus Configuration */
#define SXCNFG_OFFSET	0x1C	/* Synchronous Static Memory Control Register */
#define SXMRS_OFFSET	0x24	/* MRS value to be written to Synchronous Flash or SMROM */
#define MCMEM0_OFFSET	0x28	/* Card interface Common Memory Space Socket 0 Timing */
#define MCMEM1_OFFSET	0x2C	/* Card interface Common Memory Space Socket 1 Timing */
#define MCATT0_OFFSET	0x30	/* Card interface Attribute Space Socket 0 Timing Config. */
#define MCATT1_OFFSET	0x34	/* Card interface Attribute Space Socket 1 Timing Config. */
#define MCIO0_OFFSET	0x38	/* Card interface I/O Space Socket 0 Timing Configuration */
#define MCIO1_OFFSET	0x3C	/* Card interface I/O Space Socket 1 Timing Configuration */
#define MDMRS_OFFSET	0x40	/* MRS value to be written to SDRAM */
#define BOOT_DEF_OFFSET	0x44	/* Read-Only Boot-Time Register. (BOOT_SEL and PKG_SEL) */

#define MDCNFG_DE0		(1 << 0)
#define MDCNFG_DE1		(1 << 1)
#define MDCNFG_DE2		(1 << 16)
#define MDCNFG_DE3		(1 << 17)
#define MDCNFG_DWID0	(1 << 2)

#define MDREFR_K2FREE	(1 << 25)
#define MDREFR_K1FREE	(1 << 24)
#define MDREFR_K0FREE	(1 << 23)
#define MDREFR_SLFRSH	(1 << 22)
#define MDREFR_APD		(1 << 20)
#define MDREFR_E0PIN	(1 << 12)
#define MDREFR_E1PIN	(1 << 15)
#define MDREFR_K0DB2	(1 << 14)
#define MDREFR_K1DB2	(1 << 17)
#define MDREFR_K2DB2	(1 << 19)
#define MDREFR_K0RUN	(1 << 13)
#define MDREFR_K1RUN	(1 << 16)
#define MDREFR_K2RUN	(1 << 18)

#define MDCNFG		__REG(MEMC_BASE+0x00)	/* SDRAM Configuration Register 0 */
#define MDREFR		__REG(MEMC_BASE+0x04)	/* SDRAM Refresh Control Register */
#define MSC0		__REG(MEMC_BASE+0x08)	/* Static Memory Control Register 0 */
#define MSC1		__REG(MEMC_BASE+0x0C)	/* Static Memory Control Register 1 */
#define MSC2		__REG(MEMC_BASE+0x10)	/* Static Memory Control Register 2 */
#define MECR		__REG(MEMC_BASE+0x14)	/* Expansion Memory (PCMCIA/Compact Flash) Bus Config. */
#define SXLCR		__REG(MEMC_BASE+0x18)	/* LCR value to be written to SDRAM-Timing Synchr. Flash */
#define SXCNFG		__REG(MEMC_BASE+0x1C)	/* Synchronous Static Memory Control Register */
#define SXMRS		__REG(MEMC_BASE+0x24)	/* MRS value to be written to Synchronous Flash or SMROM */
#define MCMEM0		__REG(MEMC_BASE+0x28)	/* Card interface Common Memory Space Socket 0 Timing */
#define MCMEM1		__REG(MEMC_BASE+0x2C)	/* Card interface Common Memory Space Socket 1 Timing */
#define MCATT0		__REG(MEMC_BASE+0x30)	/* Card interface Attribute Space Socket 0 Timing Config. */
#define MCATT1		__REG(MEMC_BASE+0x34)	/* Card interface Attribute Space Socket 1 Timing Config. */
#define MCIO0		__REG(MEMC_BASE+0x38)	/* Card interface I/O Space Socket 0 Timing Configuration */
#define MCIO1		__REG(MEMC_BASE+0x3C)	/* Card interface I/O Space Socket 1 Timing Configuration */
#define MDMRS		__REG(MEMC_BASE+0x40)	/* MRS value to be written to SDRAM */
#define BOOT_DEF	__REG(MEMC_BASE+0x44)	/* Read-Only Boot-Time Register. Contains BOOT_SEL and PKG_SEL */

#endif /* __PXA255REGS_H__ */
