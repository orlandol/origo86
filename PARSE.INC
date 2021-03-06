section .text
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

  ; TODO: Generate COM initialization code

  ; TODO: Save current offset as run backpatch
  ; TODO: GenOpImm( x86Jmp, 0xFFFF ) => "jmp run"

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
  push    bp
  mov     bp, sp

  cmp     word [entryPoint], 0xFFFF
  je      .RunValid
  push    errDuplicateRun
  call    Error
 .RunValid:
  mov     word [entryPoint], 0 ; TODO: Replace 0 with codeOfs
  call    GetToken

  ; TODO: Backpatch offset of initial "jmp run"

 .StatementLoop:
  cmp     word [curToken], rsvdEnd
  jne     .NotEnd

  push    word x86Mov
  push    word x86RegAX
  push    word valUint
  push    word 0x4C00
  call    x86GenOpRegImm

  push    word x86Int
  push    word valUint8
  push    word 0x21
  call    x86GenOpImm
  jmp     .Done
 .NotEnd:

  push    strFuncStatement
  call    Expected

 .Done:
  call    GetToken

 .Exit:
  mov     sp, bp
  pop     bp
  ret

section .data
entryPoint: dw 0xFFFF
errDuplicateRun: db 'run already defined',0
strRunUndefined: db 'run undefined',0
