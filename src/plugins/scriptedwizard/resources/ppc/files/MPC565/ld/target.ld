OUTPUT_FORMAT("elf32-powerpc", "elf32-powerpc",
	      "elf32-powerpc")
OUTPUT_ARCH(powerpc)
ENTRY(_start)
/**************** USER DEFINED ********************/

MEMORY
{
	vectorspace	:	org = 0x00000000,   l = 0x2200
	ext_rom		:	org = 0x00004000,   l = 0x7C000
	int_cram    	:   	org = 0x01000000,   l = 0x8000
	ext_ram		:	org = 0x00080000,   l = 0x80000
}


SECTIONS
{
  /* Read-only sections, merged into text segment: */
    
    .vectab :
    {
  	*(.vectab)
    } > vectorspace
  
  .text      :
  {
    *(.text)
    /* trap and vector tables. */
    *(.traptab)
    *(.inttab)
    /* C++ stuff. */
    *(.gcc_except_table)
    *(.eh_frame)
    /* .gnu.warning sections are handled specially by elf32.em.  */
    *(.gnu.warning)
    *(.gnu.linkonce.t*)
    *(.init)
    *(.fixup)
    *(.got2)
    *(.got1)
    *(.got)
    *(.fini)
  } > ext_rom
  _etext = .;
  PROVIDE (etext = .);

  .rodata : 
   { 
    *(.rodata) *(.rodata.*) *(.toc) *(.gnu.linkonce.r*) 
    /* we generate the clear table (sections to be cleared at runtime) here */
     . = (. + 3) & -4 ;
    __clear_table = . ;
    LONG( 0 + ADDR(.bss) ) ; LONG( SIZEOF(.bss) );
    LONG( 0 + ADDR(.sbss) ) ; LONG( SIZEOF(.sbss) );
    LONG( 0 + ADDR(.sbss2) ) ; LONG( SIZEOF(.sbss2) );
    LONG(-1) ; LONG(-1) ;

    /* now the copy table ( section to be copied at runtime from load address
       to the runtime area) */
    __copy_table = . ;
    LONG( 0 + LOADADDR(.sdata)); LONG(ABSOLUTE(SDATA_BASE)); LONG (SIZEOF(.sdata));
    LONG( 0 + LOADADDR(.sdata2)); LONG(ADDR(.sdata2)); LONG (SIZEOF(.sdata2));
    LONG( 0 + LOADADDR(.data)); LONG(ABSOLUTE(DATA_BASE)); LONG (SIZEOF(.data));
    
    LONG(-1) ;  LONG (-1) ; LONG (-1) ;
   } > ext_rom

  .rodata1  : { *(.rodata1) } > ext_rom

  .ctors	  :
  {
    __CTOR_LIST__ = .;
    PROVIDE (__CTOR_LIST__ = .);
   *(.ctors)
    __CTOR_END__ = .;
    PROVIDE (__CTOR_END__ = .);
  } > ext_rom
  .dtors	 :
  {
    __DTOR_LIST__ = .;
    PROVIDE (__DTOR_LIST__ = .);
   *(.dtors)
    __DTOR_END__ = .;
    PROVIDE (__DTOR_END__ = .); 
  } > ext_rom

  .jcr            : { KEEP (*(.jcr)) } > ext_rom
  __FLASH_END = ALIGN (8) ;

  
  .sdata :
    AT ( __FLASH_END)
  { 
    SDATA_BASE = ABSOLUTE(.) ;
    *(.sdata .sdata.* .gnu.linkonce.s.*)
  } > ext_ram

  .sbss :
  { 
    __sbss_start = . ;
    *(.dynsbss)
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon) 
  } > ext_ram

  .sdata2 :
    AT ( LOADADDR (.sdata) + SIZEOF (.sdata)) 
  { 
    *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
  } > ext_ram

  .sbss2 :
  { 
    __sbss2_start = . ;
    *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*)
  } > ext_ram

  .data :
    AT ( LOADADDR (.sdata2) + SIZEOF (.sdata2))
  {
    DATA_BASE = ABSOLUTE(.) ;
    . = ALIGN(8) ;
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d*)
    CONSTRUCTORS
    DATA_END = ABSOLUTE(.) ;
  } > ext_ram

  
  /* we allocate the .toc section of the different moduls to .rodata */
  .data1 ((. + 7) & -8)  : { *(.data1) }
  _edata  =  .;
  PROVIDE (edata = .);

  .bss     :
  {
   PROVIDE (__bss_start = .);
   *(.dynbss)
   *(.bss .bss.* .gnu.linkonce.b.*)
   __HEAP = .;
    . += 0x8000;
   __HEAP_END = .;
   __USER_STACK_BOTTOM = .;
   . += 0x1000;
   __USER_STACK_TOP = .;
   *(COMMON)
   PROVIDE(__bss_end = .);
  } > ext_ram
  _end = . ;
  PROVIDE (end = .);
  /* These are needed for ELF backends which have not yet been
     converted to the new style linker.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /* These must appear regardless of  .  */
}
