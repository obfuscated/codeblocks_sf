/*====================================================================
* Project:  Board Support Package (BSP)
* Developed using:
* Function: phyCORE-TC1130 platform specific setup values
*
* Copyright HighTec EDV-Systeme GmbH 1982-2008
*====================================================================*/

#ifndef __PHYCORE_SETUP_H__
#define __PHYCORE_SETUP_H__

#include "tc1130b/scu/addr.h"
#include "tc1130b/ebu/addr.h"
#include "tc1130b/pmi/addr.h"

/* initialization value for PLLCLC : set clock to 150 MHz */
#define VAL_PLLCLC			0x001D0382


/* initialization values for External Bus */
#define VAL_EBU_CON			0x01F9FF68
#define VAL_EBU_BFCON		0x001001D0
#define VAL_EBU_ADDRSEL0	0xA4000853
#define VAL_EBU_BUSCON0		0x00922300
#define VAL_EBU_BUSAP0		0xC7230100
#define VAL_EBU_ADDRSEL1	0xA0000813
#define VAL_EBU_BUSCON1		0x30B20000
#define VAL_EBU_BUSAP1		0x42000000
#define VAL_EBU_ADDRSEL2	0xA8000011
#define VAL_EBU_BUSCON2		0x30B20000
#define VAL_EBU_BUSAP2		0x42000000

#define VAL_EBU_SDRMCON0	0x219E2075
#define VAL_EBU_SDRMMOD0	0x00000023
#define VAL_EBU_SDRMREF0	0x000000C9
#define VAL_EBU_SDRMCON1	0x00000000
#define VAL_EBU_SDRMMOD1	0x00000020
#define VAL_EBU_SDRMREF1	0x00000000

#define VAL_EBU_ADDRSEL3	0x00000000
#define VAL_EBU_BUSCON3		0x8092807F
#define VAL_EBU_BUSAP3		0xFFFFFFFF

/* enable instruction cache */
#define VAL_PMI_CON0		0x00000000


#endif /* __PHYCORE_SETUP_H__ */
