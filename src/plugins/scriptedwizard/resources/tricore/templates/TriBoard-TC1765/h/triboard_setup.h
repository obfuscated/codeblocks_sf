/*====================================================================
* Project:  Board Support Package (BSP)
* Developed using:
* Function: TriBoard-TC1765 platform specific setup values
*
* Copyright HighTec EDV-Systeme GmbH 1982-2007
*====================================================================*/

#ifndef __TRIBOARD_SETUP_H__
#define __TRIBOARD_SETUP_H__

#include "tc1765/scu/addr.h"
#include "tc1765/ebu/addr.h"
#include "tc1765/pmu/addr.h"

/* initialisation value for PLLCLC : set clock to 40 MHz */
#define VAL_PLLCLC			0x00022300


/* initialization values for External Bus */
#define VAL_EBU_CON			0x0000FF68
#define VAL_EBU_ADDSEL0		0xA4000051
#define VAL_EBU_BUSCON0		0x00600A21
#define VAL_EBU_ADDSEL1		0xA0000071
#define VAL_EBU_BUSCON1		0x00020000
#define VAL_EBU_ADDSEL2		0x00000001
#define VAL_EBU_BUSCON2		0x80415FFF
#define VAL_EBU_ADDSEL3		0xE0000001
#define VAL_EBU_BUSCON3		0x00020000

#ifdef ENABLE_ICACHE
#define VAL_PMU_CON			0x04000704
#define VAL_PMU_EIFCON		0x00000040
#else
#define VAL_PMU_CON			0x04003F06
#define VAL_PMU_EIFCON		0x0000005F
#endif /* ENABLE_ICACHE */


#endif /* __TRIBOARD_SETUP_H__ */
