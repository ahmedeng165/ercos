
ENTRY(ercos_hal_reset_entry)


REGMAP = 0x80000000;
MCTRL = 0x80000000;
UARTA = 0x80000100;
IRQMP = 0x80000200;
GPTIMER = 0x80000300;
REGWIN = 0x80000600;


MEMORY
{
 ram : ORIGIN = 0x40000000,
          LENGTH = 4M
}

SECTIONS
{
  .text :
  {
    CODE_START = .;

    *(.romvec)
    *(.text*)
    *(.rodata*)
    *(.gnu.linkonce.*)
   *(.eh_frame)

    . = ALIGN(0x10);

    CODE_END = .;

  } > ram

  .data :
  {
   DATA_START = .;

    *(.data)

    . = ALIGN(0x10);
 __ctors_GLOBAL__ = .;
    *(.ctors)
 QUAD(0)
 . = ALIGN(0x10);
 __dtors_GLOBAL__ = .;
    *(.dtors)
 QUAD(0)
 . = ALIGN(0x10);

    DATA_END = .;

  } > ram

  .bss :
  {
    bss_start = .;
    *(.bss)
    *(COMMON)
    . = ALIGN(0x10);
    end = .;
  } > ram

  /DISCARD/ :
  {
    *(*)
  }
}
