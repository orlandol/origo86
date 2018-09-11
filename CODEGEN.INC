;
; func bool x86GenOp( uint op )
;
x86GenOp:
  push    bp
  mov     bp, sp

  %define op bp + 4

  %define instruction bp - 2
  push    word 0

  mov     si, [op]
  mov     dx, si
  and     si, maskSubToken
  and     dx, maskGroup

  xor     ax, ax

  cmp     dx, x86Ident
  jne     .Exit

  cmp     si, 19
  ja      .Exit

  mov     al, [si + x86SingleByteOpTable]

  lea     bx, [instruction]

  mov     [bx + 0], al

  push    word [binary]
  push    bx
  push    word 1
  call    fwrite

  cmp     ax, 1
  je      .Exit
  xor     ax, ax

 .Exit:
  mov     sp, bp
  pop     bp
  ret 2

x86SingleByteOpTable:
  db 0b1001_1000 ; x86Cbw
  db 0b1001_1001 ; x86Cwd
  db 0b1111_1000 ; x86Clc
  db 0b1111_1100 ; x86Cld
  db 0b1111_1010 ; x86Cli
  db 0b1111_0101 ; x86Cmc
  db 0b1100_1100 ; x86Int3
  db 0b1100_1111 ; x86Iret
  db 0b1001_1111 ; x86Lahf
  db 0b1001_0000 ; x86Nop
  db 0b1001_1101 ; x86Popf
  db 0b1001_1100 ; x86Pushf
  db 0b1100_0011 ; x86Ret
  db 0b1100_1011 ; x86Retf
  db 0b1001_1110 ; x86Sahf
  db 0b1111_1001 ; x86Stc
  db 0b1111_1101 ; x86Std
  db 0b1111_1011 ; x86Sti
  db 0b1101_0111 ; x86Xlat
  db 0b1101_0111 ; x86Xlatb

;
; func bool x86GenRepOp( uint repOp, uint op )
;
x86GenRepOp:
  push    bp
  mov     bp, sp

  %define repOp bp + 6
  %define op    bp + 4

  %define instruction  bp - 2
  push    word 0

  mov     bx, [repOp]
  mov     si, [op]

  xor     ax, ax

  ; rep op
  cmp     bx, x86Rep
  jne     .NotRep
  mov     al, 0b1111_0011

  mov     ah, 0b1010_1100
  cmp     si, x86Lodsb
  je      .Emit
  inc     ah
  cmp     si, x86Lodsw
  je      .Emit

  mov     ah, 0b1010_0100
  cmp     si, x86Movsb
  je      .Emit
  inc     ah
  cmp     si, x86Movsw
  je      .Emit

  mov     ah, 0b1010_1010
  cmp     si, x86Stosb
  je      .Emit
  inc     ah
  cmp     si, x86Stosw
  je      .Emit

  xor     ax, ax
  jmp     .Exit
 .NotRep:

  ; repe/repz op
  cmp     bx, x86Repe
  je      .IsRepe
  cmp     bx, x86Repz
  jne     .NotRepe
 .IsRepe:
  mov     al, 0b1111_0011

  mov     ah, 0b1010_0110
  cmp     si, x86Cmpsb
  je      .Emit
  inc     ah
  cmp     si, x86Cmpsw
  je      .Emit

  mov     ah, 0b1010_1110
  cmp     si, x86Scasb
  je      .Emit
  inc     ah
  cmp     si, x86Scasw
  je      .Emit

  xor     ax, ax
  jmp     .Exit
 .NotRepe:

  ; repne/repnz op
  cmp     bx, x86Repne
  je      .IsRepne
  cmp     bx, x86Repnz
  jne     .NotRepne
 .IsRepne:
  mov     al, 0b1111_0010

  mov     ah, 0b1010_0110
  cmp     si, x86Cmpsb
  je      .Emit
  inc     ah
  cmp     si, x86Cmpsw
  je      .Emit

  mov     ah, 0b1010_1110
  cmp     si, x86Scasb
  je      .Emit
  inc     ah
  cmp     si, x86Scasw
  je      .Emit

  xor     ax, ax
  jmp     .Exit
 .NotRepne:

 .Emit:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     cx, 2

 .EmitInstruction:
  push    cx
  push    word [binary]
  push    bx
  push    cx
  call    fwrite
  pop     cx

  cmp     ax, cx
  je      .Done
  xor     ax, ax
  jmp     .Exit
 .Done:

  mov     ax, 1

 .Exit:
  mov     sp, bp
  pop     bp
  ret 4

;
; func bool x86GenOpImm( uint op, immType, imm )
;
x86GenOpImm:
  push    bp
  mov     bp, sp

  %define op          bp + 8
  %define immType     bp + 6
  %define imm         bp + 4

  %define instruction bp - 4
  push    word 0
  push    word 0

  mov     bx, [op]
  mov     di, [immType]
  mov     ax, [imm]

  cmp     di, valInt8
  jne     .NotUint8
  cbw
  jmp     .ImmInitialized
 .NotUint8:

  cmp     di, valUint8
  jne     .NotInt8
  xor     ah, ah
 .NotInt8:

 .ImmInitialized:

  mov     dx, ax

  cmp     bx, x86Int
  jne     .NotX86Int
  mov     al, 0b1100_1101
  jmp     .Emit8
 .NotX86Int:

  cmp     bx, x86JCC
  jb      .NotJcc
  cmp     bx, x86JCC + 15
  ja      .NotJcc
  mov     ax, bx
  sub     ax, x86JCC
  add     al, 0b0111_0000
  cmp     di, valInt8
  je      .Emit8
  cmp     di, valUint8
  je      .Emit8
  sub     al, 0b0111_0000
  mov     ah, al
  add     ah, 0b1000_0000
  mov     al, 0b0000_1111
  cmp     di, valInt
  je      .EmitOp16Imm16
  cmp     di, valUint
  je      .EmitOp16Imm16
  xor     ax, ax
  jmp     .Exit
 .NotJcc:

  cmp     bx, x86Call
  jne     .NotCall
  mov     al, 0b1110_1000
  jmp     .Emit16
 .NotCall:

  cmp     bx, x86Jcxz
  jne     .NotJcxz
  mov     al, 0b1110_0011
  jmp     .Emit8
 .NotJcxz:

  cmp     bx, x86Jmp
  jne     .NotJmp
  mov     al, 0b1110_1011
  cmp     di, valUint8
  je     .Emit8
  cmp     di, valInt8
  je     .Emit8
  mov     al, 0b1110_1001
  cmp     di, valUint
  je     .Emit16
  cmp     di, valInt
  je     .Emit16
  xor     ax, ax
  jmp     .Exit
 .NotJmp:

  cmp     bx, x86Loop
  jne     .NotLoop
  mov     al, 0b1110_0010
  jmp     .Emit8
 .NotLoop:

  cmp     bx, x86Loopz
  je      .IsLoopz
  cmp     bx, x86Loope
  jne     .NotLoope
 .IsLoopz:
  mov     al, 0b1110_0001
  jmp     .Emit8
 .NotLoope:

  cmp     bx, x86Loopnz
  je      .IsLoopnz
  cmp     bx, x86Loopne
  jne     .NotLoopne
 .IsLoopnz:
  mov     al, 0b1110_0000
  jmp     .Emit8
 .NotLoopne:

  cmp     bx, x86Push
  jne     .NotPush
  mov     al, 0b0110_1010
  cmp     di, valInt8
  je     .Emit8
  mov     al, 0b0110_1000
  cmp     di, valUint8
  je     .Emit16
  cmp     di, valUint
  je     .Emit16
  cmp     di, valInt
  je     .Emit16
  xor     ax, ax
  jmp     .Exit
 .NotPush:

  cmp     bx, x86Ret
  jne     .NotRet
  mov     al, 0b1100_0010
  jmp     .Emit16
 .NotRet:

  cmp     bx, x86Retf
  jne     .NotRetf
  mov     al, 0b1100_1010
  jmp     .Emit16
 .NotRetf:

  xor     ax, ax
  jmp     .Exit

 .Emit8:
  lea     bx, [instruction]
  mov     [bx + 0], al
  mov     [bx + 1], dl
  mov     cx, 2
  jmp     .EmitInstruction

 .Emit16:
  lea     bx, [instruction]
  mov     [bx + 0], al
  mov     [bx + 1], dx
  mov     cx, 3
  jmp     .EmitInstruction

 .EmitOp16Imm16:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dx
  mov     cx, 4
  jmp     .EmitInstruction

 .EmitInstruction:
  push    cx
  push    word [binary]
  push    bx
  push    cx
  call    fwrite
  pop     cx

  cmp     ax, cx
  je      .Done
  xor     ax, ax
  jmp     .Exit
 .Done:

  mov     ax, 1

 .Exit:
  mov     sp, bp
  pop     bp
  ret 6

;
; func bool x86GenOpSelOfs( uint op, sel, ofs )
;
x86GenOpSelOfs:
  push    bp
  mov     bp, sp

  %define op           bp + 8
  %define sel          bp + 6
  %define ofs          bp + 4

  %define instruction  bp - 6
  push    word 0
  push    word 0
  push    word 0

  mov     bx, [op]
  mov     si, [sel]
  mov     di, [ofs]

  cmp     bx, x86Call
  jne     .NotCall
  mov     al, 0b1001_1010
  jmp     .Emit
 .NotCall:

  cmp     bx, x86Jmp
  jne     .NotJmp
  mov     al, 0b1110_1010
  jmp     .Emit
 .NotJmp:

  xor     ax, ax
  jmp     .Exit

 .Emit:
  lea     bx, [instruction]
  mov     [bx + 0], al
  mov     [bx + 1], di
  mov     [bx + 3], si
  mov     cx, 5

 .EmitInstruction:
  push    cx
  push    word [binary]
  push    bx
  push    cx
  call    fwrite
  pop     cx

  cmp     ax, cx
  je      .Done
  xor     ax, ax
  jmp     .Exit
 .Done:

  mov     ax, 1

 .Exit:
  mov     sp, bp
  pop     bp
  ret 6

;
; func bool x86GenOpReg( uint op, reg )
;
x86GenOpReg:
  push    bp
  mov     bp, sp

  %define op           bp + 6
  %define reg          bp + 4

  %define instruction  bp - 2
  push    word 0

  mov     bx, [op]
  mov     si, [reg]

  mov     cx, si
  and     si, maskGroup
  and     cx, maskSubToken

  cmp     bx, x86Call
  jne     .NotCall
  mov     al, 0b1111_1111
  mov     ah, 0b11_010_000
  add     ah, cl
  cmp     si, x86Reg16
  je      .Emit16
  xor     ax, ax
  jmp     .Exit
 .NotCall:

  cmp     bx, x86Not
  jne     .NotNot
  mov     al, 0b1111_0110
  add     cl, 0b11_010_000
  mov     ah, cl
  jmp     .ProcessDest
 .NotNot:

  cmp     bx, x86Dec
  jne     .NotDec
  mov     al, 0b1111_1110
  add     cl, 0b11_001_000
  mov     ah, cl
  jmp     .ProcessDest
 .NotDec:

  cmp     bx, x86Div
  jne     .NotDiv
  mov     al, 0b1111_0110
  add     cl, 0b11_110_000
  mov     ah, cl
  jmp     .ProcessDest
 .NotDiv:

  cmp     bx, x86IDiv
  jne     .NotIDiv
  mov     al, 0b1111_0110
  add     cl, 0b11_111_000
  mov     ah, cl
  jmp     .ProcessDest
 .NotIDiv:

  cmp     bx, x86IMul
  jne     .NotIMul
  mov     al, 0b1111_0110
  add     cl, 0b11_101_000
  mov     ah, cl
  jmp     .ProcessDest
 .NotIMul:

  cmp     bx, x86Inc
  jne     .NotInc
  mov     al, 0b1111_1110
  add     cl, 0b11_000_000
  mov     ah, cl
  jmp     .ProcessDest
 .NotInc:

  cmp     bx, x86Mul
  jne     .NotMul
  mov     al, 0b1111_0110
  add     cl, 0b11_100_000
  mov     ah, cl
  jmp     .ProcessDest
 .NotMul:

  cmp     bx, x86Jmp
  jne     .NotJmp
  mov     al, 0b1111_1111
  add     cl, 0b11_100_000
  mov     ah, cl
  cmp     si, x86Reg16
  je      .Emit16
  xor     ax, ax
  jmp     .Exit
 .NotJmp:

  cmp     bx, x86Neg
  jne     .NotNeg
  mov     al, 0b1111_0110
  add     cl, 0b11_011_000
  mov     ah, cl
  jmp     .ProcessDest
 .NotNeg:

  cmp     bx, x86Pop
  jne     .NotPop
  mov     al, 0b0101_1_000
  add     al, cl
  cmp     si, x86Reg16
  je      .Emit8
  mov     al, 0b000_000_111
  shl     cl, 3
  add     al, cl
  cmp     si, x86SReg
  je      .Emit8
  xor     ax, ax
  jmp     .Exit
 .NotPop:

  cmp     bx, x86Push
  jne     .NotPush
  mov     al, 0b0101_0_000
  add     al, cl
  cmp     si, x86Reg16
  je      .Emit8
  mov     al, 0b000_000_110
  shl     cl, 3
  add     al, cl
  cmp     si, x86SReg
  je      .Emit8
  xor     ax, ax
  jmp     .Exit
 .NotPush:

  xor     ax, ax
  jmp     .Exit

 .ProcessDest:
  cmp     si, x86Reg8
  je      .Emit16
  inc     al
  cmp     si, x86Reg16
  je      .Emit16

  xor     ax, ax
  jmp     .Exit

 .Emit8:
  lea     bx, [instruction]
  mov     [bx + 0], al
  mov     cx, 1
  jmp     .EmitInstruction

 .Emit16:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     cx, 2

 .EmitInstruction:
  push    cx
  push    word [binary]
  push    bx
  push    cx
  call    fwrite
  pop     cx

  cmp     ax, cx
  je      .Done
  xor     ax, ax
  jmp     .Exit
 .Done:

  mov     ax, 1

 .Exit:
  mov     sp, bp
  pop     bp
  ret 4

;
; func bool x86GenOpMem( uint op, uint memSize,
;   uint memRegs, int displacement )
;
x86GenOpMem:
  push    bp
  mov     bp, sp

  %define op           bp + 10
  %define memSize      bp + 8
  %define memRegs      bp + 6
  %define displacement bp + 4

  %define instruction  bp - 4
  push    word 0
  push    word 0

  mov     bx, [op]
  mov     si, [memRegs]
  mov     dx, [displacement]

  cmp     bx, x86Not
  jne     .NotNot
  mov     al, 0b1111_0110
  mov     ah, 0b00_010_000
  jmp     .ProcessDest
 .NotNot:

  cmp     bx, x86Call
  jne     .NotCall
  mov     al, 0b1111_1111
  mov     ah, 0b00_010_000
  cmp     word [memSize], 2
  je      .ProcessRM
  mov     ah, 0b00_011_000
  cmp     word [memSize], 4
  je      .ProcessRM
  xor     ax, ax
  jmp     .Exit
 .NotCall:

  cmp     bx, x86Jmp
  jne     .NotJmp
  mov     al, 0b1111_1111
  mov     ah, 0b00_100_000
  cmp     word [memSize], 2
  je      .ProcessRM
  mov     ah, 0b00_101_000
  cmp     word [memSize], 4
  je      .ProcessRM
  xor     ax, ax
  jmp     .Exit
 .NotJmp:

  cmp     bx, x86Dec
  jne     .NotDec
  mov     al, 0b1111_1110
  mov     ah, 0b00_001_000
  jmp     .ProcessDest
 .NotDec:

  cmp     bx, x86Div
  jne     .NotDiv
  mov     al, 0b1111_0110
  mov     ah, 0b00_110_000
  jmp     .ProcessDest
 .NotDiv:

  cmp     bx, x86IDiv
  jne     .NotIDiv
  mov     al, 0b1111_0110
  mov     ah, 0b00_111_000
  jmp     .ProcessDest
 .NotIDiv:

  cmp     bx, x86IMul
  jne     .NotIMul
  mov     al, 0b1111_0110
  mov     ah, 0b00_101_000
  jmp     .ProcessDest
 .NotIMul:

  cmp     bx, x86Inc
  jne     .NotInc
  mov     al, 0b1111_1110
  mov     ah, 0b00_000_000
  jmp     .ProcessDest
 .NotInc:

  cmp     bx, x86Mul
  jne     .NotMul
  mov     al, 0b1111_0110
  mov     ah, 0b00_100_000
  jmp     .ProcessDest
 .NotMul:

  cmp     bx, x86Neg
  jne     .NotNeg
  mov     al, 0b1111_0110
  mov     ah, 0b00_011_000
  jmp     .ProcessDest
 .NotNeg:

  cmp     bx, x86Pop
  jne     .NotPop
  mov     al, 0b1000_1111
  mov     ah, 0b00_000_000
  cmp     word [memSize], 2
  je      .ProcessRM
  xor     ax, ax
  jmp     .Exit
 .NotPop:

  cmp     bx, x86Push
  jne     .NotPush
  mov     al, 0b1111_1111
  mov     ah, 0b00_110_000
  cmp     word [memSize], 2
  je      .ProcessRM
  xor     ax, ax
  jmp     .Exit
 .NotPush:

  xor     ax, ax
  jmp     .Exit

 .ProcessDest:
  cmp     word [memSize], 1
  je      .ProcessRM
  inc     al
  cmp     word [memSize], 2
  je      .ProcessRM

  xor     ax, ax
  jmp     .Exit

 .ProcessRM:
  test    si, si
  jnz     .NotDisp16
  or      ah, 0b00_000_110
  jmp     .Emit16
 .NotDisp16:

  ; Reserved bits must be 0
  cmp     si, (mrX86RegBX | mrX86RegBP | mrX86RegSI | mrX86RegDI)
  ja      .Error

  ; Get ModR/M encoding for MOD 00 as the default
  mov     cl, [si + x86RMTable]
  cmp     cl, 0b00_111_000
  je      .Error
  or      ah, cl
  cmp     si, mrX86RegBP
  je      .SkipDisp0
  test    dx, dx
  jz      .Emit
 .SkipDisp0:

  add     ah, 0b01_000_000
  cmp     dx, -128
  jl      .NotDisp8
  cmp     dx, 127
  jg      .NotDisp8
  jmp     .Emit8
 .NotDisp8:

  add     ah, 0b01_000_000
  jmp     .Emit16

 .Emit:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     cx, 2
  jmp     .EmitInstruction

 .Emit8:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dl
  mov     cx, 3
  jmp     .EmitInstruction

 .Emit16:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dx
  mov     cx, 4
  jmp     .EmitInstruction

 .EmitInstruction:
  push    cx
  push    word [binary]
  push    bx
  push    cx
  call    fwrite
  pop     cx

  cmp     ax, cx
  je      .Done
 .Error:
  xor     ax, ax
  jmp     .Exit
 .Done:

  mov     ax, 1

 .Exit:
  mov     sp, bp
  pop     bp
  ret 8

x86RMTable:
  db 0b00_111_000 ;  0 - Invalid
  db 0b00_000_101 ;  1 - [DI]
  db 0b00_000_100 ;  2 - [SI]
  db 0b00_111_000 ;  3 - Invalid
  db 0b00_000_110 ;  4 - [BP + <DISP8 | DISP16>] if not [DISP16]
  db 0b00_000_011 ;  5 - [BP + DI]
  db 0b00_000_010 ;  6 - [BP + SI]
  db 0b00_111_000 ;  7 - Invalid
  db 0b00_000_111 ;  8 - [BX]
  db 0b00_000_001 ;  9 - [BX + DI]
  db 0b00_000_000 ; 10 - [BX + SI]
  db 0b00_111_000 ; 11 - Invalid
  db 0b00_111_000 ; 12 - Invalid
  db 0b00_111_000 ; 13 - Invalid
  db 0b00_111_000 ; 14 - Invalid
  db 0b00_111_000 ; 15 - Invalid

;
; func bool x86GenOpRegImm( uint op, uint destReg, uint immType, uint imm )
;
x86GenOpRegImm:
  push    bp
  mov     bp, sp

  %define op          bp + 10
  %define destReg     bp + 8
  %define immType     bp + 6
  %define imm         bp + 4

  %define instruction bp - 4
  push    word 0
  push    word 0

  mov     bx, [op]
  mov     si, [destReg]
  mov     di, [immType]
  mov     ax, [imm]

  cmp     di, valInt8
  jne     .NotUint8
  cbw
  jmp     .ImmInitialized
 .NotUint8:

  cmp     di, valUint8
  jne     .NotInt8
  xor     ah, ah
 .NotInt8:

 .ImmInitialized:

  mov     dx, ax

  mov     cx, si
  and     si, maskGroup
  and     cx, maskSubToken

  cmp     bx, x86Mov
  jne     .NotMov
  mov     ah, 0b11_000_000 ; ModReg
  mov     al, 0b1100_0110 ; Opcode
  add     ah, cl
  jmp     .ProcessDest
 .NotMov:

  cmp     bx, x86Adc
  jne     .NotAdc
  mov     ah, 0b11_010_000
  mov     al, 0b1000_0000
  add     ah, cl
  cmp     si, x86Reg8
  je      .ProcessDest
  cmp     di, valInt8
  jne     .ProcessDest
  mov     al, 0b1000_0011
  jmp     .ProcessSrcInt8
 .NotAdc:

  cmp     bx, x86Add
  jne     .NotAdd
  mov     ah, 0b11_000_000
  mov     al, 0b1000_0000
  add     ah, cl
  cmp     si, x86Reg8
  je      .ProcessDest
  cmp     di, valInt8
  jne     .ProcessDest
  mov     al, 0b1000_0011
  jmp     .ProcessSrcInt8
 .NotAdd:

  cmp     bx, x86And
  jne     .NotAnd
  mov     ah, 0b11_100_000
  mov     al, 0b1000_0000
  add     ah, cl
  cmp     si, x86Reg8
  je      .ProcessDest
  cmp     di, valInt8
  jne     .ProcessDest
  mov     al, 0b1000_0011
  jmp     .ProcessSrcInt8
 .NotAnd:

  cmp     bx, x86Cmp
  jne     .NotCmp
  mov     ah, 0b11_111_000
  mov     al, 0b1000_0000
  add     ah, cl
  cmp     si, x86Reg8
  je      .ProcessDest
  cmp     di, valInt8
  jne     .ProcessDest
  mov     al, 0b1000_0011
  jmp     .ProcessSrcInt8
 .NotCmp:

  cmp     bx, x86Or
  jne     .NotOr
  mov     ah, 0b11_001_000
  mov     al, 0b1000_0000
  add     ah, cl
  cmp     si, x86Reg8
  je      .ProcessDest
  cmp     di, valInt8
  jne     .ProcessDest
  mov     al, 0b1000_0011
  jmp     .ProcessSrcInt8
 .NotOr:

  cmp     bx, x86Sbb
  jne     .NotSbb
  mov     ah, 0b11_011_000
  mov     al, 0b1000_0000
  add     ah, cl
  cmp     si, x86Reg8
  je      .ProcessDest
  cmp     di, valInt8
  jne     .ProcessDest
  mov     al, 0b1000_0011
  jmp     .ProcessSrcInt8
 .NotSbb:

  cmp     bx, x86Sub
  jne     .NotSub
  mov     ah, 0b11_101_000
  mov     al, 0b1000_0000
  add     ah, cl
  cmp     si, x86Reg8
  je      .ProcessDest
  cmp     di, valInt8
  jne     .ProcessDest
  mov     al, 0b1000_0011
  jmp     .ProcessSrcInt8
 .NotSub:

  cmp     bx, x86Xor
  jne     .NotXor
  mov     ah, 0b11_110_000
  mov     al, 0b1000_0000
  add     ah, cl
  cmp     si, x86Reg8
  je      .ProcessDest
  cmp     di, valInt8
  jne     .ProcessDest
  mov     al, 0b1000_0011
  jmp     .ProcessSrcInt8
 .NotXor:

  cmp     bx, x86Test
  jne     .NotTest
  mov     ah, 0b11_000_000
  mov     al, 0b1111_0110
  add     ah, cl
  jmp     .ProcessDest
 .NotTest:

  cmp     bx, x86In
  jne     .NotIn
  mov     al, 0b1110_0100
  cmp     word [destReg], x86RegAL
  je      .EmitOp8Imm8
  mov     al, 0b1110_0101
  cmp     word [destReg], x86RegAX
  je      .EmitOp8Imm8
  xor     ax, ax
  jmp     .Exit
 .NotIn:

  cmp     bx, x86Out
  jne     .NotOut
  mov     al, 0b1110_0110
  cmp     word [destReg], x86RegAL
  je      .EmitOp8Imm8
  mov     al, 0b1110_0111
  cmp     word [destReg], x86RegAX
  je      .EmitOp8Imm8
  xor     ax, ax
  jmp     .Exit
 .NotOut:

  cmp     bx, x86Rcl
  jne     .NotRcl
  mov     ah, 0b11_010_000
  mov     al, 0b1100_0000
  add     ah, cl
  jmp     .ProcessOp16Imm8
 .NotRcl:

  cmp     bx, x86Rcr
  jne     .NotRcr
  mov     ah, 0b11_011_000
  mov     al, 0b1100_0000
  add     ah, cl
  jmp     .ProcessOp16Imm8
 .NotRcr:

  cmp     bx, x86Rol
  jne     .NotRol
  mov     ah, 0b1100_0000
  mov     al, 0b11_000_000
  add     ah, cl
  jmp     .ProcessOp16Imm8
 .NotRol:

  cmp     bx, x86Ror
  jne     .NotRor
  mov     ah, 0b11_001_000
  mov     al, 0b1100_0000
  add     ah, cl
  jmp     .ProcessOp16Imm8
 .NotRor:

  cmp     bx, x86Sar
  jne     .NotSar
  mov     ah, 0b11_111_000
  mov     al, 0b1100_0000
  add     ah, cl
  jmp     .ProcessOp16Imm8
 .NotSar:

  cmp     bx, x86Sal
  je      .IsSal
  cmp     bx, x86Shl
  jne     .NotShl
 .IsSal:
  mov     ah, 0b11_100_000
  mov     al, 0b1100_0000
  add     ah, cl
  jmp     .ProcessOp16Imm8
 .NotShl:

  cmp     bx, x86Shr
  jne     .NotShr
  mov     ah, 0b11_101_000
  mov     al, 0b1100_0000
  add     ah, cl
  jmp     .ProcessOp16Imm8
 .NotShr:

  xor     ax, ax
  jmp     .Exit

 .ProcessSrcInt8:
  cmp     si, x86Reg16
  je      .Emit8
  xor     ax, ax
  jmp     .Exit

 .ProcessOp16Imm8:
  cmp     si, x86Reg8
  je      .Emit8
  inc     al
  cmp     si, x86Reg16
  je      .EmitOp16Imm8

 .ProcessDest:
  cmp     si, x86Reg8
  je      .Emit8
  inc     al
  cmp     si, x86Reg16
  je      .Emit16

  xor     ax, ax
  jmp     .Exit

 .EmitOp8Imm8:
  lea     bx, [instruction]
  mov     [bx + 0], al
  mov     [bx + 1], dl
  mov     cx, 2
  jmp     .EmitInstruction

 .EmitOp16Imm8:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dl
  mov     cx, 3
  jmp     .EmitInstruction

 .Emit8:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dl
  mov     cx, 3
  jmp     .EmitInstruction

 .Emit16:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dx
  mov     cx, 4

 .EmitInstruction:
  push    cx
  push    word [binary]
  push    bx
  push    cx
  call    fwrite
  pop     cx

  cmp     ax, cx
  je      .Done
  xor     ax, ax
  jmp     .Exit
 .Done:

  mov     ax, 1

 .Exit:
  mov     sp, bp
  pop     bp
  ret 8

;
; func bool x86GenOpRegReg( uint op, uint destReg, uint srcReg )
;
x86GenOpRegReg:
  push    bp
  mov     bp, sp

  %define op          bp + 8
  %define destReg     bp + 6
  %define srcReg      bp + 4

  %define instruction bp - 2
  push    word 0

  mov     bx, [op]
  xor     ax, ax

  mov     cx, [destReg]
  mov     si, cx
  and     cx, maskSubToken
  and     si, maskGroup

  mov     dx, [srcReg]
  mov     di, dx
  and     dx, maskSubToken
  and     di, maskGroup

  cmp     si, di
  jne     .RegTypesDiffer

  shl     dl, 3
  or      cl, dl

  cmp     bx, x86Mov
  jne     .NotMov
  mov     al, 0b1000_1000
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .ProcessSrc
 .NotMov:

  cmp     bx, x86Adc
  jne     .NotAdc
  mov     al, 0b0001_0000
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .ProcessSrc
 .NotAdc:

  cmp     bx, x86Add
  jne     .NotAdd
  mov     al, 0b0000_0000
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .ProcessSrc
 .NotAdd:

  cmp     bx, x86And
  jne     .NotAnd
  mov     al, 0b0010_0000
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .ProcessSrc
 .NotAnd:

  cmp     bx, x86Cmp
  jne     .NotCmp
  mov     al, 0b0011_1000
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .ProcessSrc
 .NotCmp:

  cmp     bx, x86Or
  jne     .NotOr
  mov     al, 0b0000_1000
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .ProcessSrc
 .NotOr:

  cmp     bx, x86Sbb
  jne     .NotSbb
  mov     al, 0b0001_1000
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .ProcessSrc
 .NotSbb:

  cmp     bx, x86Sub
  jne     .NotSub
  mov     al, 0b0010_1000
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .ProcessSrc
 .NotSub:

  cmp     bx, x86Test
  jne     .NotTest
  mov     al, 0b1000_0100
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .ProcessSrc
 .NotTest:

  cmp     bx, x86Xchg
  jne     .NotXchg
  mov     al, 0b1000_0110
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .ProcessSrc
 .NotXchg:

  cmp     bx, x86Xor
  jne     .NotXor
  mov     al, 0b0011_0000
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .ProcessSrc
 .NotXor:

 .RegTypesDiffer:

  xor     ax, ax

  cmp     bx, x86Mov
  jne     .NotMovSReg
  ; mov sreg, reg16
  cmp     si, x86SReg
  jne     .NotMovSRegDest
  cmp     di, x86Reg16
  jne     .Exit
  shl     cl, 3
  or      cl, dl
  mov     al, 0b1000_1110
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .Emit
 .NotMovSRegDest:
  ; mov reg16, sreg
  cmp     si, x86Reg16
  jne     .Exit
  cmp     di, x86SReg
  jne     .Exit
  shl     dl, 3
  or      cl, dl
  mov     al, 0b1000_1100
  mov     ah, cl
  or      ah, 0b11_000_000
  jmp     .Emit
 .NotMovSReg:

  cmp     bx, x86In
  jne     .NotIn
  mov     si, [destReg]
  mov     di, [srcReg]
  ; in al, dx
  cmp     si, x86RegAL
  jne     .NotInRegAL
  cmp     di, x86RegDX
  jne     .Exit
  mov     al, 0b1110_1100
  jmp     .EmitOp8
 .NotInRegAL:
  ; in ax, dx
  cmp     si, x86RegAX
  jne     .Exit
  cmp     di, x86RegDX
  jne     .Exit
  mov     al, 0b1110_1101
  jmp     .EmitOp8
 .NotIn:

  cmp     bx, x86Out
  jne     .NotOut
  mov     si, [destReg]
  mov     di, [srcReg]
  ; out al, dx
  cmp     si, x86RegAL
  jne     .NotOutRegAL
  cmp     di, x86RegDX
  jne     .Exit
  mov     al, 0b1110_1110
  jmp     .EmitOp8
 .NotOutRegAL:
  ; out ax, dx
  cmp     si, x86RegAX
  jne     .Exit
  cmp     di, x86RegDX
  jne     .Exit
  mov     al, 0b1110_1111
  jmp     .EmitOp8
 .NotOut:

  cmp     word [srcReg], x86RegCL
  jne     .NotSrcCL

  mov     cx, [destReg]
  and     cx, maskSubToken

  mov     al, 0b1101_0010
  cmp     bx, x86Rcl
  jne     .NotRcl
  mov     ah, 0b11_010_000
  add     ah, cl
  jmp     .ProcessSrc
 .NotRcl:

  cmp     bx, x86Rcr
  jne     .NotRcr
  mov     ah, 0b11_011_000
  add     ah, cl
  jmp     .ProcessSrc
 .NotRcr:

  cmp     bx, x86Rol
  jne     .NotRol
  mov     ah, 0b11_000_000
  add     ah, cl
  jmp     .ProcessSrc
 .NotRol:

  cmp     bx, x86Ror
  jne     .NotRor
  mov     ah, 0b11_001_000
  add     ah, cl
  jmp     .ProcessSrc
 .NotRor:

  cmp     bx, x86Sar
  jne     .NotSar
  mov     ah, 0b11_111_000
  add     ah, cl
  jmp     .ProcessSrc
 .NotSar:

  cmp     bx, x86Sal
  je      .IsSal
  cmp     bx, x86Shl
  jne     .NotShl
 .IsSal:
  mov     ah, 0b11_100_000
  add     ah, cl
  jmp     .ProcessSrc
 .NotShl:

  cmp     bx, x86Shr
  jne     .NotShr
  mov     ah, 0b11_101_000
  add     ah, cl
  jmp     .ProcessSrc
 .NotShr:

 .NotSrcCL:

  xor     ax, ax
  jmp     .Exit

 .ProcessSrc:
  cmp     si, x86Reg8
  je      .Emit
  inc     al
  cmp     si, x86Reg16
  je      .Emit

  xor     ax, ax
  jmp     .Exit

 .EmitOp8:
  lea     bx, [instruction]
  mov     [bx + 0], al
  mov     cx, 1
  jmp     .EmitInstruction

 .Emit:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     cx, 2

 .EmitInstruction:
  push    cx
  push    word [binary]
  push    bx
  push    cx
  call    fwrite
  pop     cx

  cmp     ax, cx
  je      .Done
  xor     ax, ax
  jmp     .Exit
 .Done:

  mov     ax, 1

 .Exit:
  mov     sp, bp
  pop     bp
  ret 6

;
; func bool x86GenOpRegMem( uint op, uint destReg,
;   uint memRegs, int16 displacement )
;
x86GenOpRegMem:
  push    bp
  mov     bp, sp

  %define op           bp + 10
  %define destReg      bp + 8
  %define memRegs      bp + 6
  %define displacement bp + 4

  %define instruction  bp - 6
  push    word 0
  push    word 0
  push    word 0

  mov     bx, [op]
  mov     di, [destReg]
  mov     si, [memRegs]
  mov     dx, [displacement]

  mov     cx, di
  and     di, maskGroup
  and     cx, maskSubToken

  cmp     bx, x86Mov
  jne     .NotMov
  shl     cl, 3
  mov     al, 0b1000_1110
  mov     ah, cl
  cmp     di, x86SReg
  je      .ProcessRM
  mov     al, 0b1000_1010
  mov     ah, cl
  jmp     .ProcessDest
 .NotMov:

  cmp     bx, x86Adc
  jne     .NotAdc
  mov     al, 0b0001_0010
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessDest
 .NotAdc:

  cmp     bx, x86Add
  jne     .NotAdd
  mov     al, 0b0000_0010
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessDest
 .NotAdd:

  cmp     bx, x86And
  jne     .NotAnd
  mov     al, 0b0010_0010
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessDest
 .NotAnd:

  cmp     bx, x86Cmp
  jne     .NotCmp
  mov     al, 0b0011_1000
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessDest
 .NotCmp:

  cmp     bx, x86Or
  jne     .NotOr
  mov     al, 0b0000_1010
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessDest
 .NotOr:

  cmp     bx, x86Sbb
  jne     .NotSbb
  mov     al, 0b0001_1010
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessDest
 .NotSbb:

  cmp     bx, x86Sub
  jne     .NotSub
  mov     al, 0b0010_1010
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessDest
 .NotSub:

  cmp     bx, x86Test
  jne     .NotTest
  mov     al, 0b1000_0100
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessDest
 .NotTest:

  cmp     bx, x86Xchg
  jne     .NotXchg
  mov     al, 0b1000_0110
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessDest
 .NotXchg:

  cmp     bx, x86Xor
  jne     .NotXor
  mov     al, 0b0011_0010
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessDest
 .NotXor:

  cmp     bx, x86Lds
  jne     .NotLds
  mov     al, 0b1100_0101
  shl     cl, 3
  mov     ah, cl
  cmp     di, x86Reg16
  je      .ProcessRM
 .NotLds:

  cmp     bx, x86Les
  jne     .NotLes
  mov     al, 0b1100_0100
  shl     cl, 3
  mov     ah, cl
  cmp     di, x86Reg16
  je      .ProcessRM
 .NotLes:

  cmp     bx, x86Lea
  jne     .NotLea
  mov     al, 0b1000_1101
  shl     cl, 3
  mov     ah, cl
  cmp     di, x86Reg16
  je      .ProcessRM
 .NotLea:

  xor     ax, ax
  jmp     .Exit

 .ProcessDest:
  cmp     di, x86Reg8
  je      .ProcessRM
  inc     al
  cmp     di, x86Reg16
  je      .ProcessRM
  xor     ax, ax
  jmp     .Exit

 .ProcessRM:
  test    si, si
  jnz     .NotDisp16
  or      ah, 0b00_000_110
  jmp     .Emit16
 .NotDisp16:

  ; Reserved bits must be 0
  cmp     si, (mrX86RegBX | mrX86RegBP | mrX86RegSI | mrX86RegDI)
  ja      .Error

  ; Get ModR/M encoding for MOD 00 as the default
  mov     cl, [si + x86RMTable]
  cmp     cl, 0b00_111_000
  je      .Error
  or      ah, cl
  cmp     si, mrX86RegBP
  je      .SkipDisp0
  test    dx, dx
  jz      .Emit
 .SkipDisp0:

  add     ah, 0b01_000_000
  cmp     dx, -128
  jl      .NotDisp8
  cmp     dx, 127
  jg      .NotDisp8
  jmp     .Emit8
 .NotDisp8:

  add     ah, 0b01_000_000
  jmp     .Emit16

 .Emit:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     cx, 2
  jmp     .EmitInstruction

 .Emit8:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dl
  mov     cx, 3
  jmp     .EmitInstruction

 .Emit16:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dx
  mov     cx, 4
  jmp     .EmitInstruction

 .EmitInstruction:
  push    cx
  push    word [binary]
  push    bx
  push    cx
  call    fwrite
  pop     cx

  cmp     ax, cx
  je      .Done
 .Error:
  xor     ax, ax
  jmp     .Exit
 .Done:

  mov     ax, 1

 .Exit:
  mov     sp, bp
  pop     bp
  ret 8

;
; func bool x86GenOpMemImm( uint op, uint memRegs, int16 displacement,
;   uint immType, uint imm )
;;;
x86GenOpMemImm:
  push    bp
  mov     bp, sp

  %define op           bp + 12
  %define memRegs      bp + 10
  %define displacement bp + 8
  %define immType      bp + 6
  %define imm          bp + 4

  %define instruction  bp - 6
  push    word 0
  push    word 0
  push    word 0

  ; Registers
  ; ax = opcode:modRM                   si = memRegs
  ; bx = op                             di = immType
  ; cx = instructionSize                dx = displacement; imm

  mov     bx, [op]
  mov     si, [memRegs]
  mov     dx, [displacement]
  mov     di, [immType]

  mov     ax, [imm]

  cmp     di, valInt8
  jne     .NotInt8
  cbw
  jmp     .ImmInitialized
 .NotInt8:

  cmp     di, valUint8
  jne     .NotUint8
  and     ax, 0x00FF
 .NotUint8:

 .ImmInitialized:
  mov     [imm], ax
  xor     ax, ax

  cmp     bx, x86Mov
  jne     .NotMov
  mov     al, 0b1100_0110 ; Opcode
  jmp     .ProcessRM
 .NotMov:

  cmp     bx, x86Rcl
  jne     .NotRcl
  mov     ah, 0b00_010_000
  mov     al, 0b1100_0000
  mov     di, valUint8
  jmp     .ProcessRM
 .NotRcl:

  cmp     bx, x86Rcr
  jne     .NotRcr
  mov     ah, 0b00_011_000
  mov     al, 0b1100_0000
  mov     di, valUint8
  jmp     .ProcessRM
 .NotRcr:

  cmp     bx, x86Rol
  jne     .NotRol
  ;mov     ah, 0b00_000_000
  mov     al, 0b1100_0000
  mov     di, valUint8
  jmp     .ProcessRM
 .NotRol:

  cmp     bx, x86Ror
  jne     .NotRor
  mov     ah, 0b00_001_000
  mov     al, 0b1100_0000
  mov     di, valUint8
  jmp     .ProcessRM
 .NotRor:

  cmp     bx, x86Sar
  jne     .NotSar
  mov     ah, 0b00_111_000
  mov     al, 0b1100_0000
  mov     di, valUint8
  jmp     .ProcessRM
 .NotSar:

  cmp     bx, x86Sal
  je      .IsSal
  cmp     bx, x86Shl
  jne     .NotShl
 .IsSal:
  mov     ah, 0b00_100_000
  mov     al, 0b1100_0000
  mov     di, valUint8
  jmp     .ProcessRM
 .NotShl:

  cmp     bx, x86Shr
  jne     .NotShr
  mov     ah, 0b00_101_000
  mov     al, 0b1100_0000
  mov     di, valUint8
  jmp     .ProcessRM
 .NotShr:

  cmp     bx, x86Test
  jne     .NotTest
  ;mov     ah, 0b00_000_000
  mov     al, 0b1111_0110
  cmp     di, valInt8
  je      .ProcessRM
  cmp     di, valUint8
  je      .ProcessRM
  inc     al
  jmp     .ProcessRM
 .NotTest:

  mov     al, 0b1000_0000

  cmp     bx, x86Adc
  jne     .NotAdc
  mov     ah, 0b00_010_000
  jmp     .ProcessOp
 .NotAdc:

  cmp     bx, x86Add
  jne     .NotAdd
  ;mov     ah, 0b00_000_000
  jmp     .ProcessOp
 .NotAdd:

  cmp     bx, x86And
  jne     .NotAnd
  mov     ah, 0b00_100_000
  jmp     .ProcessOp
 .NotAnd:

  cmp     bx, x86Cmp
  jne     .NotCmp
  mov     ah, 0b00_111_000
  jmp     .ProcessOp
 .NotCmp:

  cmp     bx, x86Or
  jne     .NotOr
  mov     ah, 0b00_001_000
  jmp     .ProcessOp
 .NotOr:

  cmp     bx, x86Sbb
  jne     .NotSbb
  mov     ah, 0b00_011_000
  jmp     .ProcessOp
 .NotSbb:

  cmp     bx, x86Sub
  jne     .NotSub
  mov     ah, 0b00_101_000
  jmp     .ProcessOp
 .NotSub:

  cmp     bx, x86Xor
  jne     .NotXor
  mov     ah, 0b00_110_000
  jmp     .ProcessOp
 .NotXor:

  xor     ax, ax
  jmp     .Exit

 .ProcessOp:
  cmp     di, valUint8
  je      .ProcessRM
  inc     al
  cmp     di, valInt
  je      .ProcessRM
  cmp     di, valUint
  je      .ProcessRM
  add     al, 0b10
  jmp     .ProcessRM

 .ProcessRM:
  test    si, si
  jnz     .NotDisp16
  or      ah, 0b00_000_110
  jmp     .ProcessDisp16Imm
 .NotDisp16:

  ; Reserved bits must be 0
  cmp     si, (mrX86RegBX | mrX86RegBP | mrX86RegSI | mrX86RegDI)
  ja      .Error

  ; Get ModR/M encoding for MOD 00 as the default
  mov     cl, [si + x86RMTable]
  cmp     cl, 0b00_111_000
  je      .Error
  or      ah, cl
  cmp     si, mrX86RegBP
  je      .SkipDisp0
  test    dx, dx
  jz      .ProcessImm
 .SkipDisp0:

  add     ah, 0b01_000_000
  cmp     dx, -128
  jl      .NotDisp8
  cmp     dx, 127
  jg      .NotDisp8
  jmp     .ProcessDisp8Imm
 .NotDisp8:

  add     ah, 0b01_000_000
  jmp     .ProcessDisp16Imm

  ; Assume that immediate is validated as either 8-bit or 16-bit
 .ProcessImm:
  mov     cx, [imm]

  cmp     di, valInt8
  je      .EmitImm8
  cmp     di, valUint8
  je      .EmitImm8
  cmp     di, valInt
  je      .EmitImm16
  cmp     di, valUint
  je      .EmitImm16
  xor     ax, ax
  jmp     .Exit

 .EmitImm8:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], cl
  mov     cx, 3
  jmp     .EmitInstruction

 .EmitImm16:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], cx
  mov     cx, 4
  jmp     .EmitInstruction

  ; Assume that immediate is validated as either 8-bit or 16-bit
 .ProcessDisp8Imm:
  mov     cx, [imm]

  cmp     di, valInt
  je      .EmitDisp8Imm16
  cmp     di, valUint
  je      .EmitDisp8Imm16

 .EmitDisp8Imm8:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dl
  mov     [bx + 3], cl
  mov     cx, 4
  jmp     .EmitInstruction

 .EmitDisp8Imm16:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dl
  mov     [bx + 3], cx
  mov     cx, 5
  jmp     .EmitInstruction

  ; Assume that immediate is validated as either 8-bit or 16-bit
 .ProcessDisp16Imm:
  mov     cx, [imm]

  cmp     di, valInt
  je      .EmitDisp16Imm16
  cmp     di, valUint
  je      .EmitDisp16Imm16

 .EmitDisp16Imm8:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dx
  mov     [bx + 4], cl
  mov     cx, 5
  jmp     .EmitInstruction

 .EmitDisp16Imm16:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dx
  mov     [bx + 4], cx
  mov     cx, 6
  jmp     .EmitInstruction

 .EmitInstruction:
  push    cx
  push    word [binary]
  push    bx
  push    cx
  call    fwrite
  pop     cx

  cmp     ax, cx
  je      .Done
 .Error:
  xor     ax, ax
  jmp     .Exit
 .Done:

  mov     ax, 1

 .Exit:
  mov     sp, bp
  pop     bp
  ret 10

;
; func bool x86GenOpMemReg( uint op, uint memRegs, int16 displacement,
;   uint srcReg )
;
x86GenOpMemReg:
  push    bp
  mov     bp, sp

  %define op           bp + 10
  %define memRegs      bp + 8
  %define displacement bp + 6
  %define srcReg       bp + 4

  %define instruction  bp - 6
  push    word 0
  push    word 0
  push    word 0

  mov     bx, [op]
  mov     di, [srcReg]
  mov     si, [memRegs]
  mov     dx, [displacement]

  mov     cx, di
  mov     ax, di
  and     di, maskGroup
  and     cx, maskSubToken

  ; op mem, reg
  cmp     bx, x86Mov
  jne     .NotMov
  cmp     di, x86SReg
  jne     .NotMovSReg
  mov     al, 0b1000_1100
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessRM
 .NotMovSReg:
  mov     al, 0b1000_1000
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessSrc
 .NotMov:

  cmp     bx, x86Adc
  jne     .NotAdc
  mov     al, 0b0001_0000
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessSrc
 .NotAdc:

  cmp     bx, x86Add
  jne     .NotAdd
  mov     al, 0b0000_0000
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessSrc
 .NotAdd:

  cmp     bx, x86And
  jne     .NotAnd
  mov     al, 0b0010_0000
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessSrc
 .NotAnd:

  cmp     bx, x86Cmp
  jne     .NotCmp
  mov     al, 0b0011_1010
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessSrc
 .NotCmp:

  cmp     bx, x86Or
  jne     .NotOr
  mov     al, 0b0000_1000
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessSrc
 .NotOr:

  cmp     bx, x86Sbb
  jne     .NotSbb
  mov     al, 0b0001_1000
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessSrc
 .NotSbb:

  cmp     bx, x86Sub
  jne     .NotSub
  mov     al, 0b0010_1000
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessSrc
 .NotSub:

  cmp     bx, x86Xor
  jne     .NotXor
  mov     al, 0b0011_0000
  shl     cl, 3
  mov     ah, cl
  jmp     .ProcessSrc
 .NotXor:

  xor     ax, ax
  jmp     .Exit

 .ProcessSrc:
  cmp     di, x86Reg8
  je      .ProcessRM
  inc     al
  cmp     di, x86Reg16
  je      .ProcessRM

  xor     ax, ax
  jmp     .Exit

 .ProcessRM:
  test    si, si
  jnz     .NotDisp16
  or      ah, 0b00_000_110
  jmp     .Emit16
 .NotDisp16:

  ; Reserved bits must be 0
  cmp     si, (mrX86RegBX | mrX86RegBP | mrX86RegSI | mrX86RegDI)
  ja      .Error

  ; Get ModR/M encoding for MOD 00 as the default
  mov     cl, [si + x86RMTable]
  cmp     cl, 0b00_111_000
  je      .Error
  or      ah, cl
  cmp     si, mrX86RegBP
  je      .SkipDisp0
  test    dx, dx
  jz      .Emit
 .SkipDisp0:

  add     ah, 0b01_000_000
  cmp     dx, -128
  jl      .NotDisp8
  cmp     dx, 127
  jg      .NotDisp8
  jmp     .Emit8
 .NotDisp8:

  add     ah, 0b01_000_000
  jmp     .Emit16

 .Emit:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     cx, 2
  jmp     .EmitInstruction

 .Emit8:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dl
  mov     cx, 3
  jmp     .EmitInstruction

 .Emit16:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dx
  mov     cx, 4
  jmp     .EmitInstruction

 .EmitInstruction:
  push    cx
  push    word [binary]
  push    bx
  push    cx
  call    fwrite
  pop     cx

  cmp     ax, cx
  je      .Done
 .Error:
  xor     ax, ax
  jmp     .Exit
 .Done:

  mov     ax, 1

 .Exit:
  mov     sp, bp
  pop     bp
  ret 8

;
; func bool x86GenOpMemCL( uint op, memSize, memRegs; int16 displacement )
;
x86GenOpMemCL:
  push    bp
  mov     bp, sp

  %define op           bp + 10
  %define memSize      bp + 8
  %define memRegs      bp + 6
  %define displacement bp + 4

  %define instruction  bp - 4
  push    word 0
  push    word 0

  mov     bx, [op]
  mov     di, [srcReg]
  mov     si, [memRegs]
  mov     dx, [displacement]

  mov     cx, di
  mov     ax, di
  and     di, maskGroup
  and     cx, maskSubToken

  cmp     bx, x86Rcl
  jne     .NotRcl
  mov     al, 0b1101_0010
  mov     ah, 0b00_010_000
  jmp     .ProcessDest
 .NotRcl:

  cmp     bx, x86Rcr
  jne     .NotRcr
  mov     al, 0b1101_0010
  mov     ah, 0b00_011_000
  jmp     .ProcessDest
 .NotRcr:

  cmp     bx, x86Rol
  jne     .NotRol
  mov     al, 0b1101_0010
  mov     ah, 0b00_000_000
  jmp     .ProcessDest
 .NotRol:

  cmp     bx, x86Ror
  jne     .NotRor
  mov     al, 0b1101_0010
  mov     ah, 0b00_001_000
  jmp     .ProcessDest
 .NotRor:

  cmp     bx, x86Sar
  jne     .NotSar
  mov     al, 0b1101_0010
  mov     ah, 0b00_111_000
  jmp     .ProcessDest
 .NotSar:

  cmp     bx, x86Sal
  je      .IsSal
  cmp     bx, x86Shl
  jne     .NotShl
 .IsSal:
  mov     al, 0b1101_0010
  mov     ah, 0b00_100_000
  jmp     .ProcessDest
 .NotShl:

  cmp     bx, x86Shr
  jne     .NotShr
  mov     al, 0b1101_0010
  mov     ah, 0b00_101_000
  jmp     .ProcessDest
 .NotShr:

  xor     ax, ax
  jmp     .Exit

 .ProcessDest:
  cmp     word [memSize], 1
  je      .ProcessRM

  inc     al
  cmp     word [memSize], 2
  je      .ProcessRM

  xor     ax, ax
  jmp     .Exit

 .ProcessRM:
  test    si, si
  jnz     .NotDisp16
  or      ah, 0b00_000_110
  jmp     .Emit16
 .NotDisp16:

  ; Reserved bits must be 0
  cmp     si, (mrX86RegBX | mrX86RegBP | mrX86RegSI | mrX86RegDI)
  ja      .Error

  ; Get ModR/M encoding for MOD 00 as the default
  mov     cl, [si + x86RMTable]
  cmp     cl, 0b00_111_000
  je      .Error
  or      ah, cl
  cmp     si, mrX86RegBP
  je      .SkipDisp0
  test    dx, dx
  jz      .Emit
 .SkipDisp0:

  add     ah, 0b01_000_000
  cmp     dx, -128
  jl      .NotDisp8
  cmp     dx, 127
  jg      .NotDisp8
  jmp     .Emit8
 .NotDisp8:

  add     ah, 0b01_000_000
  jmp     .Emit16

 .Emit:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     cx, 2
  jmp     .EmitInstruction

 .Emit8:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dl
  mov     cx, 3
  jmp     .EmitInstruction

 .Emit16:
  lea     bx, [instruction]
  mov     [bx + 0], ax
  mov     [bx + 2], dx
  mov     cx, 4
  jmp     .EmitInstruction

 .EmitInstruction:
  push    cx
  push    word [binary]
  push    bx
  push    cx
  call    fwrite
  pop     cx

  cmp     ax, cx
  je      .Done
 .Error:
  xor     ax, ax
  jmp     .Exit
 .Done:

  mov     ax, 1

 .Exit:
  mov     sp, bp
  pop     bp
  ret 8
