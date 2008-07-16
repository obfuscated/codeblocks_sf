/* Default linker script, for normal executables */
OUTPUT_FORMAT("elf32-tricore")
OUTPUT_ARCH(tricore)
ENTRY(_start)

/* __TC1767__ __TC131__ with Core TC1.3.1 */
__TRICORE_DERIVATE_MEMORY_MAP__ = 0x1767;
/* the internal FLASH description */
__EXT_CODE_RAM_BEGIN = 0xa0000000;
__EXT_CODE_RAM_SIZE = 2M;
/* the internal ram description */
__INT_CODE_RAM_BEGIN = 0xd4000000;
__INT_CODE_RAM_SIZE = 24K;
__INT_DATA_RAM_BEGIN = 0xd0000000;
__INT_DATA_RAM_SIZE = 68K;
/* the pcp memory description */
__PCP_CODE_RAM_BEGIN = 0xf0060000;
__PCP_CODE_RAM_SIZE = 16K;
__PCP_DATA_RAM_BEGIN = 0xf0050000;
__PCP_DATA_RAM_SIZE = 8K;

/*
 * used to check for HEAP_SIZE
 */
__RAM_END = __INT_DATA_RAM_BEGIN + __INT_DATA_RAM_SIZE;

MEMORY
{
  ext_cram (rx!p): org = 0xa0000000, len = 2M
  int_cram (rx!p): org = 0xd4000000, len = 24K
  int_dram (w!xp): org = 0xd0002000, len = 60K
  pcp_data (wp!x): org = 0xf0050000, len = 8K
  pcp_text (rxp):  org = 0xf0060000, len = 16K
}
/*
 * Define the sizes of the user and system stacks.
 */
__ISTACK_SIZE = DEFINED (__ISTACK_SIZE) ? __ISTACK_SIZE : 256 ;
__USTACK_SIZE = DEFINED (__USTACK_SIZE) ? __USTACK_SIZE : 1K ;
/*
 * The heap is the memory between the top of the user stack and
 * __RAM_END (as defined above); programs can dynamically allocate
 * space in this area using malloc() and various other functions.
 * Below you can define the minimum amount of memory that the heap
 * should provide.
 */
__HEAP_MIN = DEFINED (__HEAP_MIN) ? __HEAP_MIN : 512 ;
/*
 * Define the start address and the size of the context save area.
 */
__CSA_BEGIN = DEFINED (__CSA_BEGIN) ? __CSA_BEGIN : 0xd0000000 ;
__CSA_SIZE = DEFINED (__CSA_SIZE) ? __CSA_SIZE : 8k ;
__CSA_END = __CSA_BEGIN + __CSA_SIZE ;

SECTIONS
{
  /*
   * The startup code should be placed where the CPU expects it after a reset,
   * so we try to locate it first, no matter where it appears in the list of
   * objects and libraries (note: because the wildcard pattern doesn't match
   * directories, we'll try to find crt0.o in various (sub)directories).
   */
  .startup :
  {
    KEEP (*(.startup_code))
    . = ALIGN(8);
  } > ext_cram =0
  /*
   * Allocate space for absolute addressable sections; this requires that
   * "int_dram" starts at a TriCore segment (256M) and points to
   * some RAM area!  If these conditions are not met by your particular
   * hardware setup, you should either not use absolute data, or you
   * must move .zdata*,.zbss*,.bdata*,.bbss* input sections to some appropriate
   * memory area.
   */
 .zbss  (NOLOAD) :
  {
    ZBSS_BASE = . ;
    *(.zbss)
    *(.zbss.*)
    *(.gnu.linkonce.zb.*)
    *(.bbss)
    *(.bbss.*)
    . = ALIGN(8);
    ZBSS_END = . ;
  } > int_dram
  .zdata  :
  {
    ZDATA_BASE = . ;
    *(.zrodata)
    *(.zrodata.*)
    *(.zdata)
    *(.zdata.*)
    *(.gnu.linkonce.z.*)
    *(.bdata)
    *(.bdata.*)
    . = ALIGN(8);
    ZDATA_END = . ;
  } > int_dram AT> ext_cram

  /*
   * Allocate trap and interrupt vector tables.
   */
  .traptab  :
  {
    *(.traptab)
    . = ALIGN(8) ;
  } > ext_cram

  .inttab  :
  {
    *(.inttab)
    . = ALIGN(8) ;
  } > ext_cram

  /*
   * Allocate .text and other read-only sections.
   */
  .init  :
  {
    *(.init)
    *(.fini)
    . = ALIGN(8);
  } > ext_cram =0
  .text  :
  {
    *(.text)
    *(.text.*)
    *(.pcp_c_ptr_init)
    *(.pcp_c_ptr_init.*)
    *(.gnu.linkonce.t.*)
    /*
     * .gnu.warning sections are handled specially by elf32.em.
     */
    *(.gnu.warning)

    . = ALIGN(8);
  } > ext_cram =0
  .rodata   :
  {
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    *(.toc)
    *(.jcr)
    /*
     * Create the clear and copy tables that tell the startup code
     * which memory areas to clear and to copy, respectively.
     */
    . = ALIGN(4) ;
    PROVIDE(__clear_table = .) ;
    LONG(0 + ADDR(.bss));     LONG(SIZEOF(.bss));
    LONG(0 + ADDR(.sbss));    LONG(SIZEOF(.sbss));
    LONG(0 + ADDR(.zbss));    LONG(SIZEOF(.zbss));
    LONG(-1);                 LONG(-1);
    PROVIDE(__copy_table = .) ;
    LONG(LOADADDR(.data));    LONG(0 + ADDR(.data));    LONG(SIZEOF(.data));
    LONG(LOADADDR(.sdata));   LONG(0 + ADDR(.sdata));   LONG(SIZEOF(.sdata));
    LONG(LOADADDR(.zdata));   LONG(0 + ADDR(.zdata));   LONG(SIZEOF(.zdata));
    LONG(LOADADDR(.pcpdata)); LONG(0 + ADDR(.pcpdata)); LONG(SIZEOF(.pcpdata));
    LONG(LOADADDR(.pcptext)); LONG(0 + ADDR(.pcptext)); LONG(SIZEOF(.pcptext));
    LONG(-1);                 LONG(-1);                 LONG(-1);
    . = ALIGN(8);
  } > ext_cram
  .sdata2  :
  {
    *(.sdata.rodata)
    *(.sdata.rodata.*)
    . = ALIGN(8);
  } > ext_cram
  /*
   * C++ exception handling tables.  NOTE: gcc emits .eh_frame
   * sections when compiling C sources with debugging enabled (-g).
   * If you can be sure that your final application consists
   * exclusively of C objects (i.e., no C++ objects), you may use
   * the -R option of the "strip" and "objcopy" utilities to remove
   * the .eh_frame section from the executable.
   */
  .eh_frame  :
  {
    *(.gcc_except_table)
    __EH_FRAME_BEGIN__ = . ;
    KEEP (*(.eh_frame))
    __EH_FRAME_END__ = . ;
    . = ALIGN(8);
  } > ext_cram
  /*
   * Constructors and destructors.
   */
  .ctors :
  {
    __CTOR_LIST__ = . ;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2);
    *(.ctors)
    LONG(0) ;
    __CTOR_END__ = . ;
    . = ALIGN(8);
  } > ext_cram
  .dtors :
  {
    __DTOR_LIST__ = . ;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2);
    *(.dtors)
    LONG(0) ;
    __DTOR_END__ = . ;
    . = ALIGN(8);
  } > ext_cram
  /*
   * We're done now with the text part of the executable.  The
   * following sections are special in that their initial code or
   * data (if any) must also be stored in said text part of an
   * executable, but they "live" at completely different addresses
   * at runtime -- usually in RAM areas.  NOTE: This is not really
   * necessary if you use a special program loader (e.g., a debugger)
   * to load a complete executable consisting of code, data, BSS, etc.
   * into the RAM of some target hardware or a simulator, but it *is*
   * necessary if you want to burn your application into non-volatile
   * memories such as EPROM or FLASH.
   */
  .pcptext :
  {
    PCODE_BASE = . ;
    *(.pcptext)
    *(.pcptext.*)
    . = ALIGN(8) ;
   PCODE_END = . ;
  } > pcp_text AT> ext_cram
  .pcpdata :
  {
    PRAM_BASE = . ;
    *(.pcpdata)
    *(.pcpdata.*)
    . = ALIGN(8) ;
    PRAM_END = . ;
  } > pcp_data AT> ext_cram
  .data :
  {
    . = ALIGN(8) ;
    DATA_BASE = . ;
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    SORT(CONSTRUCTORS)
    . = ALIGN(8) ;
    DATA_END = . ;
  } > int_dram AT> ext_cram
  .sdata  :
  {
    . = ALIGN(8) ;
    SDATA_BASE = . ;
    PROVIDE(__sdata_start = .);
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(8) ;
  } > int_dram AT> ext_cram
  .sbss  :
  {
    PROVIDE(__sbss_start = .);
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    . = ALIGN(8) ;
  } > int_dram
  /*
   * Allocate space for BSS sections.
   */
  .bss  (NOLOAD) :
  {
    BSS_BASE = . ;
    *(.bss)
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(8) ;
    __ISTACK = . + __ISTACK_SIZE ;
    __USTACK = __ISTACK + __USTACK_SIZE ;
    __HEAP = __USTACK ;
    __HEAP_END = __RAM_END ;
  } > int_dram
  _end = __HEAP_END ;
  PROVIDE(end = _end) ;
  /* Make sure CSA, stack and heap addresses are properly aligned.  */
  _. = ASSERT ((__CSA_BEGIN & 0x3f) == 0 , "illegal CSA start address") ;
  _. = ASSERT ((__CSA_SIZE & 0x3f) == 0 , "illegal CSA size") ;
  _. = ASSERT ((__ISTACK & 7) == 0 , "ISTACK not doubleword aligned") ;
  _. = ASSERT ((__USTACK & 7) == 0 , "USTACK not doubleword aligned") ;
  _. = ASSERT ((__HEAP_END & 7) == 0 , "HEAP not doubleword aligned") ;

  /* Define a default symbol for address 0.  */
  NULL = DEFINED (NULL) ? NULL : 0 ;
  /*
   * DWARF debug sections.
   * Symbols in the DWARF debugging sections are relative to the
   * beginning of the section, so we begin them at 0.
   */
  /*
   * DWARF 1
   */
  .comment         0 : { *(.comment) }
  .debug           0 : { *(.debug) }
  .line            0 : { *(.line) }
  /*
   * GNU DWARF 1 extensions
   */
  .debug_srcinfo   0 : { *(.debug_srcinfo) }
  .debug_sfnames   0 : { *(.debug_sfnames) }
  /*
   * DWARF 1.1 and DWARF 2
   */
  .debug_aranges   0 : { *(.debug_aranges) }
  .debug_pubnames  0 : { *(.debug_pubnames) }
  /*
   * DWARF 2
   */
  .debug_info      0 : { *(.debug_info) }
  .debug_abbrev    0 : { *(.debug_abbrev) }
  .debug_line      0 : { *(.debug_line) }
  .debug_frame     0 : { *(.debug_frame) }
  .debug_str       0 : { *(.debug_str) }
  .debug_loc       0 : { *(.debug_loc) }
  .debug_macinfo   0 : { *(.debug_macinfo) }
  .debug_ranges    0 : { *(.debug_ranges) }
  /*
   * SGI/MIPS DWARF 2 extensions
   */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /*
   * Optional sections that may only appear when relocating.
   */
  /*
   * Optional sections that may appear regardless of relocating.
   */
  .version_info    0 : { *(.version_info) }
  .boffs           0 : { KEEP (*(.boffs)) }
}
