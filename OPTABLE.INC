section .data
opTable:
.itemIsNot:        db "!", 0, 0, 0
                   dw unaryIsNot, 0
.itemNotEq:        db "!=", 0, 0
                   dw opNotEq, 0
.itemMod:          db "%", 0, 0, 0
                   dw opMod, 0
.itemSetMod:       db "%=", 0, 0
                   dw setMod, 0
.itemAnd:          db "&", 0, 0, 0
                   dw opAnd, 0
.itemAndIs:        db "&&", 0, 0
                   dw opAndIs, 0
.itemSetAnd:       db "&=", 0, 0
                   dw setAnd, 0
.itemLParen:       db "(", 0, 0, 0
                   dw tkLParen, 0
.itemRParen:       db ")", 0, 0, 0
                   dw tkRParen, 0
.itemMul:          db "*", 0, 0, 0
                   dw opMul, 0
.itemSetMul:       db "*=", 0, 0
                   dw setMul, 0
.itemAdd:          db "+", 0, 0, 0
                   dw opAdd, 0
.itemPostInc:      db "++", 0, 0
                   dw opPostInc, 0
.itemSetAdd:       db "+=", 0, 0
                   dw setAdd, 0
.itemComma:        db ",", 0, 0, 0
                   dw tkComma, 0
.itemSub:          db "-", 0, 0, 0
                   dw opSub, 0
.itemPostDec:      db "--", 0, 0, 0
                   dw opPostInc, 0
.itemSetSub:       db "-=", 0, 0
                   dw setSub, 0
.itemDot:          db ".", 0, 0, 0
                   dw tkDot, 0
.itemDotDot:       db "..", 0, 0
                   dw tkDotDot, 0
.itemDiv:          db "/", 0, 0, 0
                   dw opDiv, 0
.itemSetDiv:       db "/=", 0, 0
                   dw setDiv, 0
.itemColon:        db ":", 0, 0, 0
                   dw tkColon, 0
.itemSemicolon:    db ";", 0, 0, 0
                   dw tkSemicolon, 0
.itemLT:           db "<", 0, 0, 0
                   dw opLT, 0
.itemSRol:         db "<-<", 0
                   dw opSRol, 0
.itemSetSRol:      db "<-<="
                   dw setSRol, 0
.itemShl:          db "<<", 0, 0
                   dw opShl, 0
.itemRol:          db "<<<", 0
                   dw opRol, 0
.itemSetRol:       db "<<<="
                   dw setRol, 0
.itemSetShl:       db "<<=", 0
                   dw setShl, 0
.itemLTEq:         db "<=", 0, 0
                   dw opLTEq, 0
.itemSetTo:        db "=", 0, 0, 0
                   dw setTo, 0
.itemEq:           db "==", 0, 0
                   dw opEq, 0
.itemSRor:         db ">->", 0
                   dw opSRor, 0
.itemSetSRor:      db ">->="
                   dw setSRor, 0
.itemGTEq:         db ">=", 0, 0
                   dw opGTEq, 0
.itemShr:          db ">>", 0, 0
                   dw opShr, 0
.itemSetShr:       db ">>=", 0
                   dw setShr, 0
.itemRor:          db ">>>", 0
                   dw opRor, 0
.itemSetRor:       db ">>>="
                   dw setRor, 0
.itemAt:           db "@", 0, 0, 0
                   dw tkAt, 0
.itemLBrace:       db "[", 0, 0, 0
                   dw tkLBrace, 0
.itemLDoubleBrace: db "[[", 0, 0
                   dw tkLDoubleBrace, 0
.itemRBrace:       db "]", 0, 0, 0
                   dw tkRBrace, 0
.itemRDoubleBrace: db "]]", 0, 0
                   dw tkRDoubleBrace, 0
.itemXor:          db "^", 0, 0, 0
                   dw opXor, 0
.itemSetXor:       db "^=", 0, 0
                   dw setXor, 0
.itemOr:           db "|", 0, 0, 0
                   dw opOr, 0
.itemSetOr:        db "|=", 0, 0
                   dw setOr, 0
.itemSShr:         db "|>>", 0
                   dw opSShr, 0
.itemSetSShr:      db "|>>="
                   dw setSShr, 0
.itemOrIs:         db "||", 0, 0
                   dw opOrIs, 0
.itemUnaryNot:     db "~", 0, 0, 0
                   dw unaryNot, 0
.itemSetNot:       db "~=", 0, 0
                   dw setNot, 0
opTableEnd:
