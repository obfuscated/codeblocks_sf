/*====================================================================
* Project: Board Support Package (BSP)
* Developed using:
* Function: Intel XScale Memory Management initialization
*
* Copyright HighTec EDV-Systeme GmbH 1982-2006
*====================================================================*/

#include <string.h>

#include "pxa255regs.h"
#include "pxa_mm.h"

#ifdef USE_DCACHE_WRITEBUFFER
#define DATA_BUFFERABLE		ARM_BUFFERABLE
#else
#define DATA_BUFFERABLE		ARM_UNBUFFERABLE
#endif /* USE_DCACHE_WRITEBUFFER */

void pxa_mmu_init(void)
{
	unsigned int domain   = ARM_ACCESS_DACR_DEFAULT;
	unsigned int ttb_base = PXA2X0_RAM_BANK0_BASE + 0x4000;

	/* set Domain Access Control Register */
	__asm__ volatile ("mcr  p15,0,%0,c3,c0,0" : : "r"(domain));

	/* set TTB Register */
	__asm__ volatile ("mcr  p15,0,%0,c2,c0,0" : : "r"(ttb_base));

	/* erase Page Table */
	memset((void *)ttb_base, 0, ARM_FIRST_LEVEL_PAGE_TABLE_SIZE);

	/* create Page Table */

	/*				  Actual		Virtual			Size	Attribute													Function	*/
	/*				  Base			Base			MB		cached?			 buffered?		   access permissions					*/
	/*				  xxx00000		xxx00000																						*/
	X_ARM_MMU_SECTION(0x000,		0x500,			64,		ARM_CACHEABLE,   ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* Boot flash ROMspace */
	X_ARM_MMU_SECTION(0x040,		0x540,			64,		ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* FPGA Register */
	X_ARM_MMU_SECTION(0x0C0,		0xf20,			1,		ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* CPLD Register */
	X_ARM_MMU_SECTION(0x100,		0xf10,			1,		ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* USB ISP-1362 */
	X_ARM_MMU_SECTION(0x140,		0xf00,			1,		ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* LAN Register */
	X_ARM_MMU_SECTION(0x400,		0x400,			192,	ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* PXA2x0 Register */

	X_ARM_MMU_SECTION(0xA00,		0x0,			64,		ARM_CACHEABLE,   DATA_BUFFERABLE,  ARM_ACCESS_PERM_RW_RW); /* SDRAM Bank 0 */
	X_ARM_MMU_SECTION(0xA40,		0x040,			64,		ARM_CACHEABLE,   DATA_BUFFERABLE,  ARM_ACCESS_PERM_RW_RW); /* SDRAM Bank 1 */
	X_ARM_MMU_SECTION(0xA80,		0x080,			64,		ARM_CACHEABLE,   DATA_BUFFERABLE,  ARM_ACCESS_PERM_RW_RW); /* SDRAM Bank 2 */
	X_ARM_MMU_SECTION(0xAc0,		0x0c0,			64,		ARM_CACHEABLE,   DATA_BUFFERABLE,  ARM_ACCESS_PERM_RW_RW); /* SDRAM Bank 3 */

	X_ARM_MMU_SECTION(0xA00,		0xA00,			256,	ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* Unmapped Memory */

	X_ARM_MMU_SECTION(0xC00,		0xC00,			128,	ARM_CACHEABLE,   ARM_BUFFERABLE,   ARM_ACCESS_PERM_RW_RW);

}
