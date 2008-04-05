/*====================================================================
* Project: Board Support Package (BSP)
* Developed using:
* Function: LPD-LH7A404 Memory Management initialization
*
* Copyright HighTec EDV-Systeme GmbH 1982-2006
*====================================================================*/

#include <string.h>

#include "lh7a404.h"
#include "arm9_mm.h"

#ifdef USE_DCACHE_WRITEBUFFER
#define DATA_BUFFERABLE		ARM_BUFFERABLE
#else
#define DATA_BUFFERABLE		ARM_UNBUFFERABLE
#endif /* USE_DCACHE_WRITEBUFFER */

void hal_mmu_init(void)
{
	unsigned int domain   = ARM_ACCESS_DACR_DEFAULT;
	unsigned int ttb_base = LH7A404_SDCSC0_BASE + 0x4000;

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
	X_ARM_MMU_SECTION(0x000,		0x100,			64,		ARM_CACHEABLE,   ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* Boot flash ROMspace */
	X_ARM_MMU_SECTION(0x400,		0x400,			256,	ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* PCMCIA Slot 1 */
	X_ARM_MMU_SECTION(0x500,		0x500,			256,	ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* PCMCIA Slot 2 */
	X_ARM_MMU_SECTION(0x600,		0x600,			256,	ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* External I/O (slow) */
	X_ARM_MMU_SECTION(0x700,		0x700,			256,	ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* CPLD Register */
	X_ARM_MMU_SECTION(0x800,		0x800,			1,		ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* LH7A404 SFRs (APB + AHB) */
	X_ARM_MMU_SECTION(0xB00,		0xB00,			1,		ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* Internal SRAM */

	X_ARM_MMU_SECTION(0xC00,		0x0,			64,		ARM_CACHEABLE,   DATA_BUFFERABLE,  ARM_ACCESS_PERM_RW_RW); /* SDRAM Bank 0 */

	X_ARM_MMU_SECTION(0xC00,		0xC00,			256,	ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* Unmapped Memory */

}
