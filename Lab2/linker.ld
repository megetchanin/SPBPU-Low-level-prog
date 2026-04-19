OUTPUT_ARCH( "riscv" )
ENTRY( _start )

SECTIONS
{
  START_ADDRESS = 0x80000000;
  STACK_SIZE = 0x800;
  
  . = START_ADDRESS;

  .text : { 
      *(.text) 
      *(.text.*)
  }
  
  .rodata : { 
      *(.rodata) 
      *(.rodata.*)
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
  }

  . = ALIGN(16);
  stack_bottom = .;
  . += STACK_SIZE;
  stack_top = .;

  _end = .;
}
