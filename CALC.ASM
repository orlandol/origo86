
  ORG 0x100

  %include "defs.inc"

  %include "initrtl.inc"

section .text
run:
  mov     ax, 0x4C00
  int     0x21

  %include "debug.inc"

  %include "rtl.inc"
