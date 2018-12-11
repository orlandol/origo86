
  ORG 0x100
  CPU 286

section .text

  mov dx, 0xFFF
  mov ax, 0xFFF
  mul dx
call dump

  xor dx, dx
  mov cx, 0xFFF
  mov ax, 0xFFF
  mul cx
call dump
  
  int 0x20

  %include "debug.inc"
