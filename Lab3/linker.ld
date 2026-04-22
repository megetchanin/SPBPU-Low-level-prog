OUTPUT_ARCH( "riscv" )
ENTRY( _start )

SECTIONS
{
  START_ADDRESS = 0x80000000;
  STACK_SIZE = 0x1000;

  /* start address */
  . = START_ADDRESS;

  /* text: обычный + "function-sections" варианты */
  .text : {
    *(.text)
    *(.text.*)
  }

  /* отдельная секция для всего sw_timer.c (плюсик #3) */
  .sw_timer : {
    *(.sw_timer)
    *(.sw_timer.*)
  }

  /* data */
  .rodata : {
    *(.rodata)
    *(.rodata.*)
    *(.srodata)
    *(.srodata.*)
  }

  .data : {
    *(.data)
    *(.data.*)
  }

  .sdata : {
    *(.sdata)
    *(.sdata.*)
  }

  .bss : {
    *(.bss)
    *(.bss.*)
    *(.sbss)
    *(.sbss.*)
    *(COMMON)
  }

  .debug : { *(.debug) }

  /* stack */
  . = ALIGN(16);
  stack_bottom = .;
  . += STACK_SIZE;
  stack_top = .;

  _end = .;
}