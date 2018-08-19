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

  cmp     si, 17
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
  db 0b1100_0011 ; x86Ret
  db 0b1100_1011 ; x86Retf
  db 0b1001_1110 ; x86Sahf
  db 0b1111_1001 ; x86Stc
  db 0b1111_1101 ; x86Std
  db 0b1111_1011 ; x86Sti
  db 0b1101_0111 ; x86Xlat
  db 0b1101_0111 ; x86Xlatb

;
; func bool x86GenRepOp( uint repToken, uint op )
;
x86GenRepOp:
  ret 4

;
; func bool x86GenOpImm( uint op, uint immType, uint imm )
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
  mov     dx, [imm]

  cmp     bx, x86Int
  jne     .NotX86Int
  mov     al, 0b1100_1101
  jmp     .Emit8
 .NotX86Int:

  xor     ax, ax
  jmp     .Exit

 .Emit8:
  lea     bx, [instruction]
  mov     [bx + 0], al
  mov     [bx + 1], dl
  mov     cx, 2
  ;jmp     .EmitInstruction

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
; func bool x86GenOpReg( uint op, uint reg )
;
x86GenOpReg:
  ret 4

;
; func bool x86GenOpMem( uint op, uint memRegs, int displacement )
;
x86GenOpMem:
  ret 6

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

  mov     ax, si
  and     si, maskGroup
  and     ax, maskSubToken

  cmp     bx, x86Mov
  jne     .NotMov
  add     ah, 0b11_000_000 ; ModReg
  mov     al, 0b1100_0110 ; Opcode
 .NotMov:

  cmp     si, x86Reg8
  je      .Emit8

  inc     al ; Opcode => 0b1100_0111
  cmp     si, x86Reg16
  je      .Emit16

  xor     ax, ax
  jmp     .Exit

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
  ;jmp     .EmitInstruction

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
  jne     .Exit
  shl     dl, 3
  or      cl, dl

  cmp     bx, x86Adc
  jne     .NotAdc
  mov     al, 0b0001_0000
  mov     ah, cl
  or      ah, 0b11_000_000
  cmp     si, x86Reg8
  je      .Emit
  inc     al
  cmp     di, x86Reg16
  je      .Emit
 .NotAdc:

  xor     ax, ax
  jmp     .Exit

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
  ret 8

;
; func bool x86GenOpMemImm( uint op, uint memRegs, int16 displacement,
;   uint immType, uint imm )
;
x86GenOpMemImm:
  ret 10

;
; func bool x86GenOpMemReg( uint op, uint memRegs, int16 displacement,
;   uint srcReg )
;
x86GenOpMemReg:
  ret 8
