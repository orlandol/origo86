;
; func bool x86GenOp( uint opToken )
;
x86GenOp:
  ret 2

;
; func bool x86GenOpReg( uint opToken, uint reg )
;
x86GenOpReg:
  ret 4

;
; func bool x86GenRepOpReg( uint repToken, uint opToken )
;
x86GenRepOpReg:
  ret 4

;
; func bool x86GenOpImm( uint opToken, uint immType, uint imm )
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
  ;add     [codeSize], cx

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
  add     ax, 0b11_000_000__1100_0110 ; ModR/M:Opcode => Opcode, ModR/M
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
  ;add     [codeSize], cx

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
; func bool x86GenOpRegMem( uint opToken, uint destReg, @x86MemRef mem )
;
x86GenOpRegMem:
  ret 6

;
; func bool x86GenOpRegReg( uint opToken, uint destReg, uint srcReg )
;
x86GenOpRegReg:
  ret 6

