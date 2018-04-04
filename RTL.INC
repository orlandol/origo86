; Default 64K Layout:
;  8K - Code
; 54K - Data
;  2K - Stack

;
; func @ alloc( uint size )
;
alloc:
  push    bp
  mov     bp, sp
  %define size bp + 4
  %define result bp - 2
  push    word 0

  mov     cx, [size]
  test    cx, cx
  jz      .Exit

  xor     bx, bx
  mov     si, heapData
  mov     dx, 0xFFFF

 .FindBestFit:
  cmp     si, heapDataEnd
  jae     .DoneBestFit
  cmp     word [si + HEAPITEM_SIG], SIG_FREEBLOCK
  jne     .NextBestFit
  mov     ax, [si + HEAPITEM_SIZE]
  cmp     cx, ax
  ja      .NextBestFit
  cmp     ax, dx
  jae     .NextBestFit
  mov     bx, si
  mov     dx, ax
 .NextBestFit:
  add     si, [si + HEAPITEM_SIZE]
  add     si, SIZEOF_HEAPITEM
  jmp     .FindBestFit
 .DoneBestFit:

  test    bx, bx
  jz      .Exit

  mov     ax, dx
  sub     ax, cx
  cmp     ax, ALLOC_MINSIZE
  ja      .NoGap
  add     cx, ax
 .NoGap:

  cmp     cx, dx
  je      .NoSplit
  lea     di, [bx + SIZEOF_HEAPITEM]
  add     di, cx
  sub     dx, cx
  sub     dx, SIZEOF_HEAPITEM
  mov     word [di + HEAPITEM_SIG], SIG_FREEBLOCK
  mov     [di + HEAPITEM_SIZE], dx
 .NoSplit:

  mov     word [bx + HEAPITEM_SIG], SIG_USEDBLOCK
  mov     [bx + HEAPITEM_SIZE], cx

  add     bx, SIZEOF_HEAPITEM
  mov     [result], bx

 .Exit:
  mov     ax, [result]

  mov     sp, bp
  pop     bp
  ret 2

;
; func free( &ptr )
;
free:
  push    bp
  mov     bp, sp

  mov     bx, [bp + 4]
  test    bx, bx
  jz      .Exit
  mov     di, [bx]
  test    di, di
  jz      .Exit

 .Exit:
  mov     sp, bp
  pop     bp
  ret 2

;
; func @ realloc( @ pointer, uint size )
;
realloc:
  push    bp
  mov     bp, sp

  mov     ax, [bp + 4]

 .Exit:
  mov     sp, bp
  pop     bp
  ret 2

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
