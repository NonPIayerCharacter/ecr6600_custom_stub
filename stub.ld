/*
Linker file used to link the bootloader.
*/

MEMORY
{
    boot_rom_ro (RWX) :     org = 0x00010000, len = 0x800000  /* the length of rom code (read only part)*/
}

/*  Default entry point:  */
ENTRY(_boot_start);


SECTIONS
{
  .nds32_init :
  {
    _nds32_init_start = ABSOLUTE(.);
	  KEEP (*(.nds32_init))
    _nds32_init_end = ABSOLUTE(.);
  } > boot_rom_ro
  
  .rom.text :
  {
    . = ALIGN (4);
     _stext = .;
    _text_start = ABSOLUTE(.);
    *(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *libc.a:(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *libgcc.a:(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *libm.a:(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *libdsp.a:(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)
    _text_end = ABSOLUTE(.);
    _etext = .;
  } > boot_rom_ro

  .rom.rodata :
  {
    _rodata_start = ABSOLUTE(.);
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    _rodata_end = ABSOLUTE(.);
	/* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _rodata_end = ABSOLUTE(.);
    . = ALIGN(4);
  } > boot_rom_ro


  .rom.data 0x00060000 : AT(ADDR(.rom.rodata) + SIZEOF(.rom.rodata))
  {
    __data_lma = ADDR(.rom.rodata) + SIZEOF(.rom.rodata);
    __data_vma_start = ABSOLUTE(.);
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)
	*(.version.data)
    __data_vma_end = ADDR(.rom.data) + SIZEOF(.rom.data);
  } > boot_rom_ro
  
    /* bss segment don't need load to RAM */
  .rom.bss (NOLOAD)  :
  {
    __bss_start = ABSOLUTE(.);
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.scommon_w)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    __bss_end = ABSOLUTE(.);
  } > boot_rom_ro


  PROVIDE (_stack = 0x0007F9F0); /* top of DRAM */
  /* comment or debug information */
  .comment	0 : { *(.comment) }
  .debug	0 : { *(.debug) }
  .line	0 : { *(.line) }
  .debug_srcinfo	0 : { *(.debug_srcinfo) }
  .debug_sfnames	0 : { *(.debug_sfnames) }
  .debug_aranges	0 : { *(.debug_aranges) }
  .debug_pubnames	0 : { *(.debug_pubnames) }
  .debug_info	0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev	0 : { *(.debug_abbrev) }
  .debug_line	0 : { *(.debug_line) }
  .debug_frame	0 : { *(.debug_frame) }
  .debug_str	0 : { *(.debug_str) }
  .debug_loc	0 : { *(.debug_loc) }
  .debug_macinfo	0 : { *(.debug_macinfo) }
  .debug_weaknames	0 : { *(.debug_weaknames) }
  .debug_funcnames	0 : { *(.debug_funcnames) }
  .debug_typenames	0 : { *(.debug_typenames) }
  .debug_varnames	0 : { *(.debug_varnames) }
}

PROVIDE(__BOOTROM_GP__ = 0x7FE20);

INCLUDE "nds_rom_common.ld"