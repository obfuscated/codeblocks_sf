/*====================================================================
* Project:  Board Support Package (BSP)
* Developed using:
* Function: HighTec EasyRun-TC1796 platform specific setup values
*
* Copyright HighTec EDV-Systeme GmbH 1982-2008
*====================================================================*/

#ifndef __EASYRUN_SETUP_H__
#define __EASYRUN_SETUP_H__

#include "tc1796b/scu/addr.h"
#include "tc1796b/ebu/addr.h"
#include "tc1796b/pmi/addr.h"

/* initialization value for PLLCLC : set clock to 150 MHz */
/* see PLL_TC.H005 TC1796BC-Errata-V1.7: NDIV >= 33 !! */
#define VAL_PLLCLC			0x003B2382


/* initialization values for External Bus */
#define VAL_EBU_CON			0x0000FF68
#define VAL_EBU_BFCON		0x001001D0
/* CS0: 1 MB external MRAM */
#define VAL_EBU_ADDRSEL0	0xA1000873
#define VAL_EBU_BUSCON0		0x00920000
#define VAL_EBU_BUSAP0		0x81200010
/* CS1: 4 MB external Flash (not used) */
#define VAL_EBU_ADDRSEL1	0xA4000853
#define VAL_EBU_BUSCON1		0x00920000
#define VAL_EBU_BUSAP1		0xC7B30000
/* CS2 is used for USB controller */
#define VAL_EBU_ADDRSEL2	0xD90000C1
#define VAL_EBU_BUSCON2		0x00420000
#define VAL_EBU_BUSAP2		0x40D809F0
/* CS3 is used for LAN controller */
#define VAL_EBU_ADDRSEL3	0xD80000C1
#define VAL_EBU_BUSCON3		0x00820000
#define VAL_EBU_BUSAP3		0x40D80000

/* enable instruction cache */
#define VAL_PMI_CON0		0x00000000


#endif /* __EASYRUN_SETUP_H__ */
