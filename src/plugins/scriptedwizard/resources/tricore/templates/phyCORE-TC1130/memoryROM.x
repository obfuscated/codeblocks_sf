/*
 * memoryROM.x -- phyCORE-TC1130 flash memory configuration
 *
 * Copyright (C) 1982-2008 HighTec EDV-Systeme GmbH.
 *
 */
/* __TC1130__ __TC13__ with Core TC1.3 */
__TRICORE_DERIVATE_MEMORY_MAP__ = 0x1130;
/* the external RAM description */
__EXT_CODE_RAM_BEGIN = 0x84000000;
__EXT_CODE_RAM_SIZE = 4M;
__EXT_DATA_RAM_BEGIN = 0xa0000000;
__EXT_DATA_RAM_SIZE = 4M;
__RAM_END = __EXT_DATA_RAM_BEGIN + __EXT_DATA_RAM_SIZE;
/* the internal ram description */
__INT_CODE_RAM_BEGIN = 0xd4000000;
__INT_CODE_RAM_SIZE = 32K;
__INT_DATA_RAM_BEGIN = 0xd0000000;
__INT_DATA_RAM_SIZE = 28K;
/* the pcp memory description */
__PCP_CODE_RAM_BEGIN = 0;
__PCP_CODE_RAM_SIZE = 0;
__PCP_DATA_RAM_BEGIN = 0;
__PCP_DATA_RAM_SIZE = 0;

/* External Bus Memory Configuration word */
__EBMCFG = 0x00009031;

MEMORY
{
  ext_cram (rx!p): org = 0x84000000, len = 4M
  ext_dram (w!xp): org = 0xa0000000, len = 4M
  int_cram (rx!p): org = 0xd4000000, len = 32K
  int_dram (w!xp): org = 0xd0000000, len = 28K
  pcp_data (wp!x): org = 0, len = 0
  pcp_text (rxp):  org = 0, len = 0
}

/* the symbol __TRICORE_DERIVATE_NAME__ will be defined in the crt0.S and is
 * tested here to confirm that this memory map and the startup file will
 * fit together
*/
_. = ASSERT ((__TRICORE_DERIVATE_MEMORY_MAP__ == __TRICORE_DERIVATE_NAME__), "Using wrong Memory Map. This Map is for TC1130");
