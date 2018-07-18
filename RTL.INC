; Default 64K Layout:
;  8K - Code
; 54K - Data
;  2K - Stack

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
 hexDigits: db '0123456789ABCDEF'

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

dumpLine1: db '  --  Registers --------------------------',13,10,
dumpLine2: db '  ax = 0x0000 (00000)  bx = 0x0000 (00000)',13,10,
dumpLine3: db '  cx = 0x0000 (00000)  dx = 0x0000 (00000)',13,10,
dumpLine4: db '  si = 0x0000 (00000)  di = 0x0000 (00000)',13,10,
dumpLine5: db '  bp = 0x0000 (00000)  sp = 0x0000 (00000)',13,10,
dumpLine6: db '  ----------------------------------------',13,10,'$'

;
; func int compare( string left, string right )
;
compare:
  push    bp
  mov     bp, sp

  mov     di, [bp + 4]
  mov     si, [bp + 6]

  xor     ax, ax
  mov     cx, 1

  test    di, di
  jz      .Exit
  test    si, si
  jz      .Exit

  mov     al, [si]
  mov     cl, [di]

 .CompareLoop:
  cmp     al, cl
  jne     .ExitCompare
  test    al, al
  jz      .ExitCompare
  test    cl, cl
  jz      .ExitCompare
  mov     al, [si]
  inc     si
  mov     cl, [di]
  inc     di
  jmp     .CompareLoop
 .ExitCompare:

 .Exit:
  sub     ax, cx

  mov     sp, bp
  pop     bp
  ret 4

;
; func int comparenc( string left, string right )
;
comparenc:
  push    bp
  mov     bp, sp

  mov     di, [bp + 4]
  mov     si, [bp + 6]

  xor     ax, ax
  mov     cx, 1

  test    di, di
  jz      .Exit
  test    si, si
  jz      .Exit

  mov     al, [si]
  mov     cl, [di]

 .CompareLoop:
  cmp     al, 'A'
  jb      .LeftNotUppercase
  cmp     al, 'Z'
  ja      .LeftNotUppercase
  or      al, 32
 .LeftNotUppercase:

  cmp     cl, 'A'
  jb      .RightNotUppercase
  cmp     cl, 'Z'
  ja      .RightNotUppercase
  or      cl, 32
 .RightNotUppercase:

  cmp     al, cl
  jne     .ExitCompare
  test    al, al
  jz      .ExitCompare
  test    cl, cl
  jz      .ExitCompare
  mov     al, [si]
  inc     si
  mov     cl, [di]
  inc     di
  jmp     .CompareLoop
 .ExitCompare:

 .Exit:
  sub     ax, cx

  mov     sp, bp
  pop     bp
  ret 4

;
; func echochar( char val )
;
echochar:
  push    bp
  mov     bp, sp

  mov     ah, 2
  mov     dl, [bp + 4]
  int     0x21

  mov     sp, bp
  pop     bp
  ret 2

;
; func echostring( string val )
;
echostring:
  push    bp
  mov     bp, sp

  mov     si, [bp + 4]
  mov     ah, 2
  test    si, si
  jz      .Exit

 .CharLoop:
  mov     dl, [si]
  inc     si
  test    dl, dl
  jz      .Exit
  int     0x21
  jmp     .CharLoop

 .Exit:
  mov     sp, bp
  pop     bp
  ret 2

;
; func echouint( uint val )
;
echouint:
  push    bp
  mov     bp, sp

  mov     ax, [bp + 4]
  xor     dx, dx
  xor     si, si
  mov     bx, 10

 .DigitLoop:
  div     bx
  add     dl, '0'
  inc     si
  push    dx
  xor     dx, dx
  test    ax, ax
  jnz     .DigitLoop

  mov     ah, 2

 .EchoLoop:
  pop     dx
  int     0x21
  dec     si
  jnz     .EchoLoop

  mov     sp, bp
  pop     bp
  ret 2

;
; func echouint32( uint32 val )
;
echouint32:
  push    bp
  mov     bp, sp

  mov     cx, [bp + 4 + 0]
  mov     ax, [bp + 4 + 2]
  xor     dx, dx
  xor     si, si
  mov     bx, 10

 .LongDigitLoop:
  div     bx
  xchg    ax, cx
  div     bx
  xchg    ax, cx
  add     dl, '0'
  inc     si
  push    dx
  xor     dx, dx
  test    cx, cx
  jnz     .LongDigitLoop
  test    ax, ax
  jz      .InitEchoLoop

 .DigitLoop:
  div     bx
  add     dl, '0'
  inc     si
  push    dx
  xor     dx, dx
  test    ax, ax
  jnz     .DigitLoop

 .InitEchoLoop:
  mov     ah, 2

 .EchoLoop:
  pop     dx
  int     0x21
  dec     si
  jnz     .EchoLoop

  mov     sp, bp
  pop     bp
  ret 2

;
; func file fcreate( string name )
;
fcreate:
  push    bp
  mov     bp, sp

  mov     si, [bp + 4]
  xor     ax, ax
  test    si, si
  jz      .Exit
  cmp     [si], byte 0
  jz      .Exit

  mov     ah, 0x3C
  xor     cx, cx
  mov     dx, si
  int     0x21

  jnc     .Exit
  xor     ax, ax

 .Exit:
  mov     sp, bp
  pop     bp
  ret 2

;
; func file fmodify( string name )
;
fmodify:
  push    bp
  mov     bp, sp

  mov     si, [bp + 4]
  xor     ax, ax
  test    si, si
  jz      .Exit
  cmp     [si], byte 0
  jz      .Exit

  mov     ax, 0x3D02
  mov     dx, si
  int     0x21

  jnc     .Exit

  mov     ah, 0x3C
  xor     cx, cx
  mov     dx, si
  int     0x21

  jnc     .Exit
  xor     ax, ax

 .Exit:
  mov     sp, bp
  pop     bp
  ret 2

;
; func file fopen( string name )
;
fopen:
  push    bp
  mov     bp, sp

  mov     si, [bp + 4]
  xor     ax, ax
  test    si, si
  jz      .Exit
  cmp     [si], byte 0
  jz      .Exit

  mov     ax, 0x3D00
  mov     dx, si
  int     0x21

  jnc     .Exit
  xor     ax, ax

 .Exit:
  mov     sp, bp
  pop     bp
  ret 2

;
; func fclose( @file handle )
;
fclose:
  push    bp
  mov     bp, sp

  mov     si, [bp + 4]
  test    si, si
  jz      .Error

  mov     ah, 0x3E
  mov     bx, [si]
  test    bx, bx
  jz      .Error
  int     0x21

  mov     word [si], 0

 .Error:
  mov     sp, bp
  pop     bp
  ret 2

;
; func void fflush( file handle )
;
fflush:
  push    bp
  mov     bp, sp

  mov     bx, [bp + 4]
  xor     ax, ax
  test    bx, bx
  jz      .Exit

  mov     ah, 0x68
  int     0x21

  mov     ax, 1
  sbb     ax, 0

 .Exit:
  mov     sp, bp
  pop     bp
  ret 2

;
; func uint fwrite( file handle, @src, uint size )
;
fwrite:
  push    bp
  mov     bp, sp

  %define handle bp + 8
  %define src    bp + 6
  %define size   bp + 4

  mov     cx, [size]
  mov     dx, [src]
  mov     bx, [handle]
  xor     ax, ax

  test    cx, cx
  jz      .Exit
  test    dx, dx
  jz      .Exit
  test    bx, bx
  jz      .Exit

  mov     ah, 0x40
  int     0x21

  jnc     .Exit
  xor     ax, ax

 .Exit:
  mov     sp, bp
  pop     bp
  ret 6

;
; func uint fread( file handle, @dest, uint size )
;
fread:
  push    bp
  mov     bp, sp

  mov     cx, [bp + 4]
  mov     dx, [bp + 6]
  mov     bx, [bp + 8]
  xor     ax, ax

  test    cx, cx
  jz      .Exit
  test    dx, dx
  jz      .Exit
  test    bx, bx
  jz      .Exit

  mov     ah, 0x3F
  int     0x21

  jnc     .Exit
  xor     ax, ax

 .Exit:
  mov     sp, bp
  pop     bp
  ret 6

;
; func fuint fpos( file handle )
;
fpos:
  push    bp
  mov     bp, sp

  xor     dx, dx
  xor     cx, cx
  mov     bx, [bp + 4]
  xor     ax, ax

  test    bx, bx
  jz      .Exit

  mov     ah, 0x42
  mov     al, 1
  int     0x21

  mov     bx, 0xFFFF
  adc     bx, 0
  and     ax, bx
  and     dx, bx

 .Exit:
  mov     sp, bp
  pop     bp
  ret 2

;
; func bool fseek( file handle, uint32 position )
;
fseek:
  push    bp
  mov     bp, sp

  mov     dx, [bp + 4 + 0]
  mov     cx, [bp + 4 + 2]
  mov     bx, [bp + 8]
  xor     ax, ax

  test    bx, bx
  jz      .Exit

  mov     ah, 0x42
  int     0x21

  mov     ax, 1
  sbb     ax, 0

 .Exit:
  mov     sp, bp
  pop     bp
  ret 6

;
; func bool fseekby( file handle, fint amount )
;
fseekby:
  push    bp
  mov     bp, sp

  mov     dx, [bp + 4 + 0]
  mov     cx, [bp + 4 + 2]
  mov     bx, [bp + 8]
  xor     ax, ax

  test    bx, bx
  jz      .Exit

  mov     ah, 0x42
  mov     al, 1
  int     0x21

  mov     ax, 1
  sbb     ax, 0

 .Exit:
  mov     sp, bp
  pop     bp
  ret 6

minMemString:     db 'Error: Need 64K to run.',13,10,13,10,0

eoln: db 13,10,0

argc:   dw 0
arg:    times 127 dw 0
argstr: times 128 db 0

maxAvail: dw 0
memAvail: dw 0

heapData:
