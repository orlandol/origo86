;
; func ParseRun()
;
ParseRun:
  cmp     word [entryPoint], 0xFFFF
  jne     .RunValid
  push    errDuplicateRun
  call    Error
 .RunValid:

  mov     word [entryPoint], 0 ; TODO: Replace 0 with codeOfs

  ; TODO: ParseStatement until rsvdEnd

  ret

entryPoint: dw 0xFFFF
errDuplicateRun: db 'run already defined',0
