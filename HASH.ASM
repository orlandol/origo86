
  ORG 0x100
  CPU 286

  %include "defs.inc"

  %include "initrtl.inc"

section .text
run:
  push    bp
  mov     bp, sp

  %define hashStr bp - 2
  push    word testStrings

 .HashLoop:
;  cmp     word [hashStr], identStrings
  cmp     word [hashStr], testStringsEnd
  jae     .ExitHashLoop
  push    word [hashStr]
  call    echostring
  push    word ','
  call    echochar
  push    word [hashStr]
  call    Hash
  push    ax
  call    echouint
  push    eoln
  call    echostring
  mov     si, [hashStr]

 .NextChar:
  mov     al, [si]
  inc     si
  test    al, al
  jz      .DoneStr
  jmp     .NextChar
 .DoneStr:
  mov     [hashStr], si
  jmp     .HashLoop
 .ExitHashLoop:

  mov     sp, bp
  pop     bp
  mov     ax, 0x4C00
  int     0x21

;
; func uint Hash( string str )
;;;
Hash:
  push    bp
  mov     bp, sp

  %define str bp + 4

  xor     ax, ax

  mov     si, [str]
  test    si, si
  jz      .Exit

  cmp     byte [si], 0
  je      .Exit

  xor     bx, bx

  ; .09AZ_az
 .CalcHash:
  mov     bl, [si]
  inc     si
  add     bx, bx
  jz      .ExitCalcHash

  xor     ax, [bx + hashBits]
  rol     ax, 1
  add     ax, 10141
  xor     ax, bx

  xor     bx, bx
  jmp     .CalcHash
 .ExitCalcHash:

 .Exit:
  mov     sp, bp
  pop     bp
  ret 2

section .data
  ; EARIOTNSLCUDPMHGBFYWKVXZJQ
  ; Letter frequency from Robert Lewand's Cryptological Mathematics
  ;   cited by Pavel Micka per Wikipedia - Letter Frequency
  hashBits:
                          times 45 dw 0
    dw 0b0001000000001000 ; .
                          times 1 dw 0
;    dw 0b0100001000000010 ; 0
;    dw 0b0000100100000100 ; 1
;    dw 0b1010000000010000 ; 2
;    dw 0b0000100100000100 ; 3
;    dw 0b0010000001000001 ; 4
;    dw 0b1000000100010000 ; 5
;    dw 0b0000100001000100 ; 6
;    dw 0b0010000100000001 ; 7
;    dw 0b1000100000000100 ; 8
;    dw 0b0010000001010000 ; 9

    dw 0b0100000000000010 ; 0
    dw 0b0000000100001000 ; 1
    dw 0b1010000000000000 ; 2
    dw 0b0000100000000100 ; 3
    dw 0b0010000001000000 ; 4
    dw 0b1000000000010000 ; 5
    dw 0b0000100001000000 ; 6
    dw 0b0000000100000001 ; 7
    dw 0b1000000000000100 ; 8
    dw 0b0010000000010000 ; 9
                          times 7 dw 0
    dw 0b0000000000100000 ; A
    dw 0b1000100000000000 ; B
    dw 0b0000010000100000 ; C
    dw 0b0000000100000001 ; D
    dw 0b0000010000000000 ; E
    dw 0b0100000001000000 ; F
    dw 0b0000000010000100 ; G
    dw 0b0000000010010000 ; H
    dw 0b0010000000000000 ; I
    dw 0b0100001000000000 ; J
    dw 0b1000000000000010 ; K
    dw 0b0100000010000000 ; L
    dw 0b0001000000001000 ; M
    dw 0b0000001000000010 ; N
    dw 0b0000000000000001 ; O
    dw 0b0010000000001000 ; P
    dw 0b0000010000000100 ; Q
    dw 0b0001000000100000 ; R
    dw 0b0000101000000000 ; S
    dw 0b1000000000000010 ; T
    dw 0b0000000000010000 ; U
    dw 0b0010000000000100 ; V
    dw 0b0000010000100000 ; W
    dw 0b0001000000000001 ; X
    dw 0b0000000000000100 ; Y
    dw 0b0010000001000000 ; Z
                          times 4 dw 0
    dw 0b0000100100000000 ; _
                          times 1 dw 0
    dw 0b1000000000000000 ; a
    dw 0b0000000100001000 ; b
    dw 0b0000100000000001 ; c
    dw 0b0000000000010100 ; d
    dw 0b0000000001000000 ; e
    dw 0b0001010000000000 ; f
    dw 0b0010000000000010 ; g
    dw 0b0100000000000010 ; h
    dw 0b0000000000001000 ; i
    dw 0b0010000000100000 ; j
    dw 0b0010000100000000 ; k
    dw 0b0000000000101000 ; l
    dw 0b0000000101000000 ; m
    dw 0b0100100000000000 ; n
    dw 0b0000000010000000 ; o
    dw 0b0000000000010010 ; p
    dw 0b0000001001000000 ; q
    dw 0b1000010000000000 ; r
    dw 0b0000000000100001 ; s
    dw 0b0001010000000000 ; t
    dw 0b0000100000000000 ; u
    dw 0b0000000100001000 ; v
    dw 0b0001000000000001 ; w
    dw 0b0100000010000000 ; x
    dw 0b0000001000000000 ; y
    dw 0b1000000000010000 ; z

  testStrings:
    db 'origo', 0
    db 'Origo', 0
    db 'ORigo', 0
    db 'ORIgo', 0
    db 'ORIGo', 0
    db 'ORIGO', 0
    db 'oRIGO', 0
    db 'orIGO', 0
    db 'oriGO', 0
    db 'origO', 0
    db 'o', 0
    db 'r', 0
    db 'i', 0
    db 'g', 0
    db 'O', 0
    db 'R', 0
    db 'I', 0
    db 'G', 0
    ; begin - idents.txt
  identStrings:
    db 'BinExtSpecified',0
    db 'BinNameFromSrc',0
    db 'BinSkipDot',0
    db 'CalcHash',0
    db 'CharLoop',0
    db 'CheckLF',0
    db 'Cleanup',0
    db 'CloseBinary',0
    db 'CloseRet',0
    db 'CompareLoop',0
    db 'Compile',0
    db 'CreateBinary',0
    db 'DeclareNamespace',0
    db 'DigitLoop',0
    db 'Done',0
    db 'DoneBinaryName',0
    db 'DoneCR',0
    db 'DoneChar',0
    db 'DoneParse',0
    db 'DoneSourceName',0
    db 'EchoLoop',0
    db 'Emit',0
    db 'Emit16',0
    db 'Emit8',0
    db 'EmitDisp16Imm16',0
    db 'EmitDisp16Imm8',0
    db 'EmitDisp8Imm16',0
    db 'EmitDisp8Imm8',0
    db 'EmitImm16',0
    db 'EmitImm8',0
    db 'EmitInstruction',0
    db 'EmitOp16Imm16',0
    db 'EmitOp16Imm8',0
    db 'EmitOp8Imm8',0
    db 'Error',0
    db 'Exit',0
    db 'ExitCalcHash',0
    db 'ExitCompare',0
    db 'ExitHashLoop',0
    db 'ExitRead',0
    db 'ExitReadLoop',0
    db 'ExitSetAlpha1',0
    db 'ExitSetAlpha2',0
    db 'ExitSetHex',0
    db 'ExitSetPrint',0
    db 'ExitSkipLead',0
    db 'ExitSkipSpace',0
    db 'Expected',0
    db 'ExprGetToken',0
    db 'GetIdent',0
    db 'GetKeyword',0
    db 'GetNumber',0
    db 'GetOper',0
    db 'GetString',0
    db 'GetToken',0
    db 'Hash',0
    db 'HashLoop',0
    db 'HaveNextCh',0
    db 'HexLoop',0
    db 'IDENT_MAXLEN',0
    db 'ImmInitialized',0
    db 'InitArgs_CopyLoop',0
    db 'InitArgs_Done',0
    db 'InitArgs_Exit',0
    db 'InitArgs_ExitCopyLoop',0
    db 'InitArgs_ExitSkipSpace',0
    db 'InitArgs_SkipSpace',0
    db 'InitEchoLoop',0
    db 'InitRTL_MemResized',0
    db 'IsLoopnz',0
    db 'IsLoopz',0
    db 'IsOpen',0
    db 'IsProgram',0
    db 'IsProgramIdent',0
    db 'IsRepe',0
    db 'IsRepne',0
    db 'IsSal',0
    db 'IsString',0
    db 'KWITEM_KWSTRING',0
    db 'KWITEM_TOKEN',0
    db 'KWTABLE_COUNT',0
    db 'LeftNotUppercase',0
    db 'LongDigitLoop',0
    db 'MatchFound',0
    db 'ModifyBinExt',0
    db 'Next',0
    db 'NotAdc',0
    db 'NotAdd',0
    db 'NotAlpha',0
    db 'NotAnd',0
    db 'NotBin',0
    db 'NotCR',0
    db 'NotCall',0
    db 'NotCmp',0
    db 'NotDec',0
    db 'NotDigit',0
    db 'NotDisp16',0
    db 'NotDisp8',0
    db 'NotDiv',0
    db 'NotEOF',0
    db 'NotEnd',0
    db 'NotHex',0
    db 'NotIDiv',0
    db 'NotIMul',0
    db 'NotIdent',0
    db 'NotIn',0
    db 'NotInRegAL',0
    db 'NotInc',0
    db 'NotInt8',0
    db 'NotJcc',0
    db 'NotJcxz',0
    db 'NotJmp',0
    db 'NotLF',0
    db 'NotLds',0
    db 'NotLea',0
    db 'NotLes',0
    db 'NotLoop',0
    db 'NotLoope',0
    db 'NotLoopne',0
    db 'NotLower',0
    db 'NotMov',0
    db 'NotMovSReg',0
    db 'NotMovSRegDest',0
    db 'NotMul',0
    db 'NotNeg',0
    db 'NotNot',0
    db 'NotNumber',0
    db 'NotOct',0
    db 'NotOr',0
    db 'NotOut',0
    db 'NotOutRegAL',0
    db 'NotPop',0
    db 'NotPush',0
    db 'NotRcl',0
    db 'NotRcr',0
    db 'NotRep',0
    db 'NotRepe',0
    db 'NotRepne',0
    db 'NotRet',0
    db 'NotRetf',0
    db 'NotRol',0
    db 'NotRor',0
    db 'NotRun',0
    db 'NotSar',0
    db 'NotSbb',0
    db 'NotShl',0
    db 'NotShr',0
    db 'NotSrcCL',0
    db 'NotString',0
    db 'NotSub',0
    db 'NotTest',0
    db 'NotUint8',0
    db 'NotUnderscore',0
    db 'NotUpper',0
    db 'NotX86Int',0
    db 'NotXchg',0
    db 'NotXor',0
    db 'OPITEM_OPERHIGH',0
    db 'OPITEM_OPERLOW',0
    db 'OPITEM_TOKEN',0
    db 'OPTABLE_COUNT',0
    db 'OpenRet',0
    db 'OperNotEq',0
    db 'ParseBinaryName',0
    db 'ParseExpr',0
    db 'ParseLoop',0
    db 'ParseOptions',0
    db 'ParseRun',0
    db 'ParseSourceName',0
    db 'PrintInfo',0
    db 'PrintLoop',0
    db 'PrintUsage',0
    db 'ProcessDest',0
    db 'ProcessDisp16Imm',0
    db 'ProcessDisp8Imm',0
    db 'ProcessOp',0
    db 'ProcessOp16Imm8',0
    db 'ProcessRM',0
    db 'ProcessSrc',0
    db 'ProcessSrcInt8',0
    db 'ReadBinLoop',0
    db 'ReadChar',0
    db 'ReadDigitLoop',0
    db 'ReadHexLoop',0
    db 'ReadLoop',0
    db 'ReadOctLoop',0
    db 'RegTypesDiffer',0
    db 'RightNotUppercase',0
    db 'RunValid',0
    db 'SIZEOF_KWITEM',0
    db 'SIZEOF_OPITEM',0
    db 'SIZE_CODE',0
    db 'SIZE_HEAPDATA',0
    db 'SIZE_STACK',0
    db 'STRING_MAXLEN',0
    db 'Search',0
    db 'SetAlpha1',0
    db 'SetAlpha2',0
    db 'SetHex',0
    db 'SetPrint',0
    db 'SkipCR',0
    db 'SkipChar',0
    db 'SkipDisp0',0
    db 'SkipFF',0
    db 'SkipLF',0
    db 'SkipLeadLoop',0
    db 'SkipPadding1',0
    db 'SkipPadding2',0
    db 'SkipPadding3',0
    db 'SkipPadding4',0
    db 'SkipSpace',0
    db 'SkipTab',0
    db 'SkipUsage',0
    db 'SkipVT',0
    db 'SkipWrite',0
    db 'SrcExtSpecified',0
    db 'SrcSkipDot',0
    db 'StatementLoop',0
    db 'ValidateLead',0
    db 'arg',0
    db 'argc',0
    db 'argexe',0
    db 'argstr',0
    db 'bannerString',0
    db 'binName',0
    db 'binary',0
    db 'chAlpha',0
    db 'chBin',0
    db 'chDigit',0
    db 'chHex',0
    db 'chIdent',0
    db 'chOct',0
    db 'chPrint',0
    db 'chSpace',0
    db 'chTable',0
    db 'chr',0
    db 'chtable',0
    db 'closeCh',0
    db 'code',0
    db 'col',0
    db 'colInc',0
    db 'colStr',0
    db 'compare',0
    db 'comparenc',0
    db 'copyrightString',0
    db 'count',0
    db 'cr',0
    db 'curCh',0
    db 'curCol',0
    db 'curIdent',0
    db 'curLine',0
    db 'curString',0
    db 'curToken',0
    db 'curUint',0
    db 'dest',0
    db 'destLoop',0
    db 'destReg',0
    db 'displacement',0
    db 'dump',0
    db 'dumpAX',0
    db 'dumpBP',0
    db 'dumpBX',0
    db 'dumpCX',0
    db 'dumpDI',0
    db 'dumpDX',0
    db 'dumpLine1',0
    db 'dumpLine2',0
    db 'dumpLine3',0
    db 'dumpLine4',0
    db 'dumpLine5',0
    db 'dumpLine6',0
    db 'dumpSI',0
    db 'dumphex16',0
    db 'dumpu16',0
    db 'echochar',0
    db 'echostring',0
    db 'echouint',0
    db 'echouint32',0
    db 'entryPoint',0
    db 'eoln',0
    db 'errDuplicateRun',0
    db 'errorSourceFile',0
    db 'errorString1',0
    db 'errorString2',0
    db 'errorString3',0
    db 'expectedString1',0
    db 'expectedString2',0
    db 'expectedString3',0
    db 'expr',0
    db 'extOfs',0
    db 'fclose',0
    db 'fcreate',0
    db 'ff',0
    db 'fflush',0
    db 'fmodify',0
    db 'fopen',0
    db 'fpos',0
    db 'fread',0
    db 'fseek',0
    db 'fseekby',0
    db 'fwrite',0
    db 'handle',0
    db 'hashStr1',0
    db 'hashStr10',0
    db 'hashStr11',0
    db 'hashStr12',0
    db 'hashStr13',0
    db 'hashStr14',0
    db 'hashStr15',0
    db 'hashStr16',0
    db 'hashStr17',0
    db 'hashStr18',0
    db 'hashStr2',0
    db 'hashStr3',0
    db 'hashStr4',0
    db 'hashStr5',0
    db 'hashStr6',0
    db 'hashStr7',0
    db 'hashStr8',0
    db 'hashStr9',0
    db 'hashStrEnd',0
    db 'heapData',0
    db 'heapDataEnd',0
    db 'hexDigits',0
    db 'imm',0
    db 'immType',0
    db 'instruction',0
    db 'itemAdd',0
    db 'itemAnd',0
    db 'itemAndIs',0
    db 'itemAt',0
    db 'itemColon',0
    db 'itemComma',0
    db 'itemDiv',0
    db 'itemDot',0
    db 'itemDotDot',0
    db 'itemEq',0
    db 'itemGTEq',0
    db 'itemIsNot',0
    db 'itemLBrace',0
    db 'itemLDoubleBrace',0
    db 'itemLParen',0
    db 'itemLT',0
    db 'itemLTEq',0
    db 'itemMod',0
    db 'itemMul',0
    db 'itemNotEq',0
    db 'itemOr',0
    db 'itemOrIs',0
    db 'itemPostDec',0
    db 'itemPostInc',0
    db 'itemRBrace',0
    db 'itemRDoubleBrace',0
    db 'itemRParen',0
    db 'itemRol',0
    db 'itemRor',0
    db 'itemSRol',0
    db 'itemSRor',0
    db 'itemSShr',0
    db 'itemSemicolon',0
    db 'itemSetAdd',0
    db 'itemSetAnd',0
    db 'itemSetDiv',0
    db 'itemSetMod',0
    db 'itemSetMul',0
    db 'itemSetNot',0
    db 'itemSetOr',0
    db 'itemSetRol',0
    db 'itemSetRor',0
    db 'itemSetSRol',0
    db 'itemSetSRor',0
    db 'itemSetSShr',0
    db 'itemSetShl',0
    db 'itemSetShr',0
    db 'itemSetSub',0
    db 'itemSetTo',0
    db 'itemSetXor',0
    db 'itemShl',0
    db 'itemShr',0
    db 'itemSub',0
    db 'itemUnaryNot',0
    db 'itemXor',0
    db 'keywordTable',0
    db 'keywordTableEnd',0
    db 'leadFlag',0
    db 'lf',0
    db 'line',0
    db 'lineStr',0
    db 'maskGroup',0
    db 'maskOper',0
    db 'maskPrec',0
    db 'maskSubToken',0
    db 'matchLen',0
    db 'memRegs',0
    db 'memSize',0
    db 'minMemString',0
    db 'mrX86RegBP',0
    db 'mrX86RegBX',0
    db 'mrX86RegDI',0
    db 'mrX86RegSI',0
    db 'nextCh',0
    db 'nextCol',0
    db 'nextIdent',0
    db 'nextLine',0
    db 'nextString',0
    db 'nextToken',0
    db 'nextUint',0
    db 'number',0
    db 'ofs',0
    db 'op',0
    db 'opAdd',0
    db 'opAnd',0
    db 'opAndIs',0
    db 'opDiv',0
    db 'opEq',0
    db 'opGT',0
    db 'opGTEq',0
    db 'opLT',0
    db 'opLTEq',0
    db 'opMod',0
    db 'opMul',0
    db 'opNotEq',0
    db 'opOr',0
    db 'opOrIs',0
    db 'opPostDec',0
    db 'opPostInc',0
    db 'opPreDec',0
    db 'opPreInc',0
    db 'opRol',0
    db 'opRor',0
    db 'opSRol',0
    db 'opSRor',0
    db 'opSShr',0
    db 'opShl',0
    db 'opShr',0
    db 'opSub',0
    db 'opTable',0
    db 'opTableEnd',0
    db 'opXor',0
    db 'operHigh',0
    db 'operLow',0
    db 'operSymbol',0
    db 'operandStr',0
    db 'prec00',0
    db 'prec01',0
    db 'prec02',0
    db 'prec03',0
    db 'prec04',0
    db 'prec05',0
    db 'prec06',0
    db 'prec07',0
    db 'prec08',0
    db 'prec09',0
    db 'prec10',0
    db 'prec11',0
    db 'prec12',0
    db 'prec13',0
    db 'prec14',0
    db 'prec15',0
    db 'prefix',0
    db 'reg',0
    db 'repOp',0
    db 'rsvdAsm',0
    db 'rsvdBreak',0
    db 'rsvdElse',0
    db 'rsvdElseIf',0
    db 'rsvdEnd',0
    db 'rsvdEndIf',0
    db 'rsvdEndWhile',0
    db 'rsvdEnum',0
    db 'rsvdExit',0
    db 'rsvdExtends',0
    db 'rsvdFunc',0
    db 'rsvdIdent',0
    db 'rsvdIf',0
    db 'rsvdImplements',0
    db 'rsvdIn',0
    db 'rsvdInterface',0
    db 'rsvdInternal',0
    db 'rsvdMethod',0
    db 'rsvdNext',0
    db 'rsvdNoFrame',0
    db 'rsvdNoRet',0
    db 'rsvdObject',0
    db 'rsvdPrivate',0
    db 'rsvdProgram',0
    db 'rsvdPublic',0
    db 'rsvdReturn',0
    db 'rsvdRun',0
    db 'rsvdSelf',0
    db 'rsvdStruct',0
    db 'rsvdThen',0
    db 'rsvdThenIf',0
    db 'rsvdVar',0
    db 'rsvdVisible',0
    db 'rsvdWhile',0
    db 'run',0
    db 'sel',0
    db 'setAdd',0
    db 'setAnd',0
    db 'setDiv',0
    db 'setMod',0
    db 'setMul',0
    db 'setNot',0
    db 'setOr',0
    db 'setRol',0
    db 'setRor',0
    db 'setSRol',0
    db 'setSRor',0
    db 'setSShr',0
    db 'setShl',0
    db 'setShr',0
    db 'setSub',0
    db 'setSymbol',0
    db 'setTo',0
    db 'setXor',0
    db 'size',0
    db 'source',0
    db 'space',0
    db 'src',0
    db 'srcName',0
    db 'srcReg',0
    db 'str',0
    db 'strAsm',0
    db 'strBreak',0
    db 'strElse',0
    db 'strElseIf',0
    db 'strEnd',0
    db 'strEndIf',0
    db 'strEndWhile',0
    db 'strEnum',0
    db 'strExit',0
    db 'strExtends',0
    db 'strFunc',0
    db 'strFuncStatement',0
    db 'strIdentifier',0
    db 'strIf',0
    db 'strImplements',0
    db 'strIn',0
    db 'strInterface',0
    db 'strInternal',0
    db 'strMethod',0
    db 'strNext',0
    db 'strNoFrame',0
    db 'strNoRet',0
    db 'strObject',0
    db 'strPrivate',0
    db 'strProgram',0
    db 'strPublic',0
    db 'strReturn',0
    db 'strRun',0
    db 'strRunUndefined',0
    db 'strSection',0
    db 'strSelf',0
    db 'strStatement',0
    db 'strStruct',0
    db 'strThen',0
    db 'strThenIf',0
    db 'strTopLevel',0
    db 'strVar',0
    db 'strVisible',0
    db 'strWhile',0
    db 'stringDest',0
    db 'stringLen',0
    db 'suffix',0
    db 'tab',0
    db 'tableName',0
    db 'tkAt',0
    db 'tkColon',0
    db 'tkComma',0
    db 'tkDot',0
    db 'tkDotDot',0
    db 'tkEOF',0
    db 'tkEOL',0
    db 'tkGeneral',0
    db 'tkIdent',0
    db 'tkLBrace',0
    db 'tkLDoubleBrace',0
    db 'tkLParen',0
    db 'tkRBrace',0
    db 'tkRDoubleBrace',0
    db 'tkRParen',0
    db 'tkSemicolon',0
    db 'token',0
    db 'unaryIsNot',0
    db 'unaryNeg',0
    db 'unaryNot',0
    db 'usageString',0
    db 'valChar',0
    db 'valFalse',0
    db 'valInt',0
    db 'valInt8',0
    db 'valLiteral',0
    db 'valString',0
    db 'valTrue',0
    db 'valUint',0
    db 'valUint8',0
    db 'value',0
    db 'vt',0
    db 'x86Adc',0
    db 'x86Add',0
    db 'x86And',0
    db 'x86Call',0
    db 'x86Cbw',0
    db 'x86Clc',0
    db 'x86Cld',0
    db 'x86Cli',0
    db 'x86Cmc',0
    db 'x86Cmp',0
    db 'x86Cmpsb',0
    db 'x86Cmpsw',0
    db 'x86Cwd',0
    db 'x86Dec',0
    db 'x86Div',0
    db 'x86GenOp',0
    db 'x86GenOpImm',0
    db 'x86GenOpMem',0
    db 'x86GenOpMemCL',0
    db 'x86GenOpMemImm',0
    db 'x86GenOpMemReg',0
    db 'x86GenOpReg',0
    db 'x86GenOpRegImm',0
    db 'x86GenOpRegMem',0
    db 'x86GenOpRegReg',0
    db 'x86GenOpSelOfs',0
    db 'x86GenRepOp',0
    db 'x86IDiv',0
    db 'x86IMul',0
    db 'x86Ident',0
    db 'x86In',0
    db 'x86Inc',0
    db 'x86Int',0
    db 'x86Int3',0
    db 'x86Iret',0
    db 'x86JCC',0
    db 'x86Ja',0
    db 'x86Jae',0
    db 'x86Jb',0
    db 'x86Jbe',0
    db 'x86Jc',0
    db 'x86Jcxz',0
    db 'x86Je',0
    db 'x86Jg',0
    db 'x86Jge',0
    db 'x86Jl',0
    db 'x86Jle',0
    db 'x86Jmp',0
    db 'x86Jna',0
    db 'x86Jnae',0
    db 'x86Jnb',0
    db 'x86Jnbe',0
    db 'x86Jnc',0
    db 'x86Jne',0
    db 'x86Jng',0
    db 'x86Jnge',0
    db 'x86Jnl',0
    db 'x86Jnle',0
    db 'x86Jno',0
    db 'x86Jnp',0
    db 'x86Jns',0
    db 'x86Jnz',0
    db 'x86Jo',0
    db 'x86Jp',0
    db 'x86Jpe',0
    db 'x86Jpo',0
    db 'x86Js',0
    db 'x86Jz',0
    db 'x86Lahf',0
    db 'x86Lds',0
    db 'x86Lea',0
    db 'x86Les',0
    db 'x86Lodsb',0
    db 'x86Lodsw',0
    db 'x86Loop',0
    db 'x86Loope',0
    db 'x86Loopne',0
    db 'x86Loopnz',0
    db 'x86Loopz',0
    db 'x86Mov',0
    db 'x86Movsb',0
    db 'x86Movsw',0
    db 'x86Mul',0
    db 'x86Neg',0
    db 'x86Nop',0
    db 'x86Not',0
    db 'x86Or',0
    db 'x86Out',0
    db 'x86Pop',0
    db 'x86Popf',0
    db 'x86Push',0
    db 'x86Pushf',0
    db 'x86RMTable',0
    db 'x86Rcl',0
    db 'x86Rcr',0
    db 'x86Reg16',0
    db 'x86Reg8',0
    db 'x86RegAH',0
    db 'x86RegAL',0
    db 'x86RegAX',0
    db 'x86RegBH',0
    db 'x86RegBL',0
    db 'x86RegBP',0
    db 'x86RegBX',0
    db 'x86RegCH',0
    db 'x86RegCL',0
    db 'x86RegCX',0
    db 'x86RegDH',0
    db 'x86RegDI',0
    db 'x86RegDL',0
    db 'x86RegDX',0
    db 'x86RegSI',0
    db 'x86RegSP',0
    db 'x86Rep',0
    db 'x86Repe',0
    db 'x86Repne',0
    db 'x86Repnz',0
    db 'x86Repz',0
    db 'x86Ret',0
    db 'x86Retf',0
    db 'x86Rol',0
    db 'x86Ror',0
    db 'x86SReg',0
    db 'x86SRegCS',0
    db 'x86SRegDS',0
    db 'x86SRegES',0
    db 'x86SRegSS',0
    db 'x86Sahf',0
    db 'x86Sal',0
    db 'x86Sar',0
    db 'x86Sbb',0
    db 'x86Scasb',0
    db 'x86Scasw',0
    db 'x86Shl',0
    db 'x86Shr',0
    db 'x86SingleByteOpTable',0
    db 'x86Stc',0
    db 'x86Std',0
    db 'x86Sti',0
    db 'x86Stosb',0
    db 'x86Stosw',0
    db 'x86Sub',0
    db 'x86Test',0
    db 'x86Xchg',0
    db 'x86Xlat',0
    db 'x86Xlatb',0
    db 'x86Xor',0
    db 'x86ifAddrSizePrefix',0
    db 'x86ifDisp0',0
    db 'x86ifDisp1',0
    db 'x86ifDisp2',0
    db 'x86ifDisp3',0
    db 'x86ifImm0',0
    db 'x86ifImm1',0
    db 'x86ifImm2',0
    db 'x86ifImm3',0
    db 'x86ifInstPrefix',0
    db 'x86ifModRM',0
    db 'x86ifOpcode',0
    db 'x86ifOperSizePrefix',0
    db 'x86ifSIB',0
    db 'x86ifSegOverride',0
    db 'x86keywordTable',0
    db 'x86keywordTableEnd',0
    ; end   - idents.txt
  testStringsEnd:

  %include "debug.inc"
  %include "chtable.inc"
  %include "rtl.inc"
