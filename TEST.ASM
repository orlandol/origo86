
  ORG 0x100

  int 0x20

  rep cmpsb
  repe cmpsb
  repne cmpsb
  repz cmpsb
  repnz cmpsb
