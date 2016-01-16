/*====================================================================
* Project: Board Support Package (BSP)
* Developed using:
* Function: Standard definitions for Sharp LH7A404 (ARM922T)
*
* Copyright HighTec EDV-Systeme GmbH 1982-2006
*====================================================================*/

#ifndef __LH7A404_H__
#define __LH7A404_H__

/* general register definition macro */
#define __REG32(x)	((volatile unsigned int *)(x))
#define __REG16(x)	((volatile unsigned short *)(x))
#define __REG8(x)	((volatile unsigned char *)(x))


/*****************/
/* Memory layout */
/*****************/

#define LH7A404_CS0_BASE		(0x00000000)	/* SMC CS0 */
#define LH7A404_CS1_BASE		(0x10000000)	/* SMC CS1 */
#define LH7A404_CS2_BASE		(0x20000000)	/* SMC CS2 */
#define LH7A404_CS3_BASE		(0x30000000)	/* SMC CS3 */
#define LH7A404_PCMCIA0_BASE	(0x40000000)	/* PC Card/CF Slot 0 */
#define LH7A404_PCMCIA1_BASE	(0x50000000)	/* PC Card/CF Slot 1 */
#define LH7A404_CS6_BASE		(0x60000000)	/* SMC CS6 */
#define LH7A404_CS7_BASE		(0x70000000)	/* SMC CS7 */
#define LH7A404_APB_BASE		(0x80000000)	/* Advanced Peripheral Bus (APB) */
#define LH7A404_AHB_BASE		(0x80002000)	/* Advanced High-Performance Bus (AHB) */

#define LH7A404_ESRAM_BASE		(0xB0000000)	/* Embedded SRAM */

#define LH7A404_SDCSC0_BASE		(0xC0000000)	/* SDRAM CS0 */
#define LH7A404_SDCSC1_BASE		(0xD0000000)	/* SDRAM CS1 */
#define LH7A404_SDCSC2_BASE		(0xE0000000)	/* SDRAM CS2 */
#define LH7A404_SDCSC3_BASE		(0xF0000000)	/* SDRAM CS3 */


/*************************/
/* Module base addresses */
/*************************/

#define APB_BASE			LH7A404_APB_BASE

#define LH7A404_AC97_BASE	(APB_BASE + 0x0000)	/* AC97 Controller */
#define LH7A404_MMC_BASE	(APB_BASE + 0x0100)	/* Multimedia Card Controller */
#define LH7A404_USB_BASE	(APB_BASE + 0x0200)	/* USB Client */
#define LH7A404_SCI_BASE	(APB_BASE + 0x0300)	/* Smart Card Interface */
#define LH7A404_CSC_BASE	(APB_BASE + 0x0400)	/* Clock/State Controller */
#define LH7A404_UART1_BASE	(APB_BASE + 0x0600)	/* UART1 Controller */
#define LH7A404_SIR_BASE	(APB_BASE + 0x0600)	/* IR Controller, same as UART1 */
#define LH7A404_UART2_BASE	(APB_BASE + 0x0700)	/* UART2 Controller */
#define LH7A404_UART3_BASE	(APB_BASE + 0x0800)	/* UART3 Controller */
#define LH7A404_DCDC_BASE	(APB_BASE + 0x0900)	/* DC to DC Controller */
#define LH7A404_ACI_BASE	(APB_BASE + 0x0A00)	/* Audio Codec Interface */
#define LH7A404_SSP_BASE	(APB_BASE + 0x0B00)	/* Synchronous Serial Port */
#define LH7A404_TIMER_BASE	(APB_BASE + 0x0C00)	/* Timer Controller */
#define LH7A404_RTC_BASE	(APB_BASE + 0x0D00)	/* Real-time Clock */
#define LH7A404_GPIO_BASE	(APB_BASE + 0x0E00)	/* General Purpose IO */
#define LH7A404_BMI_BASE	(APB_BASE + 0x0F00)	/* Battery Monitor Interface */
#define LH7A404_ALI_BASE	(APB_BASE + 0x1000)	/* Advanced LCD Interface */
#define LH7A404_PWM_BASE	(APB_BASE + 0x1100)	/* Pulse Width Modulator */
#define LH7A404_KMI_BASE	(APB_BASE + 0x1200)	/* Keyboard and Mouse Interface */
#define LH7A404_ADC_BASE	(APB_BASE + 0x1300)	/* Analog-to-Digital Converter */
#define LH7A404_WDT_BASE	(APB_BASE + 0x1400)	/* Watchdog Timer */

#define AHB_BASE			LH7A404_AHB_BASE

#define LH7A404_SMC_BASE	(AHB_BASE + 0x0000)	/* Static Memory Controller */
#define LH7A404_SDMC_BASE	(AHB_BASE + 0x0400)	/* Synchronous Dynamic Memory Controller */
#define LH7A404_DMAC_BASE	(AHB_BASE + 0x0800)	/* DMA Controller */
#define LH7A404_CLCDC_BASE	(AHB_BASE + 0x1000)	/* Color LCD Controller */

#define LH7A404_VIC1_BASE	(AHB_BASE + 0x6000)	/* Vectored Interrupt Controller 1 */
#define LH7A404_USBH_BASE	(AHB_BASE + 0x7000)	/* USB OHCI host controller */
#define LH7A404_VIC2_BASE	(AHB_BASE + 0x8000)	/* Vectored Interrupt Controller 2 */



/**************************************/
/* Static Memory Controller registers */
/**************************************/

#define SMC_BASE		LH7A404_SMC_BASE

#define SMC_BCR0		__REG32(SMC_BASE + 0x00)	/* Bank 0 Configuration */
#define SMC_BCR1		__REG32(SMC_BASE + 0x04)	/* Bank 1 Configuration */
#define SMC_BCR2		__REG32(SMC_BASE + 0x08)	/* Bank 2 Configuration */
#define SMC_BCR3		__REG32(SMC_BASE + 0x0C)	/* Bank 3 Configuration */
#define SMC_BCR6		__REG32(SMC_BASE + 0x18)	/* Bank 6 Configuration */
#define SMC_BCR7		__REG32(SMC_BASE + 0x1C)	/* Bank 7 Configuration */
#define SMC_PC1ATTRIB	__REG32(SMC_BASE + 0x20)	/* PC Card 1 Attribute Space Config. */
#define SMC_PC1COM		__REG32(SMC_BASE + 0x24)	/* PC Card 1 Common Memory Space Config. */
#define SMC_PC1IO		__REG32(SMC_BASE + 0x28)	/* PC Card 1 I/O Space Configuration */
#define SMC_PC2ATTRIB	__REG32(SMC_BASE + 0x30)	/* PC Card 2 Attribute Space Config. */
#define SMC_PC2COM		__REG32(SMC_BASE + 0x34)	/* PC Card 2 Common Memory Space Config. */
#define SMC_PC2IO		__REG32(SMC_BASE + 0x38)	/* PC Card 2 I/O Space Configuration */
#define SMC_PCMCIACON	__REG32(SMC_BASE + 0x40)	/* PCMCIA Control */

/* bitfields in BCRx */
#define BCR_IDCY_SHFT	0
#define BCR_IDCY_MSK	(0x0F << 0)		/* Idle Cycle */
#define BCR_WST1_SHFT	5
#define BCR_WST1_MSK	(0x1F << 5)		/* Wait State 1 */
#define BCR_RBLE		(1 << 10)		/* Read Byte Lane Enable */
#define BCR_WST2_SHFT	11
#define BCR_WST2_MSK	(0x1F << 11)	/* Wait State 2 */
#define BCR_WPERR		(1 << 25)		/* Write Protect Error */
#define BCR_WP			(1 << 26)		/* Write Protect */
#define BCR_PME			(1 << 27)		/* Page Mode Enable */
#define BCR_MW_SHFT		28
#define BCR_MW_MSK		(0x03 << 28)	/* Memory Width */
#define BCR_MW_8		(0x00 << 28)
#define BCR_MW_16		(0x01 << 28)
#define BCR_MW_32		(0x02 << 28)

/* bitfields in PCxATTRIB */
#define PCATTR_PA_SHFT	0
#define PCATTR_PA_MSK	(0xFF << 0)		/* Pre-Charge Delay Time for Attribute Space */
#define PCATTR_HT_SHFT	8
#define PCATTR_HT_MSK	(0x0F << 8)		/* Hold Time */
#define PCATTR_AA_SHFT	16
#define PCATTR_AA_MSK	(0xFF << 16)	/* Access Time for Attribute Space */
#define PCATTR_WA_16	(1 << 31)		/* Width of Attribute Address Space (16 bit) */
#define PCATTR_WA_8		(0 << 31)		/* Width of Attribute Address Space (8 bit) */

/* bitfields in PCxCOM */
#define PCCOM_PC_SHFT	0
#define PCCOM_PC_MSK	(0xFF << 0)		/* Pre-Charge Delay Time for Common Memory Space */
#define PCCOM_HT_SHFT	8
#define PCCOM_HT_MSK	(0x0F << 8)		/* Hold Time */
#define PCCOM_AC_SHFT	16
#define PCCOM_AC_MSK	(0xFF << 16)	/* Access Time for Common Memory Space */
#define PCCOM_WC_16		(1 << 31)		/* Width of Common Memory Address Space (16 bit) */
#define PCCOM_WC_8		(0 << 31)		/* Width of Common Memory Address Space (8 bit) */

/* bitfields in PCxIO */
#define PCIO_PI_SHFT	0
#define PCIO_PI_MSK		(0xFF << 0)		/* Pre-Charge Delay Time for I/O Space */
#define PCIO_HT_SHFT	8
#define PCIO_HT_MSK		(0x0F << 8)		/* Hold Time */
#define PCIO_AI_SHFT	16
#define PCIO_AI_MSK		(0xFF << 16)	/* Access Time for I/O Space */
#define PCIO_WI_16		(1 << 31)		/* Width of I/O Address Space (16 bit) */
#define PCIO_WI_8		(0 << 31)		/* Width of I/O Address Space (8 bit) */

/* bits in PCMCIACON register */
#define PCMCIA_PC12EN	(0x03 << 0)		/* PC Card 1 and 2 Enable */
#define PCMCIA_PC1RST	(1 << 2)		/* PC Card 1 Reset */
#define PCMCIA_PC2RST	(1 << 3)		/* PC Card 2 Reset */
#define PCMCIA_WEN1		(1 << 4)		/* Wait State Enable for Card 1 */
#define PCMCIA_WEN2		(1 << 5)		/* Wait State Enable for Card 2 */
#define PCMCIA_MPREG	(1 << 9)		/* Manual Control of PCREG */



/***************************************************/
/* Synchronous Dynamic Memory Controller registers */
/***************************************************/

#define SDMC_BASE		LH7A404_SDMC_BASE

#define SDMC_GBLCNFG	__REG32(SDMC_BASE + 0x04)	/* Global Configuration */
#define SDMC_RFSHTMR	__REG32(SDMC_BASE + 0x08)	/* Refresh Timer */
#define SDMC_BOOTSTAT	__REG32(SDMC_BASE + 0x0C)	/* Boot Status */
#define SDMC_SDCSC0		__REG32(SDMC_BASE + 0x10)	/* Synchr. Domain Chip Select Config. 0 */
#define SDMC_SDCSC1		__REG32(SDMC_BASE + 0x14)	/* Synchr. Domain Chip Select Config. 1 */
#define SDMC_SDCSC2		__REG32(SDMC_BASE + 0x18)	/* Synchr. Domain Chip Select Config. 2 */
#define SDMC_SDCSC3		__REG32(SDMC_BASE + 0x1C)	/* Synchr. Domain Chip Select Config. 3 */

/* bits in GBLCNFG register */
#define GBLCNFG_INIT	(1 << 0)		/* Initialize */
#define GBLCNFG_MRS		(1 << 1)		/* Mode Register in Synchronous device */
#define GBLCNFG_SMEMBS	(1 << 5)		/* Synchronous Memory Busy State */
#define GBLCNFG_LCR		(1 << 6)		/* Load Command Register */
#define GBLCNFG_CKSD	(1 << 30)		/* Clock Shutdown */
#define GBLCNFG_CKE		(1 << 31)		/* Clock Enable */

/* bits in BOOTSTAT register */
#define BOOTSTAT_WIDTH	(0x03 << 0)		/* Boot Memory Width */
#define BOOTSTAT_MEDCHG	(1 << 2)		/* Media Change [1 = Synchr., 0 = Asynchr. ROM] */

/* bits in SDMC_SDCSCx register */
#define SDCSC_EBW		(1 << 2)		/* External Bus Width [1 = 16, 0 = 32 bit] */
#define SDCSC_BANKCOUNT	(1 << 3)		/* Bank Count [0 = 2 , 1 = 4 bank devices] */
#define SDCSC_SROM512	(1 << 4)		/* SROM Page Depth 512 */
#define SDCSC_SROMLL	(1 << 5)		/* SROM Lookalike */
#define SDCSC_2KPAGE	(1 << 6)		/* 2K Page Depth */
#define SDCSC_CASLAT	(0x07 << 16)	/* Column Address Strobe Latency */
#define SDCSC_WBL		(1 << 19)		/* Write Burst Length */
#define SDCSC_RASTOCAS	(0x03 << 20)	/* Row Address Strobe To Column Address Strobe */
#define SDCSC_APEN		(1 << 24)		/* Auto Precharge Enable */


/***********************************/
/* Clock/State Controller register */
/***********************************/

#define CSC_BASE		LH7A404_CSC_BASE

#define CSC_PWRSR		__REG32(CSC_BASE + 0x00)	/* Power Reset Register */
#define CSC_PWRCNT		__REG32(CSC_BASE + 0x04)	/* Power Control Register */
#define CSC_HALT		__REG32(CSC_BASE + 0x08)	/* Read to enter Halt */
#define CSC_STBY		__REG32(CSC_BASE + 0x0C)	/* Read to enter Standby */
#define CSC_BLEOI		__REG32(CSC_BASE + 0x10)	/* Battery Low End Of Interrupt reg */
#define CSC_MCEOI		__REG32(CSC_BASE + 0x14)	/* Media Changed End Of Interrupt reg */
#define CSC_TEOI		__REG32(CSC_BASE + 0x18)	/* Tick End Of Interrupt register */
#define CSC_STFCLR		__REG32(CSC_BASE + 0x1C)	/* STatus Flag CLeaR register */
#define CSC_CLKSET		__REG32(CSC_BASE + 0x20)	/* Clock Set register */
#define CSC_SCRREG0		__REG32(CSC_BASE + 0x40)	/* General purpose storage register 0 */
#define CSC_SCRREG1		__REG32(CSC_BASE + 0x44)	/* General purpose storage register 1 */
#define CSC_USBDRESET	__REG32(CSC_BASE + 0x4C)	/* USB Device Reset register */
#define CSC_BMAR		__REG32(CSC_BASE + 0x54)	/* Bus Master Arbitration Register */
#define CSC_BOOTCLR		__REG32(CSC_BASE + 0x58)	/* Boot ROM Clear Register */

/* Bits in Power Reset Register */
#define CSC_PWRSR_RTCDIV_MSK	0x0000003F		/* Real Time Clock Divisor */
#define CSC_PWRSR_MCDR			(1 << 6)		/* Media Change Direct Read */
#define CSC_PWRSR_DCDET			(1 << 7)		/* Direct Current Detect */
#define CSC_PWRSR_WUDR			(1 << 8)		/* Wakeup Direct */
#define CSC_PWRSR_WUON			(1 << 9)		/* Wake Up On */
#define CSC_PWRSR_NBFLG			(1 << 10)		/* New Battery Status Flag */
#define CSC_PWRSR_RSTFLG		(1 << 11)		/* User Reset Status Flag */
#define CSC_PWRSR_PFFLG			(1 << 12)		/* Power Fail Status Flag */
#define CSC_PWRSR_CLDFLG		(1 << 13)		/* Cold Start Status Flag */
#define CSC_PWRSR_LCKFLG		(1 << 14)		/* PLL2 Lock Flag */
#define CSC_PWRSR_WDTFLG		(1 << 15)		/* Watchdog Flag */
#define CSC_PWRSR_CHIPID_MSK	(0xFF << 16)	/* Chip Identification */
#define CSC_PWRSR_CHIPMAN_MSK	(0xFF << 24)	/* Chip Manufacturer ID [0x53 = 'S'] */

/* Bits in Power Control Register */
#define CSC_PWRCNT_WDTSEL		(1 << 0)		/* Watchdog Timer Reset Select */
#define CSC_PWRCNT_WAKEDIS		(1 << 1)		/* Wakeup Disable */
#define CSC_PWRCNT_PGMCLK_MSK	(0xFF << 8)		/* Program Clock Divisor */
#define CSC_PWRCNT_DMAM2PCH1	(1 << 16)		/* DMA M2P Clock Enable Channel 1 */
#define CSC_PWRCNT_DMAM2PCH0	(1 << 17)		/* DMA M2P Clock Enable Channel 0 */
#define CSC_PWRCNT_DMAM2PCH3	(1 << 18)		/* DMA M2P Clock Enable Channel 3 */
#define CSC_PWRCNT_DMAM2PCH2	(1 << 19)		/* DMA M2P Clock Enable Channel 2 */
#define CSC_PWRCNT_DMAM2PCH5	(1 << 20)		/* DMA M2P Clock Enable Channel 5 */
#define CSC_PWRCNT_DMAM2PCH4	(1 << 21)		/* DMA M2P Clock Enable Channel 4 */
#define CSC_PWRCNT_DMAM2PCH7	(1 << 22)		/* DMA M2P Clock Enable Channel 7 */
#define CSC_PWRCNT_DMAM2PCH6	(1 << 23)		/* DMA M2P Clock Enable Channel 6 */
#define CSC_PWRCNT_DMAM2PCH9	(1 << 24)		/* DMA M2P Clock Enable Channel 9 */
#define CSC_PWRCNT_DMAM2PCH8	(1 << 25)		/* DMA M2P Clock Enable Channel 8 */
#define CSC_PWRCNT_DMAM2MCH0	(1 << 26)		/* DMA M2M Clock Enable Channel 0 */
#define CSC_PWRCNT_DMAM2MCH1	(1 << 27)		/* DMA M2M Clock Enable Channel 1 */
#define CSC_PWRCNT_USHEN		(1 << 28)		/* USB Host Clock Enable */
#define CSC_PWRCNT_UARTBAUD		(1 << 29)		/* UART Baud Clock Source */

/* Bits in CLKSET Register */
#define CSC_CLKSET_HCLKDIV_SHFT		0
#define CSC_CLKSET_HCLKDIV_MSK		(0x03 << 0)		/* HCLK Divisor */
#define CSC_CLKSET_PREDIV_SHFT		2
#define CSC_CLKSET_PREDIV_MSK		(0x1F << 2)		/* Predivisor */
#define CSC_CLKSET_MAINDIV1_SHFT	7
#define CSC_CLKSET_MAINDIV1_MSK		(0x0F << 7)		/* Main Divisor 1 */
#define CSC_CLKSET_MAINDIV2_SHFT	11
#define CSC_CLKSET_MAINDIV2_MSK		(0x1F << 11)	/* Main Divisor 2 */
#define CSC_CLKSET_PCLKDIV_SHFT		16
#define CSC_CLKSET_PCLKDIV_MSK		(0x03 << 16)	/* PCLK Divisor */
#define CSC_CLKSET_PS_SHFT			18
#define CSC_CLKSET_PS_MSK			(0x03 << 18)	/* PS Divisor (2^PS) */
#define CSC_CLKSET_SMCROM			(1 << 24)		/* SMC Clock Disable */

/* Bits in USB Device Reset Register */
#define CSC_USBDRESET_IO		(1 << 0)		/* Reset USB Device I/O side */
#define CSC_USBDRESET_APB		(1 << 1)		/* Reset USB Device control side */




/*******************************************/
/* Vectored Interrupt Controller registers */
/*******************************************/

#define VIC1_BASE		LH7A404_VIC1_BASE

#define VIC1_IRQSTATUS	__REG32(VIC1_BASE + 0x00)	/* IRQ Status Register */
#define VIC1_FIQSTATUS	__REG32(VIC1_BASE + 0x04)	/* FIQ Status Register */
#define VIC1_RAWINTR	__REG32(VIC1_BASE + 0x08)	/* Raw Interrupt Status Register */
#define VIC1_INTSEL		__REG32(VIC1_BASE + 0x0C)	/* Interrupt Select Register */
#define VIC1_INTEN		__REG32(VIC1_BASE + 0x10)	/* Interrupt Enable Register */
#define VIC1_INTENCLR	__REG32(VIC1_BASE + 0x14)	/* Interrupt Enable Clear Register */
#define VIC1_SOFTINT	__REG32(VIC1_BASE + 0x18)	/* Software Interrupt Register */
#define VIC1_SOFTINTCLR	__REG32(VIC1_BASE + 0x1C)	/* Software Interrupt Clear Register */
#define VIC1_VECTADDR	__REG32(VIC1_BASE + 0x30)	/* Vector Address Register */
#define VIC1_NVADDR		__REG32(VIC1_BASE + 0x34)	/* Non-vectored Address Register */
#define VIC1_VAD0		__REG32(VIC1_BASE + 0x100)	/* Vector Address 0 Register */
#define VIC1_VECTCNTL0	__REG32(VIC1_BASE + 0x200)	/* Vector Control 0 Register */
#define VIC1_ITCR		__REG32(VIC1_BASE + 0x300)	/* Test Control Register */
#define VIC1_ITIP1		__REG32(VIC1_BASE + 0x304)	/* Test Input Register 1 */
#define VIC1_ITIP2		__REG32(VIC1_BASE + 0x308)	/* Test Input Register 2 */
#define VIC1_ITOP1		__REG32(VIC1_BASE + 0x30C)	/* Test Output Register 1 */
#define VIC1_ITOP2		__REG32(VIC1_BASE + 0x310)	/* Test Output Register 2 */

#define VIC2_BASE		LH7A404_VIC2_BASE

#define VIC2_IRQSTATUS	__REG32(VIC2_BASE + 0x00)	/* IRQ Status Register */
#define VIC2_FIQSTATUS	__REG32(VIC2_BASE + 0x04)	/* FIQ Status Register */
#define VIC2_RAWINTR	__REG32(VIC2_BASE + 0x08)	/* Raw Interrupt Status Register */
#define VIC2_INTSEL		__REG32(VIC2_BASE + 0x0C)	/* Interrupt Select Register */
#define VIC2_INTEN		__REG32(VIC2_BASE + 0x10)	/* Interrupt Enable Register */
#define VIC2_INTENCLR	__REG32(VIC2_BASE + 0x14)	/* Interrupt Enable Clear Register */
#define VIC2_SOFTINT	__REG32(VIC2_BASE + 0x18)	/* Software Interrupt Register */
#define VIC2_SOFTINTCLR	__REG32(VIC2_BASE + 0x1C)	/* Software Interrupt Clear Register */
#define VIC2_VECTADDR	__REG32(VIC2_BASE + 0x30)	/* Vector Address Register */
#define VIC2_NVADDR		__REG32(VIC2_BASE + 0x34)	/* Non-vectored Address Register */
#define VIC2_VAD0		__REG32(VIC2_BASE + 0x100)	/* Vector Address 0 Register */
#define VIC2_VECTCNTL0	__REG32(VIC2_BASE + 0x200)	/* Vector Control 0 Register */
#define VIC2_ITCR		__REG32(VIC2_BASE + 0x300)	/* Test Control Register */
#define VIC2_ITIP1		__REG32(VIC2_BASE + 0x304)	/* Test Input Register 1 */
#define VIC2_ITIP2		__REG32(VIC2_BASE + 0x308)	/* Test Input Register 2 */
#define VIC2_ITOP1		__REG32(VIC2_BASE + 0x30C)	/* Test Output Register 1 */
#define VIC2_ITOP2		__REG32(VIC2_BASE + 0x310)	/* Test Output Register 2 */

#define VIC_CNTL_ENABLE	(0x20)	/* Enable bit in VECTCNTLx register */


/*****************************************/
/* Direct Memory Access (DMA) Controller */
/*****************************************/

#define DMAC_BASE		LH7A404_DMAC_BASE

#define DMAC_M2PCH0_BASE	(DMAC_BASE + 0x0000)	/* M2P Channel 0 Registers (Tx) */
#define DMAC_M2PCH1_BASE	(DMAC_BASE + 0x0040)	/* M2P Channel 1 Registers (Rx) */
#define DMAC_M2PCH2_BASE	(DMAC_BASE + 0x0080)	/* M2P Channel 2 Registers (Tx) */
#define DMAC_M2PCH3_BASE	(DMAC_BASE + 0x00C0)	/* M2P Channel 3 Registers (Rx) */
#define DMAC_M2MCH0_BASE	(DMAC_BASE + 0x0100)	/* M2M Channel 0 Registers */
#define DMAC_M2MCH1_BASE	(DMAC_BASE + 0x0140)	/* M2M Channel 0 Registers */
#define DMAC_M2PCH5_BASE	(DMAC_BASE + 0x0200)	/* M2P Channel 5 Registers (Rx) */
#define DMAC_M2PCH4_BASE	(DMAC_BASE + 0x0240)	/* M2P Channel 4 Registers (Tx) */
#define DMAC_M2PCH7_BASE	(DMAC_BASE + 0x0280)	/* M2P Channel 7 Registers (Rx) */
#define DMAC_M2PCH6_BASE	(DMAC_BASE + 0x02C0)	/* M2P Channel 6 Registers (Tx) */
#define DMAC_M2PCH9_BASE	(DMAC_BASE + 0x0300)	/* M2P Channel 9 Registers (Rx) */
#define DMAC_M2PCH8_BASE	(DMAC_BASE + 0x0340)	/* M2P Channel 8 Registers (Tx) */

#define DMAC_GCA		__REG32(DMAC_BASE + 0x0380)	/* Global Channel Arbitration Register */
#define DMAC_GIR		__REG32(DMAC_BASE + 0x03C0)	/* Global Interrupt Register */


/************************/
/* Color LCD Controller */
/************************/

#define CLCDC_BASE		LH7A404_CLCDC_BASE

#define CLCDC_TIMING0	__REG32(CLCDC_BASE + 0x000)	/* Horizontal axis panel control */
#define CLCDC_TIMING1	__REG32(CLCDC_BASE + 0x004)	/* Vertical axis panel control */
#define CLCDC_TIMING2	__REG32(CLCDC_BASE + 0x008)	/* Clock and signal polarity control */
#define CLCDC_UPBASE	__REG32(CLCDC_BASE + 0x010)	/* Upper panel frame base address */
#define CLCDC_LPBASE	__REG32(CLCDC_BASE + 0x014)	/* Lower panel frame base address */
#define CLCDC_INTREN	__REG32(CLCDC_BASE + 0x018)	/* Interrupt enable mask */
#define CLCDC_CONTROL	__REG32(CLCDC_BASE + 0x01C)	/* LCD panel pixel parameters */
#define CLCDC_STATUS	__REG32(CLCDC_BASE + 0x020)	/* Raw interrupt status */
#define CLCDC_INTERRUPT	__REG32(CLCDC_BASE + 0x024)	/* Final masked interrupts */
#define CLCDC_UPCURR	__REG32(CLCDC_BASE + 0x028)	/* LCD upper panel current address value */
#define CLCDC_LPCURR	__REG32(CLCDC_BASE + 0x02C)	/* LCD lower panel current address value */
#define CLCDC_OVERFLOW	__REG32(CLCDC_BASE + 0x030)	/* SDRAM overflow frame buffer address */

/* LCD Palette registers: 256 entries with 16 bit */
#define CLCDC_PALETTE_BASE	(CLCDC_BASE + 0x300)

/* Bits in CLCDC Timing Register 0 */
#define CLCDC_TIMING0_PPL_SHFT	2
#define CLCDC_TIMING0_PPL_MSK	(0x3F << 2)		/* Pixels Per Line */
#define CLCDC_TIMING0_HSW_SHFT	8
#define CLCDC_TIMING0_HSW_MSK	(0xFF << 8)		/* Horizontal Synchronization Pulse Width */
#define CLCDC_TIMING0_HFP_SHFT	16
#define CLCDC_TIMING0_HFP_MSK	(0xFF << 16)	/* Horizontal Front Porch */
#define CLCDC_TIMING0_HBP_SHFT	24
#define CLCDC_TIMING0_HBP_MSK	(0xFF << 24)	/* Horizontal Back Porch */

/* Bits in CLCDC Timing Register 1 */
#define CLCDC_TIMING1_LPP_SHFT	0
#define CLCDC_TIMING1_LPP_MSK	(0x1FF << 0)	/* Lines Per Panel */
#define CLCDC_TIMING1_VSW_SHFT	10
#define CLCDC_TIMING1_VSW_MSK	(0x3F << 10)	/* Vertical Synchronization Pulse Width */
#define CLCDC_TIMING1_VFP_SHFT	16
#define CLCDC_TIMING1_VFP_MSK	(0xFF << 16)	/* Vertical Front Porch */
#define CLCDC_TIMING1_VBP_SHFT	24
#define CLCDC_TIMING1_VBP_MSK	(0xFF << 24)	/* Vertical Back Porch */

/* Bits in CLCDC Timing Register 2 */
#define CLCDC_TIMING2_PCD_SHFT	0
#define CLCDC_TIMING2_PCD_MSK	(0x1F << 0)		/* Panel Clock Divisor */
#define CLCDC_TIMING2_CSEL		(1 << 5)		/* Clock Select */
#define CLCDC_TIMING2_ACB_SHFT	6
#define CLCDC_TIMING2_ACB_MSK	(0x1F << 6)		/* AC Bias Signal Frequency */
#define CLCDC_TIMING2_IVS		(1 << 11)		/* Invert Vertical Synchronization */
#define CLCDC_TIMING2_IHS		(1 << 12)		/* Invert Horizontal Synchronization */
#define CLCDC_TIMING2_IPC		(1 << 13)		/* Invert Panel Clock */
#define CLCDC_TIMING2_IOE		(1 << 14)		/* Invert Output Enable */
#define CLCDC_TIMING2_CPL_SHFT	16
#define CLCDC_TIMING2_CPL_MSK	(0x3FF << 16)	/* Clocks Per Line */
#define CLCDC_TIMING2_BCD		(1 << 26)		/* Bypass Pixel Clock Divider */

/* Bits in CLCDC Interrupt Enable Register (INTREN) */
#define CLCDC_INTREN_FUIEN		(1 << 1)	/* FIFO Underflow Interrupt Enable */
#define CLCDC_INTREN_BUIEN		(1 << 2)	/* Next Base Update Interrupt Enable */
#define CLCDC_INTREN_VCIEN		(1 << 3)	/* Vertical Compare Interrupt Enable */
#define CLCDC_INTREN_MBEIEN		(1 << 4)	/* Bus Master Error Interrupt Enable */

/* Bits in CLCDC Control Register (CONTROL) */
#define CLCDC_CONTROL_LCDEN		(1 << 0)	/* Color LCD Controller Enable */
#define CLCDC_CONTROL_BPP_SHFT	1
#define CLCDC_CONTROL_BPP_MSK	(0x07 << 1)	/* Bits Per Pixel */
#define CLCDC_CONTROL_BPP_1		(0 << 1)
#define CLCDC_CONTROL_BPP_2		(1 << 1)
#define CLCDC_CONTROL_BPP_4		(2 << 1)
#define CLCDC_CONTROL_BPP_8		(3 << 1)
#define CLCDC_CONTROL_BPP_16	(4 << 1)
#define CLCDC_CONTROL_BW		(1 << 4)	/* Monochrome STN LCD */
#define CLCDC_CONTROL_TFT		(1 << 5)	/* TFT LCD */
#define CLCDC_CONTROL_MONO8L	(1 << 6)	/* Monochrome LCD uses 8-bit interface */
#define CLCDC_CONTROL_DUAL		(1 << 7)	/* Dual Panel STN LCD */
#define CLCDC_CONTROL_BGR		(1 << 8)	/* RGB or BGR */
#define CLCDC_CONTROL_BEBO		(1 << 9)	/* Big Endian Byte Ordering */
#define CLCDC_CONTROL_BEPO		(1 << 10)	/* Big Endian Pixel Ordering */
#define CLCDC_CONTROL_PWR		(1 << 11)	/* LCD Power Enable */
#define CLCDC_CONTROL_VCI_SHFT	12
#define CLCDC_CONTROL_VCI_MSK	(0x3 << 12)	/* LCD Vertical Compare */
#define CLCDC_CONTROL_WATERMARK	(1 << 16)	/* LCD DMA FIFO Watermark Level */

/* Bits in CLCDC Interrupt Status Register (STATUS) */
#define CLCDC_STATUS_FUI		(1 << 1)	/* FIFO Underflow */
#define CLCDC_STATUS_BUI		(1 << 2)	/* LCD Next Base Address Update */
#define CLCDC_STATUS_VCI		(1 << 3)	/* Vertical Compare */
#define CLCDC_STATUS_MBEI		(1 << 4)	/* AMBA AHB Master Bus Error Status */

/* Bits in CLCDC Interrupt Register (INTERRUPT) */
#define CLCDC_INTERRUPT_FUIM	(1 << 1)	/* Masked FIFO Underflow Interrupt */
#define CLCDC_INTERRUPT_BUIM	(1 << 2)	/* Masked LCD Next Base Address Update Interrupt */
#define CLCDC_INTERRUPT_VCIM	(1 << 3)	/* Masked Vertical Compare Interrupt */
#define CLCDC_INTERRUPT_MBEIM	(1 << 4)	/* Masked AMBA AHB Master Bus Error Interrupt */


/******************************************/
/* Advanced LCD Interface (ALI) registers */
/******************************************/

#define ALI_BASE		LH7A404_ALI_BASE

#define ALI_SETUP		__REG32(ALI_BASE + 0x00)	/* ALI Setup Register */
#define ALI_CONTROL		__REG32(ALI_BASE + 0x04)	/* ALI Control Register */
#define ALI_TIMING1		__REG32(ALI_BASE + 0x08)	/* ALI Timing Register 1 */
#define ALI_TIMING2		__REG32(ALI_BASE + 0x0C)	/* ALI Timing Register 2 */

/* Bits in ALI Setup Register */
#define ALI_SETUP_CR_MSK	(0x03)			/* Conversion Mode Select */
#define ALI_SETUP_CR_ACTIVE	0x01			/* Active Mode */
#define ALI_SETUP_CR_BYPASS	0x00			/* Bypass Mode (for STN or TFT panels) */
#define ALI_SETUP_LBR		(1 << 2)		/* Left Before Right */
#define ALI_SETUP_UBL		(1 << 3)		/* Upper Before Lower */
#define ALI_SETUP_PPL_SHIFT	4
#define ALI_SETUP_PPL_MSK	(0x1FF << 4)	/* Pixels Per Line */
#define ALI_SETUP_ALIEN		(1 << 13)		/* ALI Enable */

/* Bits in ALI Control Register */
#define ALI_CONTROL_LCDSPSEN	(1 << 0)	/* LCDSPS Enable (Row Reset) */
#define ALI_CONTROL_LCDCLSEN	(1 << 1)	/* LCDCLS Enable (Gate Driver Clock) */

/* Bits in ALI Timing Register 1 */
#define ALI_TIMING1_LPDEL_SHFT	0
#define ALI_TIMING1_LPDEL_MSK	(0x0F << 0)	/* LCDLP Delay */
#define ALI_TIMING1_REVDEL_SHFT	4
#define ALI_TIMING1_REVDEL_MSK	(0x0F << 4)	/* Polarity-Reversal Delay */
#define ALI_TIMING1_PSCLS_SHFT	8
#define ALI_TIMING1_PSCLS_MSK	(0x0F << 8)	/* LCDPS and LCDCLS Delay */

/* Bits in ALI Timing Register 2 */
#define ALI_TIMING2_PS2CLS2_SHFT	0
#define ALI_TIMING2_PS2CLS2_MSK		(0x1FF)		/* LCDSPL and LCDCLS Delay 2 */
#define ALI_TIMING2_SPLDEL_SHFT		9
#define ALI_TIMING2_SPLDEL_MSK		(0x7F << 9)	/* LCDSPL Delay */



/*******************************************/
/* Synchronous Serial Port (SSP) registers */
/*******************************************/

#define SSP_BASE		LH7A404_SSP_BASE

#define SSP_CR0			__REG32(SSP_BASE + 0x00) /* Control Register 0 */
#define SSP_CR1			__REG32(SSP_BASE + 0x04) /* Control Register 1 */
#define SSP_IIR			__REG32(SSP_BASE + 0x08) /* Interrupt Identification Register (RO) */
#define SSP_ROEOI		__REG32(SSP_BASE + 0x08) /* Receive Overrun End-of-Interrupt (WO) */
#define SSP_DR			__REG32(SSP_BASE + 0x0C) /* Data Register */
#define SSP_CPR			__REG32(SSP_BASE + 0x10) /* Clock Prescale Register */
#define SSP_SR			__REG32(SSP_BASE + 0x14) /* Status Register */

/* Bits in SSP Control Register 0 */
#define SSP_CR0_DSS		(0x0F << 0)		/* Data Size Selection */
#define SSP_CR0_DSS_16		(0x0F << 0)	/* 16 bit */
#define SSP_CR0_DSS_15		(0x0E << 0)	/* 15 bit */
#define SSP_CR0_DSS_14		(0x0D << 0)	/* 14 bit */
#define SSP_CR0_DSS_13		(0x0C << 0)	/* 13 bit */
#define SSP_CR0_DSS_12		(0x0B << 0)	/* 12 bit */
#define SSP_CR0_DSS_11		(0x0A << 0)	/* 11 bit */
#define SSP_CR0_DSS_10		(0x09 << 0)	/* 10 bit */
#define SSP_CR0_DSS_9		(0x08 << 0)	/*  9 bit */
#define SSP_CR0_DSS_8		(0x07 << 0)	/*  8 bit */
#define SSP_CR0_DSS_7		(0x06 << 0)	/*  7 bit */
#define SSP_CR0_DSS_6		(0x05 << 0)	/*  6 bit */
#define SSP_CR0_DSS_5		(0x04 << 0)	/*  5 bit */
#define SSP_CR0_DSS_4		(0x03 << 0)	/*  4 bit */
#define SSP_CR0_FRF		(0x03 << 4)		/* Frame Format */
#define SSP_CR0_FRF_MOT		(0x00 << 4)	/* Motorola SPI frame format */
#define SSP_CR0_FRF_TI		(0x01 << 4)	/* TI synchronous serial frame format */
#define SSP_CR0_FRF_NAT		(0x02 << 4)	/* National MICROWIRE frame format */
#define SSP_CR0_SSE		(1 << 7)		/* SSP Enable */
#define SSP_CR0_SCR		(0xFF << 8)		/* Serial Clock Rate */

/* Bits in SSP Control Register 1 */
#define SSP_CR1_RXSIEN		(1 << 0)	/* Receive FIFO Service Interrupt Enable */
#define SSP_CR1_TXSIEN		(1 << 1)	/* Transmit FIFO Service Interrupt Enable */
#define SSP_CR1_LBM			(1 << 2)	/* Loopback Mode */
#define SSP_CR1_SPO			(1 << 3)	/* SPI Polarity */
#define SSP_CR1_SPH			(1 << 4)	/* SPI Phase */
#define SSP_CR1_RXOIEN		(1 << 5)	/* Receive Overrun Interrupt Enable */
#define SSP_CR1_FEN			(1 << 6)	/* FIFO Enable */
#define SSP_CR1_TXIIEN		(1 << 7)	/* Transmit Idle Interrupt Enable */

/* Bits in SSP Interrupt Identification Register */
#define SSP_IIR_RXSI		(1 << 0)	/* Receive FIFO Service Request Interrupt */
#define SSP_IIR_TXSI		(1 << 1)	/* Transmit FIFO Service Request Interrupt */
#define SSP_IIR_RXOI		(1 << 6)	/* Receive FIFO Overrun Interrupt */
#define SSP_IIR_TXII		(1 << 7)	/* Transmit Idle Interrupt */

/* Bits in SSP Status Register */
#define SSP_SR_TNF			(1 << 1)	/* Transmit FIFO Not Full */
#define SSP_SR_RNE			(1 << 2)	/* Receive FIFO Not Empty */
#define SSP_SR_BSY			(1 << 3)	/* Busy */
#define SSP_SR_THE			(1 << 4)	/* Transmit FIFO Half Empty */
#define SSP_SR_RHF			(1 << 5)	/* Receive FIFO Half Full */
#define SSP_SR_ROR			(1 << 6)	/* Receive Overrun */
#define SSP_SR_TFE			(1 << 7)	/* Transmit FIFO Empty */
#define SSP_SR_RFF			(1 << 8)	/* Receive FIFO Full */



/*******************/
/* Timer registers */
/*******************/

#define TIMER_BASE		LH7A404_TIMER_BASE

#define TIMER_LOAD1		__REG32(TIMER_BASE + 0x00) /* Timer 1 initial value */
#define TIMER_VALUE1	__REG32(TIMER_BASE + 0x04) /* Timer 1 current value */
#define TIMER_CONTROL1	__REG32(TIMER_BASE + 0x08) /* Timer 1 control word */
#define TIMER_EOI1		__REG32(TIMER_BASE + 0x0C) /* Timer 1 interrupt clear */

#define TIMER_LOAD2		__REG32(TIMER_BASE + 0x20) /* Timer 2 initial value */
#define TIMER_VALUE2	__REG32(TIMER_BASE + 0x24) /* Timer 2 current value */
#define TIMER_CONTROL2	__REG32(TIMER_BASE + 0x28) /* Timer 2 control word */
#define TIMER_EOI2		__REG32(TIMER_BASE + 0x2C) /* Timer 2 interrupt clear */

#define TIMER_BUZZCON	__REG32(TIMER_BASE + 0x40) /* Buzzer configuration */

#define TIMER_LOAD3		__REG32(TIMER_BASE + 0x80) /* Timer 3 initial value */
#define TIMER_VALUE3	__REG32(TIMER_BASE + 0x84) /* Timer 3 current value */
#define TIMER_CONTROL3	__REG32(TIMER_BASE + 0x88) /* Timer 3 control word */
#define TIMER_EOI3		__REG32(TIMER_BASE + 0x8C) /* Timer 3 interrupt clear */

#define TIMER_C_ENABLE		(1 << 7)
#define TIMER_C_PERIODIC	(1 << 6)
#define TIMER_C_FREERUNNING	(0)
#define TIMER_C_2KHZ		(0x00)		/*   1.994 kHz */
#define TIMER_C_508KHZ		(0x08)		/* 508.469 kHz */


/******************/
/* GPIO registers */
/******************/
/*
	Note:
	Direction of port pin has different meaning for ports
	Port C,D,G:     0 == output, 1 == input
	Port A,B,E,F,H: 0 == input,  1 == output
 */

#define GPIO_BASE		LH7A404_GPIO_BASE

#define GPIO_PAD			__REG32(GPIO_BASE + 0x00)	/* Port A Data register */
#define GPIO_PBD			__REG32(GPIO_BASE + 0x04)	/* Port B Data register */
#define GPIO_PCD			__REG32(GPIO_BASE + 0x08)	/* Port C Data register */
#define GPIO_PDD			__REG32(GPIO_BASE + 0x0C)	/* Port D Data register */
#define GPIO_PADD			__REG32(GPIO_BASE + 0x10)	/* Port A Data Direction register */
#define GPIO_PBDD			__REG32(GPIO_BASE + 0x14)	/* Port B Data Direction register */
#define GPIO_PCDD			__REG32(GPIO_BASE + 0x18)	/* Port C Data Direction register */
#define GPIO_PDDD			__REG32(GPIO_BASE + 0x1C)	/* Port D Data Direction register */
#define GPIO_PED			__REG32(GPIO_BASE + 0x20)	/* Port E Data register */
#define GPIO_PEDD			__REG32(GPIO_BASE + 0x24)	/* Port E Data Direction register */
#define GPIO_KBDCTL			__REG32(GPIO_BASE + 0x28)	/* Keyboard Control register */
#define GPIO_PINMUX			__REG32(GPIO_BASE + 0x2C)	/* Pin Multiplexing register */
#define GPIO_PFD			__REG32(GPIO_BASE + 0x30)	/* Port F Data register */
#define GPIO_PFDD			__REG32(GPIO_BASE + 0x34)	/* Port F Data Direction register */
#define GPIO_PGD			__REG32(GPIO_BASE + 0x38)	/* Port G Data register */
#define GPIO_PGDD			__REG32(GPIO_BASE + 0x3C)	/* Port G Data Direction register */
#define GPIO_PHD			__REG32(GPIO_BASE + 0x40)	/* Port H Data register */
#define GPIO_PHDD			__REG32(GPIO_BASE + 0x44)	/* Port H Data Direction register */
#define GPIO_INTTYPE1		__REG32(GPIO_BASE + 0x4C)	/* IRQ edge (1) or lvl (0)  */
#define GPIO_INTTYPE2		__REG32(GPIO_BASE + 0x50)	/* IRQ activ hi/lo or rising/falling */
#define GPIO_GPIOFEOI		__REG32(GPIO_BASE + 0x54)	/* GPIOF end of IRQ */
#define GPIO_GPIOINTEN		__REG32(GPIO_BASE + 0x58)	/* GPIOF IRQ enable */
#define GPIO_INTSTATUS		__REG32(GPIO_BASE + 0x5C)	/* GPIOF IRQ latch */
#define GPIO_RAWINTSTATUS	__REG32(GPIO_BASE + 0x60)	/* GPIOF IRQ raw */
#define GPIO_GPIODB			__REG32(GPIO_BASE + 0x64)	/* GPIOF Debounce */
#define GPIO_PAPD			__REG32(GPIO_BASE + 0x68)	/* Port A Pin Data register */
#define GPIO_PBPD			__REG32(GPIO_BASE + 0x6C)	/* Port B Pin Data register */
#define GPIO_PCPD			__REG32(GPIO_BASE + 0x70)	/* Port C Pin Data register */
#define GPIO_PDPD			__REG32(GPIO_BASE + 0x74)	/* Port D Pin Data register */
#define GPIO_PEPD			__REG32(GPIO_BASE + 0x78)	/* Port E Pin Data register */
#define GPIO_PFPD			__REG32(GPIO_BASE + 0x7C)	/* Port F Pin Data register */
#define GPIO_PGPD			__REG32(GPIO_BASE + 0x80)	/* Port G Pin Data register */
#define GPIO_PHPD			__REG32(GPIO_BASE + 0x84)	/* Port H Pin Data register */

/* Bits in GPIO_PINMUX */
#define GPIO_PINMUX_PEOCON		(1 << 0)	/* Port E Output Control */
#define GPIO_PINMUX_PDOCON		(1 << 1)	/* Port D Output Control */
#define GPIO_PINMUX_CODECON		(1 << 2)	/* Codec Control (AC97) */
#define GPIO_PINMUX_UART3CON	(1 << 3)	/* UART3 Control */



/******************/
/* UART registers */
/******************/

#define UART1_BASE		LH7A404_UART1_BASE

#define UART1_DATA		__REG32(UART1_BASE + 0x00)	/* Data Register */
#define UART1_FCON		__REG32(UART1_BASE + 0x04)	/* FIFO Control Register */
#define UART1_BRCON		__REG32(UART1_BASE + 0x08)	/* Baud Rate Control Register */
#define UART1_CON		__REG32(UART1_BASE + 0x0C)	/* Control Register */
#define UART1_STATUS	__REG32(UART1_BASE + 0x10)	/* Status Register */
#define UART1_RAWISR	__REG32(UART1_BASE + 0x14)	/* Raw Interrupt Status Register */
#define UART1_INTEN		__REG32(UART1_BASE + 0x18)	/* Interrupt Mask Register */
#define UART1_MISR		__REG32(UART1_BASE + 0x1C)	/* Masked Interrupt Status Register */
#define UART1_RES		__REG32(UART1_BASE + 0x20)	/* Receive Error Status + Clear Register */
#define UART1_EIC		__REG32(UART1_BASE + 0x24)	/* Error Interrupt Clear Register */
#define UART1_DMACR		__REG32(UART1_BASE + 0x28)	/* DMA Control Register */

#define UART2_BASE		LH7A404_UART2_BASE

#define UART2_DATA		__REG32(UART2_BASE + 0x00)	/* Data Register */
#define UART2_FCON		__REG32(UART2_BASE + 0x04)	/* FIFO Control Register */
#define UART2_BRCON		__REG32(UART2_BASE + 0x08)	/* Baud Rate Control Register */
#define UART2_CON		__REG32(UART2_BASE + 0x0C)	/* Control Register */
#define UART2_STATUS	__REG32(UART2_BASE + 0x10)	/* Status Register */
#define UART2_RAWISR	__REG32(UART2_BASE + 0x14)	/* Raw Interrupt Status Register */
#define UART2_INTEN		__REG32(UART2_BASE + 0x18)	/* Interrupt Mask Register */
#define UART2_MISR		__REG32(UART2_BASE + 0x1C)	/* Masked Interrupt Status Register */
#define UART2_RES		__REG32(UART2_BASE + 0x20)	/* Receive Error Status + Clear Register */
#define UART2_EIC		__REG32(UART2_BASE + 0x24)	/* Error Interrupt Clear Register */
#define UART2_DMACR		__REG32(UART2_BASE + 0x28)	/* DMA Control Register */

#define UART3_BASE		LH7A404_UART3_BASE

#define UART3_DATA		__REG32(UART3_BASE + 0x00)	/* Data Register */
#define UART3_FCON		__REG32(UART3_BASE + 0x04)	/* FIFO Control Register */
#define UART3_BRCON		__REG32(UART3_BASE + 0x08)	/* Baud Rate Control Register */
#define UART3_CON		__REG32(UART3_BASE + 0x0C)	/* Control Register */
#define UART3_STATUS	__REG32(UART3_BASE + 0x10)	/* Status Register */
#define UART3_RAWISR	__REG32(UART3_BASE + 0x14)	/* Raw Interrupt Status Register */
#define UART3_INTEN		__REG32(UART3_BASE + 0x18)	/* Interrupt Mask Register */
#define UART3_MISR		__REG32(UART3_BASE + 0x1C)	/* Masked Interrupt Status Register */
#define UART3_RES		__REG32(UART3_BASE + 0x20)	/* Receive Error Status + Clear Register */
#define UART3_EIC		__REG32(UART3_BASE + 0x24)	/* Error Interrupt Clear Register */
#define UART3_DMACR		__REG32(UART3_BASE + 0x28)	/* DMA Control Register */

/* Bits in DATA register (error flags on RX) */
#define UART_DATA_MSK	(0xFF << 0)
#define UART_DATA_FE	(1 << 8)	/* Framing Error */
#define UART_DATA_PE	(1 << 9)	/* Parity Error */
#define UART_DATA_OE	(1 << 10)	/* Overrun Error */
#define UART_DATA_BE	(1 << 11)	/* Break Error */

/* Bits in FCON register */
#define UART_FCON_BRK		(1 << 0)	/* Break (Assert Break) */
#define UART_FCON_PEN		(1 << 1)	/* Parity Enable */
#define UART_FCON_EPS		(1 << 2)	/* Even Parity Set */
#define UART_FCON_STP2		(1 << 3)	/* Stop bits (1 = 2 stop bits, 0 = 1 stop bit) */
#define UART_FCON_FEN		(1 << 4)	/* FIFO Enable */
#define UART_FCON_WLEN5		(0 << 5)	/* Word Length: 5 bit data */
#define UART_FCON_WLEN6		(1 << 5)	/* Word Length: 6 bit data */
#define UART_FCON_WLEN7		(2 << 5)	/* Word Length: 7 bit data */
#define UART_FCON_WLEN8		(3 << 5)	/* Word Length: 8 bit data */

/* Bits in CON register */
#define UART_CON_UARTEN		(1 << 0)	/* UART Enable */
#define UART_CON_SIRD		(1 << 1)	/* Serial Infrared disable (UART1) */
#define UART_CON_SIRLP		(1 << 2)	/* Serial Infrared Low Power (UART1) */
#define UART_CON_RXP		(1 << 3)	/* Receive Polarity */
#define UART_CON_TXP		(1 << 4)	/* Transmit Polarity */
#define UART_CON_MXP		(1 << 5)	/* Modem Transfer Polarity */
#define UART_CON_LBE		(1 << 6)	/* Loopback Enable */
#define UART_CON_SIRBD		(1 << 7)	/* Serial Infrared Blanking Disable (UART1) */

/* Bits in STATUS register */
#define UART_STATUS_CTS		(1 << 0)	/* Clear To Send (UART2/3) */
#define UART_STATUS_DSR		(1 << 1)	/* Data Set Ready (UART2/3) */
#define UART_STATUS_DCD		(1 << 2)	/* Data Carrier Detect (UART2/3) */
#define UART_STATUS_BUSY	(1 << 3)	/* UART busy */
#define UART_STATUS_RXFE	(1 << 4)	/* Receive FIFO empty */
#define UART_STATUS_TXFF	(1 << 5)	/* Transmit FIFO full */
#define UART_STATUS_RXFF	(1 << 6)	/* Receive FIFO full */
#define UART_STATUS_TXFE	(1 << 7)	/* Transmit FIFO empty */

/* Bits in RAWISR register (Read) */
#define UART_RAWISR_RI		(1 << 0)	/* Receive Interrupt */
#define UART_RAWISR_TI		(1 << 1)	/* Transmit Interrupt */
#define UART_RAWISR_MI		(1 << 2)	/* Modem Status Interrupt */
#define UART_RAWISR_RTI		(1 << 3)	/* Receive Timeout Interrupt */
#define UART_RAWISR_FEI		(1 << 4)	/* Frame Error Interrupt */
#define UART_RAWISR_PEI		(1 << 5)	/* Parity Error Interrupt */
#define UART_RAWISR_BEI		(1 << 6)	/* Break Error Interrupt */
#define UART_RAWISR_OEI		(1 << 7)	/* Overrun Error Interrupt */

/* Bits in INTEN register */
#define UART_INTEN_REN		(1 << 0)	/* Receive Interrupt Enable */
#define UART_INTEN_TEN		(1 << 1)	/* Transmit Interrupt Enable */
#define UART_INTEN_MEN		(1 << 2)	/* Modem Status Interrupt Enable */
#define UART_INTEN_RTEN		(1 << 3)	/* Receive Timeout Interrupt Enable */
#define UART_INTEN_FEEN		(1 << 4)	/* Frame Error Interrupt Enable */
#define UART_INTEN_PEEN		(1 << 5)	/* Parity Error Interrupt Enable */
#define UART_INTEN_BEEN		(1 << 6)	/* Break Error Interrupt Enable */
#define UART_INTEN_OEEN		(1 << 7)	/* Overrun Error Interrupt Enable */

/* Bits in MISR register */
#define UART_MISR_RIM		(1 << 0)	/* Masked Receive Interrupt */
#define UART_MISR_TIM		(1 << 1)	/* Masked Transmit Interrupt */
#define UART_MISR_MIM		(1 << 2)	/* Masked Modem Status Interrupt */
#define UART_MISR_RTIM		(1 << 3)	/* Masked Receive Timeout Interrupt */
#define UART_MISR_FEIM		(1 << 4)	/* Masked Frame Error Interrupt */
#define UART_MISR_PEIM		(1 << 5)	/* Masked Parity Error Interrupt */
#define UART_MISR_BEIM		(1 << 6)	/* Masked Break Error Interrupt */
#define UART_MISR_OEIM		(1 << 7)	/* Masked Overrun Error Interrupt */

/* Bits in RES register */
#define UART_RES_FE			(1 << 0)	/* Frame Error */
#define UART_RES_PE			(1 << 1)	/* Parity Error */
#define UART_RES_OE			(1 << 2)	/* Overrun Error */
#define UART_RES_BE			(1 << 3)	/* Break Error */

/* Bits in EIC register */
#define UART_EIC_FEIC		(1 << 0)	/* Masked Frame Error Interrupt Clear */
#define UART_EIC_PEIC		(1 << 1)	/* Masked Parity Error Interrupt Clear */
#define UART_EIC_OEIC		(1 << 2)	/* Masked Overrun Error Interrupt Clear */
#define UART_EIC_BEIC		(1 << 3)	/* Masked Break Error Interrupt Clear */

/* Bits in DMACR register */
#define UART_DMACR_RDE		(1 << 0)	/* Receive DMA Enable */
#define UART_DMACR_TDE		(1 << 1)	/* Transmit DMA Enable */
#define UART_DMACR_DE		(1 << 2)	/* DMA On Error */



/***********************************/
/* MultiMediaCard (MMC) Controller */
/***********************************/

#define MMC_BASE		LH7A404_MMC_BASE

#define MMC_CLKC		__REG32(MMC_BASE + 0x00)	/* Clock control register */
#define MMC_STATUS		__REG32(MMC_BASE + 0x04)	/* Controller status register */
#define MMC_RATE		__REG32(MMC_BASE + 0x08)	/* SD/MMC clock divider register */
#define MMC_PREDIV		__REG32(MMC_BASE + 0x0C)	/* SD/MMC predivide register */
#define MMC_CMDCON		__REG32(MMC_BASE + 0x14)	/* Command control register */
#define MMC_RES_TO		__REG32(MMC_BASE + 0x18)	/* Response timeout register */
#define MMC_READ_TO		__REG32(MMC_BASE + 0x1C)	/* Read timeout register */
#define MMC_BLK_LEN		__REG32(MMC_BASE + 0x20)	/* Block length register */
#define MMC_NOB			__REG32(MMC_BASE + 0x24)	/* Number Of Blocks register */
#define MMC_INT_STATUS	__REG32(MMC_BASE + 0x28)	/* Interrupt status register */
#define MMC_EOI			__REG32(MMC_BASE + 0x2C)	/* End Of Interrupt register */
#define MMC_INT_MASK	__REG32(MMC_BASE + 0x34)	/* Interrupt mask register */
#define MMC_CMD			__REG32(MMC_BASE + 0x38)	/* Command number register */
#define MMC_ARGUMENT	__REG32(MMC_BASE + 0x3C)	/* Command argument register */
#define MMC_RES_FIFO	__REG32(MMC_BASE + 0x40)	/* Response FIFO location */
#define MMC_DATA_FIFO	__REG32(MMC_BASE + 0x48)	/* FIFO data register */

/* Bits in MMC_CLKC */
#define MMC_CLKC_STOP_CLK	(1 << 0)	/* Stop Clock */
#define MMC_CLKC_START_CLK	(1 << 1)	/* Start Clock */

/* Bits in MMC_STATUS */
#define MMC_STATUS_TOREAD		(1 << 0)	/* Read Timeout */
#define MMC_STATUS_TORES		(1 << 1)	/* Response Timeout */
#define MMC_STATUS_CRCWRITE		(1 << 2)	/* CRC Write Error */
#define MMC_STATUS_CRCREAD		(1 << 3)	/* CRC Read Error */
#define MMC_STATUS_CRC			(1 << 5)	/* Response CRC Error */
#define MMC_STATUS_FIFO_EMPTY	(1 << 6)	/* FIFO Empty */
#define MMC_STATUS_FIFO_FULL	(1 << 7)	/* FIFO Full */
#define MMC_STATUS_CLK_DIS		(1 << 8)	/* Clock Disabled */
#define MMC_STATUS_TRANDONE		(1 << 11)	/* Data Transfer Done */
#define MMC_STATUS_DONE			(1 << 12)	/* Program Done */
#define MMC_STATUS_ENDRESP		(1 << 13)	/* End Command Response */

/* Bits in MMC_PREDIV */
#define MMC_PREDIV_MMC_PREDIV	(0x0F << 0)	/* SD/MMC Predivisor */
#define MMC_PREDIV_MMC_EN		(1 << 4)	/* SD/MMC Enable */
#define MMC_PREDIV_APB_RD_EN	(1 << 5)	/* APB Read Enable */

/* Bits in MMC_CMDCON */
#define MMC_CMDCON_ABORT			(1 << 13)	/* Abort Transaction */
#define MMC_CMDCON_SET_READ_WRITE	(1 << 12)	/* Set Transaction to Read (0) or Write (1) */
#define MMC_CMDCON_MULTI_BLK4_INTEN	(1 << 11)	/* Multiple Block Interrupt Enable */
#define MMC_CMDCON_READ_WAIT_EN		(1 << 10)	/* Enable Read Wait States */
#define MMC_CMDCON_SDIO_EN			(1 << 9)	/* Enable SD Input/Output */
#define MMC_CMDCON_BIG_ENDIAN		(1 << 8)	/* Enable Big Endian Mode for MMC */
#define MMC_CMDCON_WIDE				(1 << 7)	/* Enable Wide Mode for SD */
#define MMC_CMDCON_INITIALIZE		(1 << 6)	/* Enable 80-bit Initialization Sequence */
#define MMC_CMDCON_BUSY				(1 << 5)	/* Busy Expected */
#define MMC_CMDCON_STREAM			(1 << 4)	/* Stream Mode */
#define MMC_CMDCON_WRITE			(1 << 3)	/* Write Transfer Mode */
#define MMC_CMDCON_DATA_EN			(1 << 2)	/* Data Transfer Enable */
#define MMC_CMDCON_RESP_FORMAT_NONE	(0 << 0)	/* No Response */
#define MMC_CMDCON_RESP_FORMAT_R1	(1 << 0)	/* Response Format R1 */
#define MMC_CMDCON_RESP_FORMAT_R2	(2 << 0)	/* Response Format R2 */
#define MMC_CMDCON_RESP_FORMAT_R3	(3 << 0)	/* Response Format R3 */

/* Bits in MMC_INT_STATUS, MMC_EOI + MMC_INT_MASK */
#define MMC_INT_SDIO_INT			(1 << 5)	/* SD Interrupt */
#define MMC_INT_BUS_CLOCK_STOPPED	(1 << 4)	/* Bus Clock Stopped Interrupt */
#define MMC_INT_BUF_READY			(1 << 3)	/* Buffer Ready Interrupt */
#define MMC_INT_END_CMD				(1 << 2)	/* End Command Response Interrupt */
#define MMC_INT_DONE				(1 << 1)	/* Program Done Interrupt */
#define MMC_INT_DATA_TRAN			(1 << 0)	/* Data Transfer Done Interrupt */


/*************************************/
/* Universal Serial Bus (USB) Device */
/*************************************/

#define USB_BASE		LH7A404_USB_BASE

#define USB_FAR			__REG32(USB_BASE + 0x00)	/* Function Address Register */
#define USB_PMR			__REG32(USB_BASE + 0x04)	/* Power Management Register */
#define USB_IIR			__REG32(USB_BASE + 0x08)	/* IN Interrupt Register Bank */
#define USB_OIR			__REG32(USB_BASE + 0x10)	/* OUT Interrupt Register Bank */
#define USB_UIR			__REG32(USB_BASE + 0x18)	/* USB Interrupt Register Bank */
#define USB_IIE			__REG32(USB_BASE + 0x1C)	/* IN Interrupt Enable Register Bank */
#define USB_OIE			__REG32(USB_BASE + 0x24)	/* OUT Interrupt Enable Register Bank */
#define USB_UIE			__REG32(USB_BASE + 0x2C)	/* USB Interrupt Enable Register Bank */
#define USB_FRAME1		__REG32(USB_BASE + 0x30)	/* Frame Number1 Register */
#define USB_FRAME2		__REG32(USB_BASE + 0x34)	/* Frame Number2 Register */
#define USB_INDEX		__REG32(USB_BASE + 0x38)	/* Index Register */
#define USB_INMAXP		__REG32(USB_BASE + 0x40)	/* IN Maximum Packet Size Register */
#define USB_INCSR1		__REG32(USB_BASE + 0x44)	/* Control and Status Register (EP0,1,3) */
#define USB_INCSR2		__REG32(USB_BASE + 0x48)	/* IN Control Register (EP1,EP3) */
#define USB_OUTMAXP		__REG32(USB_BASE + 0x4C)	/* OUT Maximum Packet Size Register */
#define USB_OUTCSR1		__REG32(USB_BASE + 0x50)	/* OUT Control and Status Register */
#define USB_OUTCSR2		__REG32(USB_BASE + 0x54)	/* OUT Control Register */
#define USB_COUNT1		__REG32(USB_BASE + 0x58)	/* OUT FIFO Write Count1 Register */
#define USB_EP0FIFO		__REG32(USB_BASE + 0x80)	/* EP0 (Control, 8 Bytes) */
#define USB_EP1FIFO		__REG32(USB_BASE + 0x84)	/* EP1 (IN BULK, 64 Bytes) */
#define USB_EP2FIFO		__REG32(USB_BASE + 0x88)	/* EP2 (OUT BULK, 64 Bytes) */
#define USB_EP3FIFO		__REG32(USB_BASE + 0x8C)	/* EP3 (IN Interrupt, 64 Bytes) */

/* Bits in USB_FAR */
#define USB_FAR_FUNCTION_ADDR	(0x7F << 0)	/* Function Address field */
#define USB_FAR_ADDR_UPDATE		(1 << 7)	/* Address Update */

/* Bits in USB_PMR */
#define USB_PMR_ENABLE_SUSPEND	(1 << 0)	/* SUSPEND Enable */
#define USB_PMR_SUSPEND_MODE	(1 << 1)	/* SUSPEND Mode active */
#define USB_PMR_UC_RESUME		(1 << 2)	/* UC RESUME */
#define USB_PMR_USB_RESET		(1 << 3)	/* USB RESET active */
#define USB_PMR_USB_ENABLE		(1 << 4)	/* USB Enable */
#define USB_PMR_DCP_CTRL		(1 << 5)	/* DCP Pin Control (0=pulled up, 1=floating) */

/* Bits in USB_IIR */
#define USB_IIR_EP0				(1 << 0)	/* End Point 0 Interrupt */
#define USB_IIR_EP1IN			(1 << 1)	/* End Point 1 IN Interrupt */
#define USB_IIR_EP3IN			(1 << 3)	/* End Point 3 IN Interrupt */

/* Bits in USB_OIR */
#define USB_OIR_EP2OUT			(1 << 2)	/* End Point 2 OUT Interrupt */

/* Bits in USB_UIR */
#define USB_UIR_SUSINT			(1 << 0)	/* SUSPEND Interrupt */
#define USB_UIR_RESINT			(1 << 1)	/* RESUME Interrupt */
#define USB_UIR_URINT			(1 << 2)	/* USB RESET Interrupt */

/* Bits in USB_IIE */
#define USB_IIE_EP0EN			(1 << 0)	/* End Point 0 Interrupt Enable */
#define USB_IIE_EP1INEN			(1 << 1)	/* End Point 1 IN Interrupt Enable */
#define USB_IIE_EP3INEN			(1 << 3)	/* End Point 3 IN Interrupt Enable */

/* Bits in USB_OIE */
#define USB_OIE_EP2OUTEN		(1 << 2)	/* End Point 2 OUT Interrupt Enable */

/* Bits in USB_UIE */
#define USB_UIE_SUSINTEN		(1 << 0)	/* SUSPEND Interrupt Enable (also RESUME) */
#define USB_UIE_URINTEN			(1 << 2)	/* USB RESET Interrupt Enable */

/* Values for USB_INDEX */
#define USB_INDEX_EP0			0
#define USB_INDEX_EP1			1
#define USB_INDEX_EP2			2
#define USB_INDEX_EP3			3

/* Values for USB_MAXP */
#define USB_MAXP_MAXP_8			1
#define USB_MAXP_MAXP_16		2
#define USB_MAXP_MAXP_24		3
#define USB_MAXP_MAXP_32		4
#define USB_MAXP_MAXP_40		5
#define USB_MAXP_MAXP_48		6
#define USB_MAXP_MAXP_56		7
#define USB_MAXP_MAXP_64		8

/* Bits in USB_INCSR1 */
#define USB_INCSR1_IN_PKT_RDY	(1 << 0)	/* IN Packet Ready */
#define USB_INCSR1_FIFO_NE		(1 << 1)	/* FIFO Not Empty */
#define USB_INCSR1_FIFO_FLUSH	(1 << 3)	/* FIFO Flush Request */
#define USB_INCSR1_SEND_STALL	(1 << 4)	/* Send Stall Handshake to USB */
#define USB_INCSR1_SENT_STALL	(1 << 5)	/* STALL Send Acknowledge */
#define USB_INCSR1_CLRTOG		(1 << 6)	/* Clear Data Toggle */

/* Bits in USB_INCSR2 */
#define USB_INCSR2_USB_DMA_EN	(1 << 4)	/* USB DMA Enable */
#define USB_INCSR2_AUTO_SET		(1 << 7)	/* Auto Set IN_PKT_RDY Bit */

/* Bits in USB_OUTCSR1 */
#define USB_OUTCSR1_OUT_PKT_RDY	(1 << 0)	/* OUT Packet Ready */
#define USB_OUTCSR1_FIFO_FULL	(1 << 1)	/* FIFO Full */
#define USB_OUTCSR1_FIFO_FLUSH	(1 << 4)	/* Flush OUT FIFO */
#define USB_OUTCSR1_SEND_STALL	(1 << 5)	/* Send Stall Handshake */
#define USB_OUTCSR1_SENT_STALL	(1 << 6)	/* STALL Handshake Sent */
#define USB_OUTCSR1_CL_DATATOG	(1 << 7)	/* Clear Data Toggle Sequence Bit */

/* Bits in USB_OUTCSR2 */
#define USB_OUTCSR2_USB_DMA_EN	(1 << 4)	/* USB DMA Enable */
#define USB_OUTCSR2_AUTO_CLR	(1 << 7)	/* Auto Clear OUT_PKT_RDY Bit */

/* Bits in EP0CSR (== USB_INCSR1 with USB_INDEX = 0) */
#define USB_EP0CSR_OUT_PKT_RDY		(1 << 0)	/* OUT Packet Ready (RO) */
#define USB_EP0CSR_IN_PKT_RDY		(1 << 1)	/* IN Packet Ready */
#define USB_EP0CSR_SENT_STALL		(1 << 2)	/* Sent STALL Handshake */
#define USB_EP0CSR_DATA_END			(1 << 3)	/* Data End */
#define USB_EP0CSR_SETUP_END		(1 << 4)	/* Setup Ends (RO) */
#define USB_EP0CSR_SEND_STALL		(1 << 5)	/* Send Stall Handshake to USB */
#define USB_EP0CSR_CLR_OUT			(1 << 6)	/* Clear OUT Packet Ready Bit */
#define USB_EP0CSR_CLR_SETUP_END	(1 << 7)	/* Clear Setup End Bit */


/***********************/
/* USB Host Controller */
/***********************/

#define USBH_BASE		LH7A404_USBH_BASE

#define USBH_CMDSTATUS	__REG32(USBH_BASE + 0x08)



/*******************/
/* AC97 Controller */
/*******************/

#define AC97_BASE		LH7A404_AC97_BASE

#define AC97_DR1		__REG32(AC97_BASE + 0x00)	/* FIFO1 Data Register */
#define AC97_RXCR1		__REG32(AC97_BASE + 0x04)	/* FIFO1 Receive Control Register */
#define AC97_TXCR1		__REG32(AC97_BASE + 0x08)	/* FIFO1 Transmit Control Register */
#define AC97_SR1		__REG32(AC97_BASE + 0x0C)	/* FIFO1 Status Register */
#define AC97_RISR1		__REG32(AC97_BASE + 0x10)	/* FIFO1 Raw Interrupt Status Register */
#define AC97_ISR1		__REG32(AC97_BASE + 0x14)	/* FIFO1 Interrupt Status Register */
#define AC97_IE1		__REG32(AC97_BASE + 0x18)	/* FIFO1 Interrupt Enable Register */

#define AC97_DR2		__REG32(AC97_BASE + 0x20)	/* FIFO2 Data Register */
#define AC97_RXCR2		__REG32(AC97_BASE + 0x24)	/* FIFO2 Receive Control Register */
#define AC97_TXCR2		__REG32(AC97_BASE + 0x28)	/* FIFO2 Transmit Control Register */
#define AC97_SR2		__REG32(AC97_BASE + 0x2C)	/* FIFO2 Status Register */
#define AC97_RISR2		__REG32(AC97_BASE + 0x30)	/* FIFO2 Raw Interrupt Status Register */
#define AC97_ISR2		__REG32(AC97_BASE + 0x34)	/* FIFO2 Interrupt Status Register */
#define AC97_IE2		__REG32(AC97_BASE + 0x38)	/* FIFO2 Interrupt Enable Register */

#define AC97_DR3		__REG32(AC97_BASE + 0x40)	/* FIFO3 Data Register */
#define AC97_RXCR3		__REG32(AC97_BASE + 0x44)	/* FIFO3 Receive Control Register */
#define AC97_TXCR3		__REG32(AC97_BASE + 0x48)	/* FIFO3 Transmit Control Register */
#define AC97_SR3		__REG32(AC97_BASE + 0x4C)	/* FIFO3 Status Register */
#define AC97_RISR3		__REG32(AC97_BASE + 0x50)	/* FIFO3 Raw Interrupt Status Register */
#define AC97_ISR3		__REG32(AC97_BASE + 0x54)	/* FIFO3 Interrupt Status Register */
#define AC97_IE3		__REG32(AC97_BASE + 0x58)	/* FIFO3 Interrupt Enable Register */

#define AC97_DR4		__REG32(AC97_BASE + 0x60)	/* FIFO4 Data Register */
#define AC97_RXCR4		__REG32(AC97_BASE + 0x64)	/* FIFO4 Receive Control Register */
#define AC97_TXCR4		__REG32(AC97_BASE + 0x68)	/* FIFO4 Transmit Control Register */
#define AC97_SR4		__REG32(AC97_BASE + 0x6C)	/* FIFO4 Status Register */
#define AC97_RISR4		__REG32(AC97_BASE + 0x70)	/* FIFO4 Raw Interrupt Status Register */
#define AC97_ISR4		__REG32(AC97_BASE + 0x74)	/* FIFO4 Interrupt Status Register */
#define AC97_IE4		__REG32(AC97_BASE + 0x78)	/* FIFO4 Interrupt Enable Register */

#define AC97_S1DATA		__REG32(AC97_BASE + 0x80)	/* Data Register on Slot 1 */
#define AC97_S2DATA		__REG32(AC97_BASE + 0x84)	/* Data Register on Slot 2 */
#define AC97_S12DATA	__REG32(AC97_BASE + 0x88)	/* Data Register on Slot 12 */
#define AC97_RGIS		__REG32(AC97_BASE + 0x8C)	/* Raw Global Interrupt Status */
#define AC97_GIS		__REG32(AC97_BASE + 0x90)	/* Global Interrupt Status */
#define AC97_GIEN		__REG32(AC97_BASE + 0x94)	/* Global Interrupt Enable */
#define AC97_GEOI		__REG32(AC97_BASE + 0x98)	/* Global Interrupt Clear */
#define AC97_GCR		__REG32(AC97_BASE + 0x9C)	/* Global Control Register */
#define AC97_RESET		__REG32(AC97_BASE + 0xA0)	/* Reset Control Register */
#define AC97_SYNC		__REG32(AC97_BASE + 0xA4)	/* Sync Control Register */
#define AC97_GCIS		__REG32(AC97_BASE + 0xA8)	/* Global Control FIFO Interrupt Status */


/*******************************/
/* Audio Codec Interface (ACI) */
/*******************************/

#define ACI_BASE		LH7A404_ACI_BASE

#define ACI_DATA		__REG32(ACI_BASE + 0x00)	/* Data Register */
#define ACI_CTL			__REG32(ACI_BASE + 0x04)	/* Control Register */
#define ACI_STATUS		__REG32(ACI_BASE + 0x08)	/* Status Register */
#define ACI_EOI			__REG32(ACI_BASE + 0x0C)	/* End-Of-Interrupt Register */
#define ACI_CLKDIV		__REG32(ACI_BASE + 0x10)	/* Clock Divider Register */

#define ACI_DATA_MASK	0xFF	/* valid bits in Data Register */

/* Bits in Control Register */
#define ACI_CTL_TXEN		(1 << 0)	/* Transmit Enable */
#define ACI_CTL_RXEN		(1 << 1)	/* Receive Enable */
#define ACI_CTL_RXIE		(1 << 2)	/* Receive Interrupt Enable */
#define ACI_CTL_TXIE		(1 << 3)	/* Transmit Interrupt Enable */
#define ACI_CTL_LB			(1 << 4)	/* Loopback */
#define ACI_CTL_TXEPCLKEN	(1 << 5)	/* Transmit FIFO Empty Stop Clock Enable */

/* Bits in Status Register */
#define ACI_STATUS_RXFE		(1 << 0)	/* Receive FIFO Empty */
#define ACI_STATUS_TXFF		(1 << 1)	/* Transmit FIFO Full */
#define ACI_STATUS_RXFF		(1 << 2)	/* Receive FIFO Full */
#define ACI_STATUS_TXFE		(1 << 3)	/* Transmit FIFO Empty */
#define ACI_STATUS_RXI		(1 << 4)	/* Receive Interrupt */
#define ACI_STATUS_TXI		(1 << 5)	/* Transmit Interrupt */
#define ACI_STATUS_RXBUSY	(1 << 6)	/* Receive Busy */
#define ACI_STATUS_TXBUSY	(1 << 7)	/* Transmit Busy */


/*******************************/
/* Pulse Width Modulator (PWM) */
/*******************************/

#define PWM_BASE		LH7A404_PWM_BASE

#define PWM_TC2			__REG32(PWM_BASE + 0x00)	/* PWM2 Terminal Count Register */
#define PWM_DC2			__REG32(PWM_BASE + 0x04)	/* PWM2 Duty Cycle Register */
#define PWM_EN2			__REG32(PWM_BASE + 0x08)	/* PWM2 Enable Register */
#define PWM_INV2		__REG32(PWM_BASE + 0x0C)	/* PWM2 Invert Register */
#define PWM_SYNC2		__REG32(PWM_BASE + 0x10)	/* PWM2 Synchronous Register */
#define PWM_TC3			__REG32(PWM_BASE + 0x20)	/* PWM3 Terminal Count Register */
#define PWM_DC3			__REG32(PWM_BASE + 0x24)	/* PWM3 Duty Cycle Register */
#define PWM_EN3			__REG32(PWM_BASE + 0x28)	/* PWM3 Enable Register */
#define PWM_INV3		__REG32(PWM_BASE + 0x2C)	/* PWM3 Invert Register */

/* Bits in PWM_ENx Register */
#define PWM_EN_ENABLE		(1 << 0)	/* PWM Enable */

/* Bits in PWM_INVx Register */
#define PWM_INV_INV			(1 << 0)	/* Invert PWM Output */

/* Bits in PWM_SYNC2 Register */
#define PWM_SYNC2_MODCE		(1 << 0)	/* PWM Mode Select */
#define PWM_SYNC2_SOURCE	(1 << 1)	/* PWM Sync Signal Source */



/*************************************/
/* Analog-to-Digital Converter (ADC) */
/*************************************/

#define ADC_BASE		LH7A404_ADC_BASE

#define ADC_HW			__REG32(ADC_BASE + 0x00)	/* High Word Register */
#define ADC_LW			__REG32(ADC_BASE + 0x04)	/* Low Word Register */
#define ADC_RR			__REG32(ADC_BASE + 0x08)	/* Results Register */
#define ADC_IM			__REG32(ADC_BASE + 0x0C)	/* Interrupt Mask Register */
#define ADC_PC			__REG32(ADC_BASE + 0x10)	/* Power Configuration Register */
#define ADC_GC			__REG32(ADC_BASE + 0x14)	/* General Configuration Register */
#define ADC_GS			__REG32(ADC_BASE + 0x18)	/* General Status Register */
#define ADC_IS			__REG32(ADC_BASE + 0x1C)	/* Raw Interrupt Status Register */
#define ADC_FS			__REG32(ADC_BASE + 0x20)	/* FIFO Status Register */
#define ADC_HWCB0		__REG32(ADC_BASE + 0x24)	/* High Word Control Bank Register 0 */
#define ADC_HWCB1		__REG32(ADC_BASE + 0x28)	/* High Word Control Bank Register 1 */
#define ADC_HWCB2		__REG32(ADC_BASE + 0x2C)	/* High Word Control Bank Register 2 */
#define ADC_HWCB3		__REG32(ADC_BASE + 0x30)	/* High Word Control Bank Register 3 */
#define ADC_HWCB4		__REG32(ADC_BASE + 0x34)	/* High Word Control Bank Register 4 */
#define ADC_HWCB5		__REG32(ADC_BASE + 0x38)	/* High Word Control Bank Register 5 */
#define ADC_HWCB6		__REG32(ADC_BASE + 0x3C)	/* High Word Control Bank Register 6 */
#define ADC_HWCB7		__REG32(ADC_BASE + 0x40)	/* High Word Control Bank Register 7 */
#define ADC_HWCB8		__REG32(ADC_BASE + 0x44)	/* High Word Control Bank Register 8 */
#define ADC_HWCB9		__REG32(ADC_BASE + 0x48)	/* High Word Control Bank Register 9 */
#define ADC_HWCB10		__REG32(ADC_BASE + 0x4C)	/* High Word Control Bank Register 10 */
#define ADC_HWCB11		__REG32(ADC_BASE + 0x50)	/* High Word Control Bank Register 11 */
#define ADC_HWCB12		__REG32(ADC_BASE + 0x54)	/* High Word Control Bank Register 12 */
#define ADC_HWCB13		__REG32(ADC_BASE + 0x58)	/* High Word Control Bank Register 13 */
#define ADC_HWCB14		__REG32(ADC_BASE + 0x5C)	/* High Word Control Bank Register 14 */
#define ADC_HWCB15		__REG32(ADC_BASE + 0x60)	/* High Word Control Bank Register 15 */
#define ADC_LWCB0		__REG32(ADC_BASE + 0x64)	/* Low Word Control Bank Register 0 */
#define ADC_LWCB1		__REG32(ADC_BASE + 0x68)	/* Low Word Control Bank Register 1 */
#define ADC_LWCB2		__REG32(ADC_BASE + 0x6C)	/* Low Word Control Bank Register 2 */
#define ADC_LWCB3		__REG32(ADC_BASE + 0x70)	/* Low Word Control Bank Register 3 */
#define ADC_LWCB4		__REG32(ADC_BASE + 0x74)	/* Low Word Control Bank Register 4 */
#define ADC_LWCB5		__REG32(ADC_BASE + 0x78)	/* Low Word Control Bank Register 5 */
#define ADC_LWCB6		__REG32(ADC_BASE + 0x7C)	/* Low Word Control Bank Register 6 */
#define ADC_LWCB7		__REG32(ADC_BASE + 0x80)	/* Low Word Control Bank Register 7 */
#define ADC_LWCB8		__REG32(ADC_BASE + 0x84)	/* Low Word Control Bank Register 8 */
#define ADC_LWCB9		__REG32(ADC_BASE + 0x88)	/* Low Word Control Bank Register 9 */
#define ADC_LWCB10		__REG32(ADC_BASE + 0x8C)	/* Low Word Control Bank Register 10 */
#define ADC_LWCB11		__REG32(ADC_BASE + 0x90)	/* Low Word Control Bank Register 11 */
#define ADC_LWCB12		__REG32(ADC_BASE + 0x94)	/* Low Word Control Bank Register 12 */
#define ADC_LWCB13		__REG32(ADC_BASE + 0x98)	/* Low Word Control Bank Register 13 */
#define ADC_LWCB14		__REG32(ADC_BASE + 0x9C)	/* Low Word Control Bank Register 14 */
#define ADC_LWCB15		__REG32(ADC_BASE + 0xA0)	/* Low Word Control Bank Register 15 */
#define ADC_IHWCTRL		__REG32(ADC_BASE + 0xA4)	/* Idle High Word Register */
#define ADC_ILWCTRL		__REG32(ADC_BASE + 0xA8)	/* Idle Low Word Register */
#define ADC_MIS			__REG32(ADC_BASE + 0xAC)	/* Masked Interrupt Status Register */
#define ADC_IC			__REG32(ADC_BASE + 0xB0)	/* Interrupt Clear Register */



/**************************************/
/* Keyboard and Mouse Interface (KMI) */
/**************************************/

#define KMI_BASE		LH7A404_KMI_BASE

#define KMI_CR			__REG32(KMI_BASE + 0x00)	/* KMI Control Register */
#define KMI_STAT		__REG32(KMI_BASE + 0x04)	/* KMI Status Register */
#define KMI_DATA		__REG32(KMI_BASE + 0x08)	/* KMI Data Register */
#define KMI_CLKDIV		__REG32(KMI_BASE + 0x0C)	/* KMI Clock Divider Register */
#define KMI_ISR			__REG32(KMI_BASE + 0x10)	/* KMI Interrupt Status Register */

/* Bits in KMI Control Register */
#define KMI_CR_FCL		(1 << 0)		/* Force KMI Clock LOW */
#define KMI_CR_FDL		(1 << 1)		/* Force KMI Data Line LOW */
#define KMI_CR_KMIEN	(1 << 2)		/* KMI Enable */
#define KMI_CR_TIE		(1 << 3)		/* Transmit Interrupt Enable */
#define KMI_CR_RIE		(1 << 4)		/* Receive Interrupt Enable */
#define KMI_CR_TYPE		(1 << 5)		/* Keyboard Type: 0 = PS2/AT (with line control bit) */

/* Bits in KMI Status Register */
#define KMI_STAT_DSTAT		(1 << 0)	/* Data Line Status */
#define KMI_STAT_CLKSTAT	(1 << 1)	/* Clock Line Status */
#define KMI_STAT_RXPARITY	(1 << 2)	/* Receive Parity */
#define KMI_STAT_RXBUSY		(1 << 3)	/* Receive Busy */
#define KMI_STAT_RXFULL		(1 << 4)	/* Receive Register Full */
#define KMI_STAT_TXBUSY		(1 << 5)	/* Transmit Busy */
#define KMI_STAT_TXEMPTY	(1 << 6)	/* Transmit Register Empty */

/* Bits in KMI Data Register */
#define KMI_DATA_MASK		(0xFF << 0)	/* KMI Data */

/* Bits in KMI Clock Divider Register */
#define KMI_CLKDIV_MASK		(0x0F << 0)	/* KMI Clock Divisor */

/* Bits in KMI Interrupt Status Register */
#define KMI_ISR_RXI			(1 << 0)	/* Receive interrupt */
#define KMI_ISR_TXI			(1 << 1)	/* Transmit interrupt */


#endif /* __LH7A404_H__ */
