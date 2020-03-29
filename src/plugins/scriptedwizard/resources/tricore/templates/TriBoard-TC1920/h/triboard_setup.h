/*====================================================================
* Project:  Board Support Package (BSP)
* Developed using:
* Function: TriBoard-TC1920 platform specific setup values
*
* Copyright HighTec EDV-Systeme GmbH 1982-2008
*====================================================================*/

#ifndef __TRIBOARD_SETUP_H__
#define __TRIBOARD_SETUP_H__

#include "tc1920/scu/addr.h"
#include "tc1920/ebu/addr.h"
#include "tc1920/pmu/addr.h"

/* initialization value for PLLCLC : set clock to 96 MHz */
#define VAL_PLLCLC			0x010F0144


/* initialization values for External Bus */
#define VAL_EBU_CON			0x00F9FF68
#define VAL_EBU_BFCON		0x00000053
#define VAL_EBU_ADDRSEL0	0xA4000051
#define VAL_EBU_BUSCON0		0x00860000
#define VAL_EBU_BUSAP0		0x23FF0100
#define VAL_EBU_ADDRSEL1	0xA0000031
#define VAL_EBU_BUSCON1		0x30B20000
#define VAL_EBU_BUSAP1		0x00000000
#define VAL_EBU_ADDRSEL2	0xA2000021
#define VAL_EBU_BUSCON2		0x30B20000
#define VAL_EBU_BUSAP2		0x00000000

#define VAL_EBU_SDRMCON0	0x01161075
#define VAL_EBU_SDRMMOD0	0x00000023
#define VAL_EBU_SDRMREF0	0x000000CB
#define VAL_EBU_SDRMCON1	0x01162075
#define VAL_EBU_SDRMMOD1	0x00000023
#define VAL_EBU_SDRMREF1	0x000000CB

#define VAL_EBU_ADDRSEL3	0x00000001
#define VAL_EBU_BUSCON3		0x80415FFF
#define VAL_EBU_BUSAP3		0x22070000

#ifdef ENABLE_ICACHE
/* enable instruction cache */
#define VAL_PMU_CON0		0x00000000
#else
/* disable instruction cache */
#define VAL_PMU_CON0		0x00000002
#endif /* ENABLE_ICACHE */


#endif /* __TRIBOARD_SETUP_H__ */
