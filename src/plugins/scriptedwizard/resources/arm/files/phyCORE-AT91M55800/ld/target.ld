/*
    linker script for phyCORE_AT91M55800
 */

ENTRY(reset_vector)

MEMORY
{
    ram1 : ORIGIN = 0x00000000, LENGTH = 0x2000
    ram2 : ORIGIN = 0x04100000, LENGTH = 0x100000
    rom  : ORIGIN = 0x04000000, LENGTH = 0x100000
}

SECTIONS
{

    PROVIDE (PxPrepareInit = 0);

    .rom_vectors :
    {
      KEEP (*(.vectors))
    } > rom 
    .text :
    {
      stext = ABSOLUTE(.); PROVIDE (__stext = ABSOLUTE(.));
      *(.text*)
      /* C++ stuff. */
      *(.gcc_except_table)
      *(.eh_frame)
      /* .gnu.warning sections are handled specially by elf32.em.  */
      *(.gnu.warning)
      *(.gnu.linkonce.t*)
      *(.init)
      *(.glue_7)
      *(.glue_7t)
      *(.fixup)
      *(.got)
      *(.fini)
    } > rom  etext = .; PROVIDE (__etext = .); 
    .rodata :
    {
       *(.rodata*) *(.rodata.*) *(.toc) *(.gnu.linkonce.r*)
      . = ALIGN (4);
    } > rom  
    .rodata1 :
    {
       *(.rodata1)
      . = ALIGN (4);
    } > rom  

    .jcr :
    {
       KEEP (*(.jcr))
      . = ALIGN (4);
    } > rom

    .data :
    {
      __ram_data_start = ABSOLUTE (.);
      *(.data*) *(.gnu.linkonce.d*)
      *(.data1)   _GOT1_START_ = ABSOLUTE (.);
      *(.got1) _GOT1_END_ = ABSOLUTE (.); _GOT2_START_ = ABSOLUTE (.);
      *(.got2) _GOT2_END_ = ABSOLUTE (.);
      . = ALIGN (4);
      __DEVTAB__ = ABSOLUTE (.);
      KEEP (*(SORT (.devtab*))) __DEVTAB_END__ = ABSOLUTE (.);
      __NETDEVTAB__ = ABSOLUTE (.);
      KEEP (*(SORT (.netdevtab*))) __NETDEVTAB_END__ = ABSOLUTE (.);
      __CTOR_LIST__ = ABSOLUTE (.);
      KEEP (*(SORT (.ctors*))) __CTOR_END__ = ABSOLUTE (.);
      __DTOR_LIST__ = ABSOLUTE (.);
      KEEP (*(SORT (.dtors*))) __DTOR_END__ = ABSOLUTE (.);
      *(.dynamic)
      *(.sdata*)
      *(.sbss*)
    } > ram2 AT> rom
    __rom_data_start = LOADADDR (.data);
    __ram_data_end = .; PROVIDE (__ram_data_end = .);
    _edata = .; PROVIDE (edata = .); PROVIDE (__rom_data_end = LOADADDR (.data) + SIZEOF(.data));

    .bss :
    {
      . = ALIGN (4);
      __bss_start = ABSOLUTE (.);
      *(.scommon)
      *(.dynbss)
      *(.bss*) *(.gnu.linkonce.b*)
      *(COMMON)
      __bss_end = ABSOLUTE (.);
    } > ram2  
    . = ALIGN(4);
    _end = .; PROVIDE (end = .); PROVIDE (__end__ = .); PROVIDE (__HEAP = .);
    PROVIDE (__heap_end__ = . + 0x8000); PROVIDE (__HEAP_END = . + 0x8000);

    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    .debug_info     0 : { *(.debug_info) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) } 
}
