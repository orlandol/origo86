
  ORG 0x100

  %include "defs.inc"

  %include "initrtl.inc"

section .text
run:
  mov     ax, 0x4C00
  int     0x21

;
; func uint ExprGetToken()
;
ExprGetToken:
  ret

section .data
  operandStr: times 64 db 0

  expr: db "1234 + 4321 / 11"

  %include "debug.inc"

  %include "rtl.inc"
