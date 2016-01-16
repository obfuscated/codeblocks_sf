/*====================================================================
* Project: Board Support Package (BSP)
* Developed using:
* Function: Standard definitions for LogicPD board LPD-LH7A404
*
* Copyright HighTec EDV-Systeme GmbH 1982-2006
*====================================================================*/

#ifndef __LPD404_H__
#define __LPD404_H__

#include "lh7a404.h"


/*********************************/
/* LPD I/O Controller Space CPLD */
/*********************************/

#define CPLD00_BASE		0x70000000	/* Wired LAN Chip Select */
#define CPLD02_BASE		0x70200000
#define CPLD06_BASE		0x70600000
#define CPLD08_BASE		0x70800000
#define CPLD0A_BASE		0x70A00000
#define CPLD0C_BASE		0x70C00000
#define CPLD0E_BASE		0x70E00000
#define CPLD10_BASE		0x71000000
#define CPLD12_BASE		0x71200000
#define CPLD14_BASE		0x71400000
#define CPLD16_BASE		0x71600000
#define CPLD18_BASE		0x71800000
#define CPLD1A_BASE		0x71A00000

#define CPLD_CONTROL		__REG8(CPLD02_BASE)
#define CPLD_SPI_DATA		__REG8(CPLD06_BASE)
#define CPLD_SPI_CONTROL	__REG8(CPLD08_BASE)
#define CPLD_SPI_EEPROM		__REG8(CPLD0A_BASE)
#define CPLD_INTERRUPTS		__REG8(CPLD0C_BASE) /* IRQ mask/status */
#define CPLD_BOOT_MODE		__REG8(CPLD0E_BASE)
#define CPLD_FLASH			__REG8(CPLD10_BASE)
#define CPLD_POWER_MGMT		__REG8(CPLD12_BASE)
#define CPLD_REVISION		__REG8(CPLD14_BASE)
#define CPLD_GPIO_EXT		__REG8(CPLD16_BASE)
#define CPLD_GPIO_DATA		__REG8(CPLD18_BASE)
#define CPLD_GPIO_DIR		__REG8(CPLD1A_BASE)

/* Bits in CPLD_CONTROL register */
#define CPLD_CONTROL_NWLPE	(1 << 0)	/* wired LAN power enable signal [0 = enabled] */
#define CPLD_CONTROL_AWKP	(1 << 2)	/* Auto Wakeup enable signal (removed feature) */
#define CPLD_CONTROL_SWINT	(1 << 7)	/* Software setable interrupt source */

/* Bits in CPLD_INTERRUPTS register */
#define CPLD_INTERRUPTS_NWIRQ	(1 << 0)	/* wired LAN chip interrupt request */
#define CPLD_INTERRUPTS_WMSK	(1 << 2)	/* wired LAN chip interrupt mask */


#endif /* __LPD404_H__ */
