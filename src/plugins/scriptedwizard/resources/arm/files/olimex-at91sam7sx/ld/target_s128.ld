/*
    linker script for AT91SAM7S128
 */

ENTRY(reset_vector)

MEMORY
{
    CODE (rx) : ORIGIN = 0x00100000, LENGTH = 0x00020000
    VECT (rw) : ORIGIN = 0x00200000, LENGTH = 0x00000040
    DATA (rw) : ORIGIN = 0x00200040, LENGTH = 0x00007FC0
}

SECTIONS
{

    PROVIDE (PxPrepareInit = 0);

    .rom_vectors :
    {
      KEEP (*(.vectors))
    } > CODE =0 
    .text :
    {
      stext = ABSOLUTE(.); PROVIDE (__stext = ABSOLUTE(.));
      *(.text*)
      /* C++ stuff. */
      *(.gcc_except_table)
      *(.eh_frame)
      /* .gnu.warning sections are handled specially by elf32.em. */
      *(.gnu.warning)
      *(.gnu.linkonce.t*)
      *(.init)
      *(.glue_7)
      *(.glue_7t)
      *(.fixup)
      *(.got)
      *(.fini)
    } > CODE  etext = .; PROVIDE (__etext = .); 
    .rodata :
    {
       *(.rodata*) *(.rodata.*) *(.toc) *(.gnu.linkonce.r*)
      . = ALIGN (4);
    } > CODE  
    .rodata1 :
    {
       *(.rodata1)
      . = ALIGN (4);
    } > CODE  

    .jcr :
    {
       KEEP (*(.jcr))
      . = ALIGN (4);
    } > CODE

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
    } > DATA AT> CODE
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
    } > DATA  
    . = ALIGN(4);
    _end = .; PROVIDE (end = .); PROVIDE (__end__ = .); PROVIDE (__HEAP = .);
    PROVIDE (__heap_end__ = . + 0x0040); PROVIDE (__HEAP_END = . + 0x0040);

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
