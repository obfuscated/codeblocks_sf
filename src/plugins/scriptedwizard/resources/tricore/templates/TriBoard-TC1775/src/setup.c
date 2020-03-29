/*====================================================================
* Project:  Board Support Package (BSP)
* Developed using:
* Function: board initialization table (TriBoard-TC1775)
*
* Copyright HighTec EDV-Systeme GmbH 1982-2007
*====================================================================*/

#ifdef TRIBOARD_TC1775
#include "triboard_setup.h"
#else
#error ERROR: NO TARGET DEFINED!
#endif /* TRIBOARD_TC1775 */


typedef struct
{
	unsigned long addr;
	unsigned long val;
} InitTab_t;

const InitTab_t boardSetupTab[] =
{
	{PLL_CLC_ADDR,		VAL_PLLCLC		},
	{EBU_ADDSEL0_ADDR,	VAL_EBU_ADDSEL0	},
	{EBU_BUSCON0_ADDR,	VAL_EBU_BUSCON0	},
	{EBU_ADDSEL1_ADDR,	VAL_EBU_ADDSEL1	},
	{EBU_BUSCON1_ADDR,	VAL_EBU_BUSCON1	},
	{EBU_ADDSEL2_ADDR,	VAL_EBU_ADDSEL2	},
	{EBU_BUSCON2_ADDR,	VAL_EBU_BUSCON2	},
	{EBU_ADDSEL3_ADDR,	VAL_EBU_ADDSEL3	},
	{EBU_BUSCON3_ADDR,	VAL_EBU_BUSCON3	},
	/*
		This must be done after CSx programming to avoid disabling EBU_CON.CS0FAM
		(CS0 Fills Address Map)
	 */
	{EBU_CON_ADDR,		VAL_EBU_CON		},
	{PMU_EIFCON_ADDR,	VAL_PMU_EIFCON	},
	{PMU_CON_ADDR,		VAL_PMU_CON		}
};

const unsigned long boardSetupTabSize = sizeof(boardSetupTab) / sizeof(InitTab_t);

