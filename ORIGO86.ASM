
  ORG 0x100

  %include "defs.inc"

  InitRTL

run:
  push    bp
  mov     bp, sp

  call    ParseOptions

  call    OpenRet
  call    CreateBinary

  call    Compile

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

  ; Validate parameter count
  cmp     word [argc], 1
  je      .SkipUsage
  push    eoln
  call    echostring
  call    PrintUsage
  mov     ax, 0x4C00
  int     0x21
 .SkipUsage:

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

  push    word [arg + 0]
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

  push    binaryName
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

%include "scan.inc"
%include "parse.inc"
%include "expr.inc"
%include "emit.inc"
%include "codegen.inc"
%include "symtab.inc"

bannerString:     db 'Origo86 Alpha',0
copyrightString:  db 'Copyright 2016 - 2018 Orlando Llanes',0
usageString:      db 'usage: origo86 source.ret',0

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

line:     dw 1
col:      dw 1

curLine:  dw 1
curCol:   dw 1

nextLine: dw 1
nextCol:  dw 1

binaryName: db "program.com",0

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
