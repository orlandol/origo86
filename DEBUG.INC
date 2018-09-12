section .text
;
; func dump()
;
dump:
  push    bp
  mov     bp, sp

  %define dumpBP bp +  0
  %define dumpAX bp - 12
  %define dumpBX bp - 10
  %define dumpCX bp -  8
  %define dumpDX bp -  6
  %define dumpSI bp -  4
  %define dumpDI bp -  2

  push    di
  push    si
  push    dx
  push    cx
  push    bx
  push    ax

  ; uint(ax)
  push    word (dumpLine2 +  9)
  push    word [dumpAX]
  call    dumphex16

  push    word (dumpLine2 + 15)
  push    word [dumpAX]
  call    dumpu16

  ; uint(bx)
  push    word (dumpLine2 + 30)
  push    word [dumpBX]
  call    dumphex16

  push    word (dumpLine2 + 36)
  push    word [dumpBX]
  call    dumpu16

  ; uint(cx)
  push    word (dumpLine3 +  9)
  push    word [dumpCX]
  call    dumphex16

  push    word (dumpLine3 + 15)
  push    word [dumpCX]
  call    dumpu16

  ; uint(dx)
  push    word (dumpLine3 + 30)
  push    word [dumpDX]
  call    dumphex16

  push    word (dumpLine3 + 36)
  push    word [dumpDX]
  call    dumpu16

  ; uint(si)
  push    word (dumpLine4 +  9)
  push    word [dumpSI]
  call    dumphex16

  push    word (dumpLine4 + 15)
  push    word [dumpSI]
  call    dumpu16

  ; uint(di)
  push    word (dumpLine4 + 30)
  push    word [dumpDI]
  call    dumphex16

  push    word (dumpLine4 + 36)
  push    word [dumpDI]
  call    dumpu16

  ; uint(bp)
  push    word (dumpLine5 +  9)
  push    word [dumpBP]
  call    dumphex16

  push    word (dumpLine5 + 15)
  push    word [dumpBP]
  call    dumpu16

  ; uint(sp)
  push    word (dumpLine5 + 30)
  push    bp
  call    dumphex16

  push    word (dumpLine5 + 36)
  push    bp
  call    dumpu16

  ; Print formatted text
  mov     dx, dumpLine1
  mov     ah, 9
  int     0x21

  mov     sp, bp
  pop     bp

  mov     ax, 0x4C01
  int     0x21
  ret

;
; func dumphex16( char[4] dest, uint16 value )
;
dumphex16:
  push    bp
  mov     bp, sp

  %define dest  bp + 6
  %define value bp + 4

  mov     di, [dest]
  add     di, 3
  mov     ax, [value]
  mov     cx, 4

 .HexLoop:
  mov     bx, ax
  shr     ax, 4
  and     bx, 0xF
  mov     dl, [bx + hexDigits]
  mov     [di], dl
  dec     di
  dec     cx
  jnz     .HexLoop

  mov     sp, bp
  pop     bp
  ret 4

section .data
 hexDigits: db '0123456789ABCDEF'

section .text
;
; func dumpu16( char[5] dest, uint16 value )
;
dumpu16:
  push    bp
  mov     bp, sp

  %define dest  bp + 6
  %define value bp + 4

  mov     ax, [value]
  xor     dx, dx
  mov     di, [dest]
  mov     bx, 10
  test    di, di
  jz      .Exit
  add     di, 4

 .DigitLoop:
  div     bx
  add     dl, '0'
  mov     [di], dl
  dec     di
  xor     dx, dx
  test    ax, ax
  jnz     .DigitLoop

 .Exit:
  mov     sp, bp
  pop     bp
  ret 2

section .data
dumpLine1: db '  --  Registers --------------------------',13,10,
dumpLine2: db '  ax = 0x0000 (00000)  bx = 0x0000 (00000)',13,10,
dumpLine3: db '  cx = 0x0000 (00000)  dx = 0x0000 (00000)',13,10,
dumpLine4: db '  si = 0x0000 (00000)  di = 0x0000 (00000)',13,10,
dumpLine5: db '  bp = 0x0000 (00000)  sp = 0x0000 (00000)',13,10,
dumpLine6: db '  ----------------------------------------',13,10,'$'

