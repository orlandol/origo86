;
; Sorted general keyword token list
;
keywordTable:
dw strAsm, rsvdAsm
dw strBreak, rsvdBreak
dw strElse, rsvdElse
dw strElseIf, rsvdElseIf
dw strEnd, rsvdEnd
dw strEndIf, rsvdEndIf
dw strEndWhile, rsvdEndWhile
dw strEnum, rsvdEnum
dw strExit, rsvdExit
dw strExtends, rsvdExtends
dw strFunc, rsvdFunc
dw strIf, rsvdIf
dw strImplements, rsvdImplements
dw strIn, rsvdIn
dw strInterface, rsvdInterface
dw strInternal, rsvdInternal
dw strMethod, rsvdMethod
dw strNext, rsvdNext
dw strNoFrame, rsvdNoFrame
dw strNoRet, rsvdNoRet
dw strObject, rsvdObject
dw strPrivate, rsvdPrivate
dw strProgram, rsvdProgram
dw strPublic, rsvdPublic
dw strReturn, rsvdReturn
dw strRun, rsvdRun
dw strSelf, rsvdSelf
dw strStruct, rsvdStruct
dw strThen, rsvdThen
dw strThenIf, rsvdThenIf
dw strVar, rsvdVar
dw strVisible, rsvdVisible
dw strWhile, rsvdWhile
keywordTableEnd:

;
; Keyword strings
;
strEnum:       db "enum",0
strInternal:   db "internal",0
strObject:     db "object",0
strProgram:    db "program",0
strPublic:     db "public",0
strStruct:     db "struct",0
strVisible:    db "visible",0
strExtends:    db "extends",0
strPrivate:    db "private",0
strInterface:  db "interface",0
strImplements: db "implements",0
strMethod:     db "method",0
strSelf:       db "self",0
strFunc:       db "func",0
strAsm:        db "asm",0
strNoRet:      db "noret",0
strNoFrame:    db "noframe",0
strReturn:     db "return",0
strVar:        db "var",0
strRun:        db "run",0
strEnd:        db "end",0
strIf:         db "if",0
strThen:       db "then",0
strThenIf:     db "thenif",0
strElseIf:     db "elseif",0
strElse:       db "else",0
strEndIf:      db "endif",0
strWhile:      db "while",0
strNext:       db "next",0
strBreak:      db "break",0
strEndWhile:   db "endwhile",0
strIn:         db "in",0
strExit:       db "exit",0

;
; Sorted x86 assembler keyword list
;
x86keywordTable:
x86keywordTableEnd:
