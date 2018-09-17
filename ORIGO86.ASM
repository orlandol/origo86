
  ORG 0x100
  CPU 286

  %include "defs.inc"

  %include "initrtl.inc"

section .text
run:
  push    bp
  mov     bp, sp

  %define count bp - 2
  push    word 0
        
  call    ParseOptions

  call    OpenRet
  call    CreateBinary

  call    Compile

push word x86Xor
push word mrX86RegBX | mrX86RegDI
push word 0
push word valInt8
push word 0x12
call x86GenOpMemImm

push word x86Xor
push word mrX86RegBX | mrX86RegDI
push word 0
push word valUint8
push word 0x12
call x86GenOpMemImm

push word x86Xor
push word mrX86RegBX | mrX86RegDI
push word 0
push word valInt
push word 0x1234
call x86GenOpMemImm

push word x86Xor
push word mrX86RegBX | mrX86RegDI
push word 0
push word valUint
push word 0x1234
call x86GenOpMemImm

  call    Cleanup

  mov     sp, bp
  pop     bp

  mov     ax, 0x4C00
  int     0x21

;
; func ParseOptions()
;
ParseOptions:
  push    bp
  mov     bp, sp

  %define extOfs bp - 2
  push    word 0

  ; Validate parameter count
  cmp     word [argc], 0
  ja      .SkipUsage
  push    eoln
  call    echostring
  call    PrintUsage
  mov     ax, 0x4C00
  int     0x21
 .SkipUsage:

  ; Parse source name
  mov     si, [arg + 0];2]
  mov     di, srcName
  xor     cx, cx

 .ParseSourceName:
  mov     al, [si]
  inc     si
  test    al, al
  jz      .DoneSourceName
  inc     cx
  mov     [di], al
  cmp     al, '.'
  jne     .SrcSkipDot
  mov     [extOfs], di
 .SrcSkipDot:
  inc     di
  jmp     .ParseSourceName
 .DoneSourceName:
  mov     byte [di], 0

  mov     di, [extOfs]
  test    di, di
  jnz     .SrcExtSpecified
  add     cx, srcName
  mov     di, cx
  mov     byte [di + 0], '.'
  mov     byte [di + 1], 'r'
  mov     byte [di + 2], 'e'
  mov     byte [di + 3], 't'
 .SrcExtSpecified:

  ; Parse binary name
  mov     si, srcName
  cmp     word [argc], 1
  jbe     .BinNameFromSrc
  mov     si, [arg + 2];4]
 .BinNameFromSrc:
  mov     di, binName
  xor     cx, cx

 .ParseBinaryName:
  mov     al, [si]
  inc     si
  test    al, al
  jz      .DoneBinaryName
  inc     cx
  mov     [di], al
  cmp     al, '.'
  jne     .BinSkipDot
  mov     [extOfs], di
 .BinSkipDot:
  inc     di
  jmp     .ParseBinaryName
 .DoneBinaryName:

  cmp     word [argc], 1
  ja      .BinExtSpecified
  mov     di, [extOfs]
  test    di, di
  jne     .ModifyBinExt
  add     cx, binName
  mov     di, cx
 .ModifyBinExt:
  mov     byte [di + 0], '.'
  mov     byte [di + 1], 'c'
  mov     byte [di + 2], 'o'
  mov     byte [di + 3], 'm'
 .BinExtSpecified:

 .Exit:
  mov     sp, bp
  pop     bp
  ret

;
; func PrintInfo()
;
PrintInfo:
  push    eoln
  call    echostring
  push    bannerString
  call    echostring
  push    eoln
  call    echostring
  push    copyrightString
  call    echostring
  push    eoln
  call    echostring
  push    eoln
  call    echostring
  ret

;
; func PrintUsage()
;
PrintUsage:
  push    usageString
  call    echostring
  push    eoln
  call    echostring
  ret

;
; func OpenRet()
;
OpenRet:
  call    CloseRet

  push    srcName
  call    fopen
  mov     [source], ax

  test    ax, ax
  jnz     .IsOpen
  push    errorSourceFile
  call    echostring
  mov     ax, 0x4C00
  int     0x21
 .IsOpen:

  ; Read character and lookahead character
  call    ReadChar
  call    ReadChar

  mov     word [line], 1
  mov     word [col], 1

  ; Get token and lookahead token
  call    GetToken
  call    GetToken

 .Exit:
  ret

;
; func CloseRet()
;
CloseRet:
  push    source
  call    fclose
  ret

;
; func CreateBinary()
;
CreateBinary:
  call    CloseBinary

  push    binName
  call    fcreate
  mov     [binary], ax
  ret

;
; func CloseBinary()
;
CloseBinary:
  push    bp
  mov     bp, sp

  push    binary
  call    fclose

  mov     sp, bp
  pop     bp
  ret

;
; func Cleanup()
;
Cleanup:
  call    CloseRet
  call    CloseBinary
  ret

;
; func Expected( string msg )
;
Expected:
  push    bp
  mov     bp, sp

  push    expectedString1
  call    echostring
  push    word [curLine]
  call    echouint
  push    expectedString2
  call    echostring
  push    word [curCol]
  call    echouint
  push    expectedString3
  call    echostring
  push    word [bp + 4]
  call    echostring
  push    eoln
  call    echostring

  mov     ax, 0x4C02
  int     0x21

  mov     sp, bp
  pop     bp
  ret 2

;
; func Error( string msg )
;
Error:
  push    bp
  mov     bp, sp

  push    errorString1
  call    echostring
  push    word [curLine]
  call    echouint
  push    errorString2
  call    echostring
  push    word [curCol]
  call    echouint
  push    errorString3
  call    echostring
  push    word [bp + 4]
  call    echostring
  push    eoln
  call    echostring

  mov     ax, 0x4C01
  int     0x21

  mov     sp, bp
  pop     bp
  ret 2

section .data
bannerString:     db 'Origo86 Alpha',0
copyrightString:  db 'Copyright 2016 - 2018 Orlando Llanes',0
usageString:      db 'usage: origo86 source[.ret] [binary[.ext]]',0

errorSourceFile:  db 'Unable to open source file',13,10,0

expectedString1:  db 'Expected[L',0
expectedString2:  db ',C',0
expectedString3:  db ']: ',0

errorString1:     db 'Error[L', 0
errorString2:     db ',C',0
errorString3:     db ']: ',0

strIdentifier:    db 'identifier',0
strTopLevel:      db 'run',0
strFuncStatement: db 'end'
strStatement:     db '',0

lineStr: db 'line: ', 0
colStr:  db 'col:  ', 0

srcName: times 128 + 4 + 1 db 0
binName: times 128 + 4 + 1 db 0

line:     dw 1
col:      dw 1

curLine:  dw 1
curCol:   dw 1

nextLine: dw 1
nextCol:  dw 1

source: dw 0
binary: dw 0

curCh:      dw 0
nextCh:     dw 0

curToken:   dw 0
curUint:    dw 0
curIdent:   times (IDENT_MAXLEN + 1) db 0
curString:  times (STRING_MAXLEN + 1) db 0

nextToken:  dw 0
nextUint:   dw 0
nextIdent:  times (IDENT_MAXLEN + 1) db 0
nextString: times (STRING_MAXLEN + 1) db 0

%include "chtable.inc"
%include "keywords.inc"
%include "optable.inc"

%include "debug.inc"
%include "rtl.inc"
                     
%include "scan.inc"
%include "parse.inc"
%include "expr.inc"
%include "codegen.inc"
%include "symtab.inc"
