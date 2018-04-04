
  ; RTL declarations
  %define SIZE_CODE      (8 * 1024)
  %define SIZE_HEAPDATA (54 * 1024)
  %define SIZE_STACK     (2 * 1024)

  %define SIZEOF_HEAPITEM 4
  %define HEAPITEM_SIG    0
  %define HEAPITEM_SIZE   2

  %define SIZEOF_PTRHDR 4
  %define PTRHDR_SIG    -4
  %define PTRHDR_SIZE   -2

  %define SIG_FREEBLOCK 0101010101010101b
  %define SIG_USEDBLOCK 1010101010101010b
  %define SIG_RSVDBLOCK 1101100110011011b

  %define ALLOC_MINSIZE (SIZEOF_HEAPITEM + 2)

  %define heapDataEnd (heapData + SIZE_HEAPDATA + SIZEOF_HEAPITEM)

  ; Origo declarations
  %define IDENT_MAXLEN    31
  %define STRING_MAXLEN   255

  %define SIZEOF_OPITEM   8
  %define OPITEM_OPERHIGH 0
  %define OPITEM_OPERLOW  2
  %define OPITEM_TOKEN    4
  %define OPTABLE_COUNT   ((opTableEnd - opTable) / SIZEOF_OPITEM)

  %define SIZEOF_KWITEM   4
  %define KWITEM_KWSTRING 0
  %define KWITEM_TOKEN    2
  %define KWTABLE_COUNT   ((keywordTableEnd - keywordTable) / SIZEOF_KWITEM)

  %define chSpace     (1 << 0)
  %define chPrint     (1 << 1)
  %define chAlpha     (1 << 2)
  %define chBin       (1 << 3)
  %define chOct       (1 << 4)
  %define chHex       (1 << 5)
  %define chDigit     (1 << 6)
  %define chIdent     (1 << 7)

  ; Token masks
  %define maskGroup    0xFF00
  %define maskSubToken 0x00FF

  ; Token groups
  %define tkGeneral  (0 << 8)
  %define rsvdIdent  (1 << 8)
  %define valLiteral (2 << 8)
  %define operSymbol (3 << 8)
  %define setSymbol  (4 << 8)
  %define x86Ident   (5 << 8)
  %define x86Reg16   (6 << 8)

  ; Operator precedence masks
  %define maskPrec    0x00F0
  %define maskOper    0x000F

  ; Operator precedence groups
  %define prec00      (0 << 4)
  %define prec01      (1 << 4)
  %define prec02      (2 << 4)
  %define prec03      (3 << 4)
  %define prec04      (4 << 4)
  %define prec05      (5 << 4)
  %define prec06      (6 << 4)
  %define prec07      (7 << 4)
  %define prec08      (8 << 4)
  %define prec09      (9 << 4)
  %define prec10     (10 << 4)
  %define prec11     (11 << 4)
  %define prec12     (12 << 4)
  %define prec13     (13 << 4)
  %define prec14     (14 << 4)
  %define prec15     (15 << 4)

  ; General tokens
  %define tkEOF          (tkGeneral + 1)
  %define tkEOL          (tkGeneral + 2)
  %define tkIdent        (tkGeneral + 3)
  %define tkDot          (tkGeneral + 4)
  %define tkDotDot       (tkGeneral + 5)
  %define tkLParen       (tkGeneral + 6)
  %define tkRParen       (tkGeneral + 7)
  %define tkLBrace       (tkGeneral + 8)
  %define tkRBrace       (tkGeneral + 9)
  %define tkLDoubleBrace (tkGeneral + 10)
  %define tkRDoubleBrace (tkGeneral + 11)
  %define tkComma        (tkGeneral + 12)
  %define tkColon        (tkGeneral + 13)
  %define tkSemicolon    (tkGeneral + 14)
  %define tkAt           (tkGeneral + 15)

  ; Reserved high level keyword tokens
  %define rsvdProgram    (rsvdIdent + 1)
  %define rsvdEnum       (rsvdIdent + 2)
  %define rsvdStruct     (rsvdIdent + 3)
  %define rsvdObject     (rsvdIdent + 4)
  %define rsvdPublic     (rsvdIdent + 5)
  %define rsvdVisible    (rsvdIdent + 6)
  %define rsvdInternal   (rsvdIdent + 7)
  %define rsvdExtends    (rsvdIdent + 8)
  %define rsvdPrivate    (rsvdIdent + 9)
  %define rsvdInterface  (rsvdIdent + 10)
  %define rsvdImplements (rsvdIdent + 11)
  %define rsvdMethod     (rsvdIdent + 12)
  %define rsvdSelf       (rsvdIdent + 13)
  %define rsvdFunc       (rsvdIdent + 14)
  %define rsvdAsm        (rsvdIdent + 15)
  %define rsvdNoRet      (rsvdIdent + 16)
  %define rsvdNoFrame    (rsvdIdent + 17)
  %define rsvdReturn     (rsvdIdent + 18)
  %define rsvdVar        (rsvdIdent + 19)
  %define rsvdRun        (rsvdIdent + 20)
  %define rsvdEnd        (rsvdIdent + 21)
  %define rsvdIf         (rsvdIdent + 22)
  %define rsvdThen       (rsvdIdent + 23)
  %define rsvdThenIf     (rsvdIdent + 24)
  %define rsvdElseIf     (rsvdIdent + 25)
  %define rsvdElse       (rsvdIdent + 26)
  %define rsvdEndIf      (rsvdIdent + 27)
  %define rsvdWhile      (rsvdIdent + 28)
  %define rsvdNext       (rsvdIdent + 29)
  %define rsvdBreak      (rsvdIdent + 30)
  %define rsvdEndWhile   (rsvdIdent + 31)
  %define rsvdIn         (rsvdIdent + 32)
  %define rsvdExit       (rsvdIdent + 33)

  ; Literal value tokens
  %define valFalse  (valLiteral + 0)
  %define valTrue   (valLiteral + 1)
  %define valUint   (valLiteral + 2)
  %define valString (valLiteral + 3)
  %define valChar   (valLiteral + 4)

  ; Operator tokens
  %define opPostInc  (operSymbol + prec01 + 0)
  %define opPostDec  (operSymbol + prec01 + 1)
  %define opPreInc   (operSymbol + prec02 + 0)
  %define opPreDec   (operSymbol + prec02 + 1)
  %define unaryNeg   (operSymbol + prec02 + 2)
  %define unaryIsNot (operSymbol + prec02 + 3)
  %define unaryNot   (operSymbol + prec02 + 4)
  %define opMul      (operSymbol + prec04 + 0)
  %define opDiv      (operSymbol + prec04 + 1)
  %define opMod      (operSymbol + prec04 + 2)
  %define opAdd      (operSymbol + prec05 + 0)
  %define opSub      (operSymbol + prec05 + 1)
  %define opShl      (operSymbol + prec06 + 0)
  %define opShr      (operSymbol + prec06 + 1)
  %define opSShr     (operSymbol + prec06 + 2)
  %define opRol      (operSymbol + prec06 + 3)
  %define opSRol     (operSymbol + prec06 + 4)
  %define opRor      (operSymbol + prec06 + 5)
  %define opSRor     (operSymbol + prec06 + 6)
  %define opLT       (operSymbol + prec08 + 0)
  %define opLTEq     (operSymbol + prec08 + 1)
  %define opGT       (operSymbol + prec08 + 2)
  %define opGTEq     (operSymbol + prec08 + 3)
  %define opEq       (operSymbol + prec09 + 0)
  %define opNotEq    (operSymbol + prec09 + 1)
  %define opAnd      (operSymbol + prec10 + 0)
  %define opXor      (operSymbol + prec11 + 0)
  %define opOr       (operSymbol + prec12 + 0)
  %define opAndIs    (operSymbol + prec13 + 0)
  %define opOrIs     (operSymbol + prec14 + 0)

  ; Assignment operators
  %define setTo   (setSymbol + prec15 + 0)
  %define setNot  (setSymbol + prec15 + 1)
  %define setAdd  (setSymbol + prec15 + 2)
  %define setSub  (setSymbol + prec15 + 3)
  %define setMul  (setSymbol + prec15 + 4)
  %define setDiv  (setSymbol + prec15 + 5)
  %define setMod  (setSymbol + prec15 + 6)
  %define setShl  (setSymbol + prec15 + 7)
  %define setShr  (setSymbol + prec15 + 8)
  %define setSShr (setSymbol + prec15 + 9)
  %define setRol  (setSymbol + prec15 + 10)
  %define setSRol (setSymbol + prec15 + 11)
  %define setRor  (setSymbol + prec15 + 12)
  %define setSRor (setSymbol + prec15 + 13)
  %define setAnd  (setSymbol + prec15 + 14)
  %define setXor  (setSymbol + prec15 + 15)
  %define setOr   (setSymbol + prec15 + 16)

  ; x86 assembler reserved word tokens
  %define x86Mov (x86Ident + 0)

  ; x86 16-bit register tokens
  %define x86RegAX (x86Reg16 + 0)
  %define x86RegCX (x86Reg16 + 1)
  %define x86RegDX (x86Reg16 + 2)
  %define x86RegBX (x86Reg16 + 3)
  %define x86RegSP (x86Reg16 + 4)
  %define x86RegBP (x86Reg16 + 5)
  %define x86RegSI (x86Reg16 + 6)
  %define x86RegDI (x86Reg16 + 7)

  %define x86ifInstPrefix     0
  %define x86ifAddrSizePrefix 1
  %define x86ifOperSizePrefix 2
  %define x86ifSegOverride    3
  %define x86ifOpcode1        4
  %define x86ifOpcode2        5
  %define x86ifModRM          6
  %define x86ifSIB            7
  %define x86ifDisp0          8
  %define x86ifDisp1          9
  %define x86ifDisp2          10
  %define x86ifDisp3          11
  %define x86ifImm0           12
  %define x86ifImm1           13
  %define x86ifImm2           14
  %define x86ifImm3           15

;
; macro InitRTL
;
%macro InitRTL 0
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
  mov     word [heapData + HEAPITEM_SIG],  SIG_FREEBLOCK
  mov     word [heapData + HEAPITEM_SIZE], SIZE_HEAPDATA

  ; Initialize command-line arguments
  mov     bx, arg
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
%endmacro
