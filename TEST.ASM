
  ORG 0x100
  CPU 8086

  push    bp
  mov     bp, sp

  sub     sp, 4
  mov     word [bp - 2], 0x1234
  mov     word [bp - 4], 0x2345
  call    foo

  mov     sp, bp
  pop     bp
  int 0x20

foo:
  ret 4
