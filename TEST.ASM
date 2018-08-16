
  ORG 0x100

  int 0x20

  xchg cl, [foo]
  xchg bx, [foo]
 foo:
