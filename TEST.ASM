
  ORG 0x100

 foo:
  db 0b1100_0111, 0b11_000_000, 0x00, 0x4C

  int 0x20

  jmp near foo
