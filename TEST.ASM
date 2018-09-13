
  ORG 0x100

  int 0x20

  mov al, [es:123 + bx - 10 * 2 + di - 3]
