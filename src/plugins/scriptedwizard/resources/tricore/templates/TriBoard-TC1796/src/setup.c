/*====================================================================
* Project:  Board Support Package (BSP)
* Developed using:
* Function: board initialization table (TriBoard-TC1796)
*
* Copyright HighTec EDV-Systeme GmbH 1982-2007
*====================================================================*/

#if defined(TRIBOARD_TC1796)
#include "triboard_setup.h"
#else
#error ERROR: NO TARGET DEFINED!
#endif /* TRIBOARD_TC1796 */


typedef struct
{
	unsigned long addr;
	unsigned long val;
} InitTab_t;

const InitTab_t boardSetupTab[] =
{
	{PLL_CLC_ADDR,		VAL_PLLCLC		},
	{EBU_ADDRSEL0_ADDR,	VAL_EBU_ADDRSEL0},
	{EBU_BUSCON0_ADDR,	VAL_EBU_BUSCON0	},
	{EBU_BUSAP0_ADDR,	VAL_EBU_BUSAP0	},
	{EBU_ADDRSEL1_ADDR,	VAL_EBU_ADDRSEL1},
	{EBU_BUSCON1_ADDR,	VAL_EBU_BUSCON1	},
	{EBU_BUSAP1_ADDR,	VAL_EBU_BUSAP1	},
	/*
		This must be done after CSx programming to avoid disabling EBU_CON.CS0FAM
		(CS0 Fills Address Map)
	 */
	{EBU_CON_ADDR,		VAL_EBU_CON		},
	{EBU_BFCON_ADDR,	VAL_EBU_BFCON	},
	{PMI_CON0_ADDR,		VAL_PMI_CON0	}
};

const unsigned long boardSetupTabSize = sizeof(boardSetupTab) / sizeof(InitTab_t);
