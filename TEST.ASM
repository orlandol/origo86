
  ORG 0x100

  %define SIZE_HEAPDATA (54 * 1024)

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

run:
  cld

  mov     di, heapData
  mov     cx, (SIZE_HEAPDATA / 2)
  xor     ax, ax
  rep stosw
  mov     word [heapData + HEAPITEM_SIG],  SIG_FREEBLOCK
  mov     word [heapData + HEAPITEM_SIZE], SIZE_HEAPDATA

  push    word 8192
  call    alloc
  mov     [ptr1], ax

  push    word 8192
  call    alloc
  mov     [ptr2], ax

push ptr1
call free
push word 8192
call alloc
mov  [ptr1], ax

  push    word 8192
  call    alloc
  mov     [ptr3], ax

  push    word ptr1
  call    free

  push    word ptr2
  call    free

  push    word ptr3
  call    free

  mov     ax, 0x4C00
  int     0x21

ptr1: dw 0
ptr2: dw 0
ptr3: dw 0

;
; func @ alloc( uint size )
;
alloc:
  push    bp
  mov     bp, sp
  %define size bp + 4
  %define result bp - 2
  push    word 0

  mov     cx, [size]
  test    cx, cx
  jz      .Exit

  xor     bx, bx
  mov     si, heapData
  mov     dx, 0xFFFF

 .FindBestFit:
  cmp     si, heapDataEnd
  jae     .DoneBestFit
  cmp     word [si + HEAPITEM_SIG], SIG_FREEBLOCK
  jne     .NextBestFit
  mov     ax, [si + HEAPITEM_SIZE]
  cmp     cx, ax
  ja      .NextBestFit
  cmp     ax, dx
  jae     .NextBestFit
  mov     bx, si
  mov     dx, ax
 .NextBestFit:
  add     si, [si + HEAPITEM_SIZE]
  add     si, SIZEOF_HEAPITEM
  jmp     .FindBestFit
 .DoneBestFit:

  test    bx, bx
  jz      .Exit

  mov     ax, dx
  sub     ax, cx
  cmp     ax, ALLOC_MINSIZE
  ja      .NoGap
  add     cx, ax
 .NoGap:

  cmp     cx, dx
  je      .NoSplit
  lea     di, [bx + SIZEOF_HEAPITEM]
  add     di, cx
  sub     dx, cx
  sub     dx, SIZEOF_HEAPITEM
  mov     word [di + HEAPITEM_SIG], SIG_FREEBLOCK
  mov     [di + HEAPITEM_SIZE], dx
 .NoSplit:

  mov     word [bx + HEAPITEM_SIG], SIG_USEDBLOCK
  mov     [bx + HEAPITEM_SIZE], cx

  add     bx, SIZEOF_HEAPITEM
  mov     [result], bx

 .Exit:
  mov     ax, [result]

  mov     sp, bp
  pop     bp
  ret 2

;
; func free( &ptr )
;
free:
  push    bp
  mov     bp, sp
  %define ptr bp + 4

  mov     bx, [ptr]
  test    bx, bx
  jz      .Exit

  mov     di, [bx]
  sub     di, SIZEOF_HEAPITEM
  cmp     di, heapData
  jb      .Exit
  cmp     di, heapDataEnd
  jae     .Exit

  xor     bx, bx
  mov     si, heapData
 .FindPrev:
  cmp     si, di
  je      .DonePrev
  cmp     word [si + HEAPITEM_SIG], SIG_FREEBLOCK
  jne     .NextPrev
  mov     bx, si
 .NextPrev:
  add     si, [si + HEAPITEM_SIZE]
  add     si, SIZEOF_HEAPITEM
  jmp     .FindPrev
 .DonePrev:

  add     di, [di + HEAPITEM_SIZE]
  add     di, SIZEOF_HEAPITEM

  cmp     di, heapDataEnd
  jae     .SkipMergeNext
  cmp     word [di + HEAPITEM_SIG], SIG_FREEBLOCK
  jne     .SkipMergeNext
  mov     cx, [di + HEAPITEM_SIZE]
  add     cx, SIZEOF_HEAPITEM
  mov     word [di + HEAPITEM_SIG], 0
  add     [si + HEAPITEM_SIZE], cx
 .SkipMergeNext:

  test    bx, bx
  jz      .SkipMergePrev
  mov     cx, [si + HEAPITEM_SIZE]
  add     cx, SIZEOF_HEAPITEM
  mov     word [si + HEAPITEM_SIG], 0
  add     [bx + HEAPITEM_SIZE], cx
  mov     si, bx
 .SkipMergePrev:

  mov     word [si + HEAPITEM_SIG], SIG_FREEBLOCK
  mov     bx, [ptr]
  mov     word [bx], 0

 .Exit:
  mov     sp, bp
  pop     bp
  ret 2

;
; func resize( &ptr, uint newSize )
;
resize:
  push    bp
  mov     bp, sp
  %define ptr bp + 4
  %define newSize bp + 6
  %define ptrNew bp - 2
  push    word [ptr]

  mov     cx, [newSize]
  mov     si, [ptr]
  sub     si, SIZEOF_HEAPITEM

  ; If newSize is 0, release ptr
  test    cx, cx
  jz      .ReleasePtr

  ; If ptr is null, delegate to alloc
  test    si, si
  jnz     .endDelegateAlloc
  push    cx
  call    alloc
  mov     [ptrNew], ax
  jmp     .Exit
 .endDelegateAlloc:

  ; Initialize tmpPtr and nextPtr
  mov     bx, [si + HEAPITEM_SIZE]
  add     bx, si
  add     bx, SIZEOF_HEAPITEM

  mov     di, cx
  add     di, si
  add     di, SIZEOF_HEAPITEM

  ; Attempt to shrink memory in place
 .ShrinkPtr:
  cmp     [si + HEAPITEM_SIZE], cx
  ja      .endShrinkPtr1
  mov     dx, [si + HEAPITEM_SIZE]
  mov     [si + HEAPITEM_SIZE], cx
  jmp     .CheckGap
 .endShrinkPtr1:

  ; Attempt to grow using alloc if tmpPtr is out of heapData bounds
  cmp     bx, heapData
  jb      .GrowAlloc
  cmp     bx, heapDataEnd
  jae     .GrowAlloc

  ; Attempt to grow memory into the next free block
 .GrowPtr:
  cmp     word [bx + HEAPITEM_SIG], SIG_FREEBLOCK
  jne     .endGrowPtr1

  mov     dx, [si + HEAPITEM_SIZE]
  add     dx, [bx + HEAPITEM_SIZE]

  cmp     cx, dx
  jae     .endGrowPtr1

  mov     [si + HEAPITEM_SIZE], cx
 .endGrowPtr1:

  ; Temporarily release free block header, then absorb or merge gap
 .CheckGap:
  mov     ax, dx
  sub     ax, cx

  cmp     di, heapDataEnd
  jae     .Exit

  cmp     word [di + HEAPITEM_SIG], SIG_FREEBLOCK
  jne     .endCheckGap1
  mov     word [bx + HEAPITEM_SIG], 0
  add     ax, SIZEOF_HEAPITEM
 .endCheckGap1:

  cmp     ax, ALLOC_MINSIZE
  jb      .endCheckGap2
  mov     word [di + HEAPITEM_SIG], SIG_FREEBLOCK
  mov     word [di + HEAPITEM_SIZE], ALLOC_MINSIZE + SIZEOF_HEAPITEM
  jmp     .Exit
 .endCheckGap2:

  add     [si + HEAPITEM_SIZE], ax
  jmp     .Exit

  ; If all else fails, attempt to resize using alloc
 .GrowAlloc:
  push    cx
  call    alloc
  mov     bx, ax

  test    bx, bx
  jz      .Exit

  mov     [ptrNew], bx
  mov     di, bx
  mov     cx, [si + HEAPITEM_SIZE]
  add     si, SIZEOF_HEAPITEM
  rep movsb

 .ReleasePtr:
  push    word [ptr]
  call    free

  ; Clean up and exit
 .Exit:
  mov     ax, [ptrNew]

  mov     sp, bp
  pop     bp
  ret 4

maxAvail: dw SIZE_HEAPDATA
memAvail: dw SIZE_HEAPDATA

heapData:
