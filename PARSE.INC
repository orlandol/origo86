;
; func Compile()
;
Compile:
  ; program IDENT => PROGNAME
  cmp     word [curToken], rsvdProgram
  je      .IsProgram
  push    strProgram
  call    Expected
 .IsProgram:
  call    GetToken

  cmp     word [curToken], tkIdent
  je      .IsProgramIdent
  push    strIdentifier
  call    Expected
 .IsProgramIdent:
  push    curIdent
  call    DeclareNamespace
  call    GetToken

  ; run, ...
 .ParseLoop:
  mov     ax, [curToken]

  cmp     ax, rsvdRun
  jne     .NotRun
  call    ParseRun
  jmp     .ParseLoop
 .NotRun:

  cmp     ax, tkEOF
  je      .DoneParse

  push    strTopLevel
  call    Expected

 .DoneParse:
  cmp     word [entryPoint], 0xFFFF
  jne     .Exit
  push    strRunUndefined
  call    Error

 .Exit:
  ret

;
; func ParseRun()
;
ParseRun:
  cmp     word [entryPoint], 0xFFFF
  je      .RunValid
  push    errDuplicateRun
  call    Error
 .RunValid:
  mov     word [entryPoint], 0 ; TODO: Replace 0 with codeOfs
  call    GetToken

 .StatementLoop:
  cmp     word [curToken], rsvdEnd
  je      .Done

  push    strFuncStatement
  call    Expected

 .Done:
  call    GetToken

 .Exit:
  ret

entryPoint: dw 0xFFFF
errDuplicateRun: db 'run already defined',0
strRunUndefined: db 'run undefined',0
