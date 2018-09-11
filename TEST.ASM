
  ORG 0x100

  int 0x20

  rcl bh, cl
  db 0b1101_0010, 0b11_010_111
