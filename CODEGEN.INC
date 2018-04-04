;
; func bool EmitOp( uint format, uint8[16] instr )
;
EmitOp:
  push    bp
  mov     bp, sp
  push    word 0
  push    word 0
  push    word 0
  push    word 0
  push    word 0
  push    word 0
  push    word 0
  push    word 0

  lea     di, [bp - 2 - 16]
  mov     si, [bp + 4]
  mov     dx, [bp + 6]
  xor     cx, cx
  xor     ax, ax

  test    si, si
  jz      .Exit
  test    dx, dx
  jz      .Exit

 .Exit:
  mov     sp, bp
  pop     bp
  ret 4
