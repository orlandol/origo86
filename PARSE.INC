;
; func bool ReadChar()
;
ReadChar:
  push    bp
  mov     bp, sp
  %define colInc bp - 2
  push    word 1

  cmp     word [curCh], 10
  jne     .NotLF
  inc     word [line]
  mov     word [col], 0
  mov     word [colInc], 0
 .NotLF:

  mov     ax, [nextCh]
  mov     [curCh], ax
  mov     word [nextCh], 0

  push    word [source]
  push    nextCh
  push    word 1
  call    fread
  cmp     ax, 1
  je      .HaveNextCh
  xor     ax, ax
  jmp     .Exit
 .HaveNextCh:

  cmp     word [nextCh], 13
  jne     .NotCR
  push    word [source]
  push    nextCh
  push    word 1
  call    fread
  cmp     ax, 1
  je      .CheckLF
  xor     ax, ax
  jmp     .Exit
 .CheckLF:
  cmp     word [nextCh], 10
  je      .DoneCR
  push    word [source]
  push    word -1
  call    fseekby
  cmp     ax, 1
  je      .DoneCR
  xor     ax, ax
  jmp     .Exit
 .DoneCR:
  mov     word [nextCh], 10
 .NotCR:

  mov     ax, [colInc]
  add     word [col], ax

  mov     ax, 1

 .Exit:
  mov     sp, bp
  pop     bp
  ret

;
; func uint GetNumber()
;
GetNumber:
  push    bp
  mov     bp, sp
  %define number bp - 2
  push    word 0
  %define destLoop bp - 4
  push    .ReadDigitLoop
  %define leadFlag bp - 6
  push    word chDigit

  xor     ax, ax
  mov     bx, [curCh]
  mov     cl, [bx + chTable]
  test    cl, chBin | chOct | chHex | chDigit
  jz      .Exit

  cmp     bl, '0'
  jne     .ExitSkipLead

  cmp     word [nextCh], 'x'
  jne     .NotHex
  mov     word [destLoop], .ReadHexLoop
  mov     word [leadFlag], chHex
  jmp     .ValidateLead
 .NotHex:

  cmp     word [nextCh], 'b'
  jne     .NotBin
  mov     word [destLoop], .ReadBinLoop
  mov     word [leadFlag], chBin
  jmp     .ValidateLead
 .NotBin:

  cmp     word [nextCh], 'o'
  jne     .NotOct
  mov     word [destLoop], .ReadOctLoop
  mov     word [leadFlag], chOct
  jmp     .ValidateLead
 .NotOct:

  jmp     .SkipLeadLoop

 .ValidateLead:
  call    ReadChar
  call    ReadChar
  xor     ax, ax
  mov     bx, [curCh]
  mov     cl, [bx + chTable]
  test    cl, byte [leadFlag]
  jz      .Exit

 .SkipLeadLoop:
  cmp     bl, '0'
  jne     .ExitSkipLead
  call    ReadChar
  mov     bx, [curCh]
  jmp     .SkipLeadLoop
 .ExitSkipLead:
  jmp     word [destLoop]

 .ReadDigitLoop:
  mov     cl, [bx + chTable]
  test    cl, chDigit
  jz      .Done
  mov     dx, [number]
  mov     cx, dx
  sub     bl, '0'
  shl     dx, 3
  add     cx, cx
  add     dx, bx
  add     dx, cx
  mov     [number], dx
  call    ReadChar
  mov     bx, [curCh]
  jmp     .ReadDigitLoop

 .ReadHexLoop:
  mov     cl, [bx + chTable]
  test    cl, chHex
  jz      .Done
  test    cl, chAlpha
  jz      .NotAlpha
  or      bl, 32
  sub     bl, ('a' - '0' - 10)
 .NotAlpha:
  sub     bl, '0'
  mov     dx, [number]
  shl     dx, 4
  add     dx, bx
  mov     [number], dx
  call    ReadChar
  mov     bx, [curCh]
  jmp     .ReadHexLoop

 .ReadBinLoop:
  mov     cl, [bx + chTable]
  test    cl, chBin
  jz      .Done
  mov     dx, [number]
  sub     bl, '0'
  add     dx, dx
  add     dx, bx
  mov     [number], dx
  call    ReadChar
  mov     bx, [curCh]
  jmp     .ReadBinLoop

 .ReadOctLoop:
  mov     cl, [bx + chTable]
  test    cl, chOct
  jz      .Done
  mov     dx, [number]
  sub     bl, '0'
  shl     dx, 3
  add     dx, bx
  mov     [number], dx
  call    ReadChar
  mov     bx, [curCh]
  jmp     .ReadOctLoop

 .Done:
  mov     ax, valUint
  mov     dx, [number]
  mov     [nextUint], dx

 .Exit:
  mov     sp, bp
  pop     bp
  ret

;
; func uint GetIdent()
;
GetIdent:
  push    bp
  mov     bp, sp

  mov     di, nextIdent
  xor     ax, ax
  xor     cx, cx

  mov     bx, [curCh]
  mov     dl, [bx + chTable]
  and     dl, chIdent | chAlpha
  jz      .Exit

  push    cx
  push    di

 .ReadLoop:
  mov     cx, [bp - 2]
  mov     di, [bp - 4]
  mov     dl, [bx + chTable]
  and     dl, (chIdent | chDigit)
  jz      .ExitReadLoop
  cmp     cx, IDENT_MAXLEN
  jae     .SkipWrite
  mov     [di], bl
  inc     di
  inc     cx
 .SkipWrite:
  mov     [bp - 2], cx
  mov     [bp - 4], di
  call    ReadChar
  mov     bx, [curCh]
  jmp     .ReadLoop
 .ExitReadLoop:
  mov     byte [di], 0

  mov     ax, tkIdent

 .Exit:
  mov     sp, bp
  pop     bp
  ret

;
; func uint GetKeyword()
;
GetKeyword:
  push    bp
  mov     bp, sp

  %define token bp - 2
  push    word 0

  call    GetIdent
  cmp     ax, tkIdent
  jne     .Exit
  mov     [token], ax

  mov     bx, keywordTable

 .Search:
  cmp     bx, keywordTableEnd
  jae     .Exit

  push    bx
  push    nextIdent;curIdent?
  push    word [bx]
  call    comparenc
  pop     bx

  cmp     ax, 0
  je      .MatchFound
  add     bx, SIZEOF_KWITEM
  jmp     .Search

 .MatchFound:
  mov     ax, [bx + KWITEM_TOKEN]
  mov     [token], ax

 .Exit:
  mov     ax, [token]

  mov     sp, bp
  pop     bp
  ret

;
; func uint GetString()
;
GetString:
  push    bp
  mov     bp, sp
  %define closeCh bp - 2
  push    word 0
  %define stringDest bp - 4
  push    nextString
  %define stringLen bp - 6
  push    word 0

  xor     ax, ax
  mov     bx, [curCh]

  cmp     bx, 34
  je      .IsString
  cmp     bx, 39
  jne     .Exit
 .IsString:
  mov     [closeCh], bx

  call    ReadChar

  mov     bx, [curCh]
  mov     di, [stringDest]
  mov     dx, [stringLen]

 .ReadLoop:
  xor     ax, ax
  test    bx, bx
  jz      .Exit
  cmp     dx, STRING_MAXLEN
  je      .Exit
  cmp     bx, word [closeCh]
  je      .ExitRead
  mov     cl, [bx + chTable]
  test    cl, chPrint
  jz      .Exit
  mov     [di], bl
  inc     di
  inc     dx
  mov     [stringLen], dx
  mov     [stringDest], di
  call    ReadChar
  mov     bx, [curCh]
  mov     di, [stringDest]
  mov     dx, [stringLen]
  jmp     .ReadLoop
 .ExitRead:
  call    ReadChar

  mov     ax, valChar
  cmp     word [stringLen], 1
  jbe     .Exit
  mov     ax, valString

 .Exit:
  mov     sp, bp
  pop     bp
  ret

;
; func uint GetOper()
;
GetOper:
  push    bp
  mov     bp, sp

  %define operLow bp - 2
  push    word [curCh]
  %define operHigh bp - 4
  push    word 0
  %define matchLen bp - 6
  push    word 0
  %define token bp - 8
  push    word 0

 .Next:
  cmp     word [matchLen], 4
  jae     .Exit

  mov     bx, opTable

  mov     ax, [operLow]
  mov     dx, [operHigh]

 .Search:
  cmp     bx, opTableEnd
  jae     .Exit
  cmp     ax, [bx + OPITEM_OPERLOW]
  jne     .OperNotEq
  cmp     dx, [bx + OPITEM_OPERHIGH]
  jne     .OperNotEq
  mov     cx, [bx + OPITEM_TOKEN]
  mov     [token], cx
  jmp     .MatchFound
 .OperNotEq:

  add     bx, SIZEOF_OPITEM
  jmp     .Search

 .MatchFound:
  pusha
  call    ReadChar
  popa

  mov     ax, [operLow]
  mov     dx, [operHigh]
  mov     dh, dl
  mov     dl, ah
  mov     ah, al
  mov     al, [curCh]
  mov     [operLow], ax
  mov     [operHigh], dx
  inc     word [matchLen]
  jmp     .Next

 .Exit:
  mov     ax, [token]

  mov     sp, bp
  pop     bp
  ret

;
; func uint GetToken()
;
GetToken:
  push    bp
  mov     bp, sp

  ; Copy next variables into current variables
  mov     dx, [nextLine]
  mov     [curLine], dx
  mov     dx, [nextCol]
  mov     [curCol], dx
  mov     dx, [nextToken]
  mov     [curToken], dx
  mov     dx, [nextUint]
  mov     [curUint], dx
  mov     si, nextIdent
  mov     di, curIdent
  mov     cx, IDENT_MAXLEN + 1
  rep movsb
  mov     si, nextString
  mov     di, curString
  mov     cx, IDENT_MAXLEN + 1
  rep movsb

  ; Initialize appropriate next variables
  mov     word [nextToken], 0
  mov     word [nextUint], 0
  mov     di, nextIdent
  mov     cx, IDENT_MAXLEN + 1
  xor     ax, ax
  rep stosb
  mov     di, nextString
  mov     cx, STRING_MAXLEN + 1
  xor     ax, ax
  rep stosb

  ; Initialize defaults
  xor     ax, ax
  mov     bx, [curCh]
  mov     cl, [bx + chTable]

  ; Skip space characters
 .SkipSpace:
  test    cl, chSpace
  jz      .ExitSkipSpace
  call    ReadChar
  mov     bx, [curCh]
  mov     cl, [bx + chTable]
  jmp     .SkipSpace
 .ExitSkipSpace:

  mov     ax, [line]
  mov     [nextLine], ax
  mov     ax, [col]
  mov     [nextCol], ax

  ; Check for EOF
  test    bl, bl
  jz      .Exit

  ; Check for number
  test    cl, chDigit
  jz      .NotNumber
  call    GetNumber
  mov     [nextToken], ax
  jmp     .Exit
 .NotNumber:

  ; Check for ident
  test    cl, chIdent
  jz      .NotIdent
  call    GetKeyword
  mov     [nextToken], ax
  jmp     .Exit
 .NotIdent:

  ; Check for string
  cmp     bl, 34 ; "
  je      .IsString
  cmp     bl, 39 ; '
  jne     .NotString
 .IsString:
  call    GetString
  mov     [nextToken], ax
  jmp     .Exit
 .NotString:

  call    GetOper
  mov     [nextToken], ax

 .Exit:
  mov     sp, bp
  pop     bp
  ret
