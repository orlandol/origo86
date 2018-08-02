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
; func bool x86GenOpImm( uint opToken, uint imm )
;
x86GenOpImm:
  ret 4

;
; func bool x86GenOpRegImm( uint opToken, uint destReg,
;   uint immType, uint value )
;
x86GenOpRegImm:
  push    bp
  mov     bp, sp
  %define opToken bp - 12
  %define destReg bp - 10
  %define immType bp - 8
  %define value   bp - 6

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

