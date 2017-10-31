/*====================================================================
* Project:  Board Support Package (BSP)
* Developed using:
* Function: phyCORE-TC1775 platform specific setup values
*
* Copyright HighTec EDV-Systeme GmbH 1982-2007
*====================================================================*/

#ifndef __PHYCORE_SETUP_H__
#define __PHYCORE_SETUP_H__

#include "tc1775b/scu/addr.h"
#include "tc1775b/ebu/addr.h"
#include "tc1775b/pmu/addr.h"

/* initialization value for PLLCLC : set clock to 40 MHz */
#define VAL_PLLCLC			0x00022300


/* initialization values for External Bus */
#define VAL_EBU_CON			0x0000FF68
#define VAL_EBU_ADDSEL0		0xA4000061
#define VAL_EBU_BUSCON0		0x00420A7C
#define VAL_EBU_ADDSEL1		0xA0000051
#define VAL_EBU_BUSCON1		0x00020244
#define VAL_EBU_ADDSEL2		0x00000001
#define VAL_EBU_BUSCON2		0x80415FFF

/* CS3: ethernet controller CS8900 */
#define VAL_EBU_ADDSEL3		0xB0000061
#define VAL_EBU_BUSCON3		0x00415A7F

#ifdef ENABLE_ICACHE
#define VAL_PMU_CON			0x04000704
#define VAL_PMU_EIFCON		0x00000040
#else
#define VAL_PMU_CON			0x04003F06
#define VAL_PMU_EIFCON		0x0000005F
#endif /* ENABLE_ICACHE */


#endif /* __PHYCORE_SETUP_H__ */
