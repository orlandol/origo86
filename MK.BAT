@echo off
nasm mkct.asm -o mkct.com
mkct > chtable.inc
nasm origo86.asm -o origo86.com
