
  ORG 0x100
  CPU 286

  %include "defs.inc"

  %include "initrtl.inc"

section .text
run:
  or      byte [chtable + 0x09], chSpace
  or      byte [chtable + 0x0A], chSpace
  or      byte [chtable + 0x0B], chSpace
  or      byte [chtable + 0x0C], chSpace
  or      byte [chtable + 0x0D], chSpace
  or      byte [chtable + 0x20], chSpace

  or      byte [chtable + 0x09], chPrint
  mov     bx, 0x20
 SetPrint:
  cmp     bx, 0x7E
  ja      ExitSetPrint
  or      byte [bx + chtable], chPrint
  inc     bx
  jmp     SetPrint
 ExitSetPrint:

  or      byte [chtable + '_'], chIdent
  mov     bx, 'A'
 SetAlpha1:
  cmp     bx, 'Z'
  ja      ExitSetAlpha1
  or      byte [bx + chtable], chAlpha | chIdent
  inc     bx
  jmp     SetAlpha1
 ExitSetAlpha1:

  mov     bx, 'a'
 SetAlpha2:
  cmp     bx, 'z'
  ja      ExitSetAlpha2
  or      byte [bx + chtable], chAlpha | chIdent
  inc     bx
  jmp     SetAlpha2
 ExitSetAlpha2:

  or      byte [chtable + '0'], chPrint | chBin | chOct | chHex | chDigit
  or      byte [chtable + '1'], chPrint | chBin | chOct | chHex | chDigit
  or      byte [chtable + '2'], chPrint | chOct | chHex | chDigit
  or      byte [chtable + '3'], chPrint | chOct | chHex | chDigit
  or      byte [chtable + '4'], chPrint | chOct | chHex | chDigit
  or      byte [chtable + '5'], chPrint | chOct | chHex | chDigit
  or      byte [chtable + '6'], chPrint | chOct | chHex | chDigit
  or      byte [chtable + '7'], chPrint | chOct | chHex | chDigit
  or      byte [chtable + '8'], chPrint | chHex | chDigit
  or      byte [chtable + '9'], chPrint | chHex | chDigit

  mov     bx, 'A'
 SetHexUpper:
  cmp     bx, 'F'
  ja      ExitSetHexUpper
  or      byte [bx + chtable], chHex
  inc     bx
  jmp     SetHexUpper
 ExitSetHexUpper:

  mov     bx, 'a'
 SetHexLower:
  cmp     bx, 'f'
  ja      ExitSetHexLower
  or      byte [bx + chtable], chHex
  inc     bx
  jmp     SetHexLower
 ExitSetHexLower:

  push    strSection
  call    echostring

  push    tableName
  call    echostring

 PrintLoop:
  push    prefix
  call    echostring

  mov     bx, chtable
  mov     ax, [chr]
  xlat
  mov     [code], ax

  cmp     word [code], 10
  jae     SkipPadding1
  push    word 0x20
  call    echochar
 SkipPadding1:

  cmp     word [code], 100
  jae     SkipPadding2
  push    word 0x20
  call    echochar
 SkipPadding2:

  push    word [code]
  call    echouint

  push    suffix
  call    echostring

  cmp     word [chr], 10
  jae     SkipPadding3
  push    word 0x20
  call    echochar
 SkipPadding3:

  cmp     word [chr], 100
  jae     SkipPadding4
  push    word 0x20
  call    echochar
 SkipPadding4:

  push    word [chr]
  call    echouint

  cmp     word [chr], 0x09
  jnz     SkipTab
  push    tab
  call    echostring
 SkipTab:

  cmp     word [chr], 0x0A
  jne     SkipLF
  push    lf
  call    echostring
 SkipLF:

  cmp     word [chr], 0x0B
  jne     SkipVT
  push    vt
  call    echostring
 SkipVT:

  cmp     word [chr], 0x0C
  jne     SkipFF
  push    ff
  call    echostring
 SkipFF:

  cmp     word [chr], 0x0D
  jne     SkipCR
  push    cr
  call    echostring
 SkipCR:

  cmp     word [chr], 0x20
  jne     SkipSpace
  push    space
  call    echostring
 SkipSpace:

  cmp     word [chr], 0x20
  jb      SkipChar
  cmp     word [chr], 0x7E
  ja      SkipChar
    push    word 0x20
    call    echochar
    push    word [chr]
    call    echochar
 SkipChar:

  push    eoln
  call    echostring

  inc     word [chr]

  dec     word [count]
  jnz     PrintLoop

  mov     ax, 0x4C00
  int     0x21

section .data
chr:    dw 0
code:   dw 0
count:  dw 256

strSection: db 'section .data',13,10,0
tableName: db 'chTable:',13,10,0
prefix:    db '  db ',0
suffix:    db ' ; ',0

tab:   db ' TAB',0   ; 9
lf:    db ' LF',0    ; 10
vt:    db ' VT',0    ; 11
ff:    db ' FF',0    ; 12
cr:    db ' CR',0    ; 13
space: db ' SPACE',0 ; 32

chtable: times 256 db 0

%include "rtl.inc"
