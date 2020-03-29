/*
 * memoryROM.x -- TriBoard-TC1775 flash memory configuration
 *
 * Copyright (C) 1982-2008 HighTec EDV-Systeme GmbH.
 *
 */


/* __TC12__ like __TC1775__ __TC1765__ with Core TC1.2 */
__TRICORE_DERIVATE_MEMORY_MAP__ = 0x12;
/* the external RAM description */
__EXT_CODE_RAM_BEGIN = 0xa4000000;
__EXT_CODE_RAM_SIZE = 1M ;
__EXT_DATA_RAM_BEGIN = 0xa0000000;
__EXT_DATA_RAM_SIZE = 1M;
__RAM_END = __EXT_DATA_RAM_BEGIN + __EXT_DATA_RAM_SIZE;
/* the internal ram description */
__INT_CODE_RAM_BEGIN = 0xc0000000;
__INT_CODE_RAM_SIZE = 32K;
__INT_DATA_RAM_BEGIN = 0xd0000000;
__INT_DATA_RAM_SIZE = 32K;
/* the pcp memory description */
__PCP_CODE_RAM_BEGIN = 0xf0020000;
__PCP_CODE_RAM_SIZE = 16K;
__PCP_DATA_RAM_BEGIN = 0xf0010000;
__PCP_DATA_RAM_SIZE = 4K;

/* External Bus Memory Configuration word */
__EBMCFG = 0x0000808C;

MEMORY
{
  ext_cram (rx!p): org = 0xa4000000, len = 1M
  ext_dram (w!xp): org = 0xa0000000, len = 1M
  int_cram (rx!p): org = 0xc0000000, len = 32K
  int_dram (w!xp): org = 0xd0000000, len = 32K
  pcp_data (wp!x): org = 0xf0010000, len = 4K
  pcp_text (rxp):  org = 0xf0020000, len = 16K
}

/* the symbol __TRICORE_DERIVATE_NAME__ will be defined in the crt0.S and is
 * tested here to confirm that this memory map and the startup file will
 * fit together
*/
_. = ASSERT ((__TRICORE_DERIVATE_MEMORY_MAP__ == __TRICORE_DERIVATE_NAME__), "Using wrong Memory Map. This Map is for TC1775/TC1765");
