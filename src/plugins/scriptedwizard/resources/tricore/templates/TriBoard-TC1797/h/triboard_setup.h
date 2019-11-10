/*====================================================================
* Project:  Board Support Package (BSP)
* Developed using:
* Function: TriBoard-TC1797 platform specific setup values
*
* Copyright HighTec EDV-Systeme GmbH 1982-2008
*====================================================================*/

#ifndef __TRIBOARD_TC1797_SETUP_H__
#define __TRIBOARD_TC1797_SETUP_H__

#include "tc1797/ebu/addr.h"
#include "tc1797/cpu/addr.h"


/* initialization values for External Bus */
#define VAL_EBU_MODCON		0x800000E1
/* CS0: 4 MB external Flash */
#define VAL_EBU_ADDRSEL0	0xA4000853
#define VAL_EBU_BUSRCON0	0x00D30040
#define VAL_EBU_BUSRAP0		0x011F0190
#define VAL_EBU_BUSWCON0	0x00D30000
#define VAL_EBU_BUSWAP0		0x011F0190
/* CS1: 1 MB external SRAM */
#define VAL_EBU_ADDRSEL1	0xA1000873
#define VAL_EBU_BUSRCON1	0x00D30040
#define VAL_EBU_BUSRAP1		0x011F0190
#define VAL_EBU_BUSWCON1	0x00D30000
#define VAL_EBU_BUSWAP1		0x011F0190

/* disable instruction cache */
#define VAL_PMI_CON0		0x00000002


#endif /* __TRIBOARD_TC1797_SETUP_H__ */
