;
; InitRTL
;
section .text
  cld

  ; Attempt mem resize to 64K
  mov     ah, 0x4A
  mov     bx, 65536 / 16
  int     0x21
  jnc     InitRTL_MemResized
  ; Not enough memory available
  push    minMemString
  call    echostring
  mov     ax, 0x4C00
  int     0x21
 InitRTL_MemResized:

  cli
  pop     ax
  mov     bx, 0xFFFF
  mov     sp, bx
  push    ax
  sti

  ; Initialize heap data to 0
  mov     di, heapData
  mov     cx, ((SIZE_HEAPDATA) / 2)
  xor     ax, ax
  rep stosw

  ;;; TODO: Fill argexe

  ; Initialize command-line arguments
  mov     bx, arg ; + 2
  mov     si, 129
  mov     di, argstr
  xor     cx, cx

  mov     al, [si]

 InitArgs_SkipSpace:
  cmp     al, 13
  je      InitArgs_Done
  cmp     al, 32
  ja      InitArgs_ExitSkipSpace
  mov     al, [si]
  inc     si
  jmp     InitArgs_SkipSpace
 InitArgs_ExitSkipSpace:

  mov     [bx], di
  add     bx, 2

 InitArgs_CopyLoop:
  cmp     al, 32
  jbe     InitArgs_ExitCopyLoop
  mov     [di], al
  inc     di
  mov     al, [si]
  inc     si
  jmp     InitArgs_CopyLoop
 InitArgs_ExitCopyLoop:

  mov     byte [di], 0
  inc     di
  inc     cx
  cmp     al, 13
  jne     InitArgs_SkipSpace

 InitArgs_Done:
  mov     [argc], cx

 InitArgs_Exit:
  jmp     run
