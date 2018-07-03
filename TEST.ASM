
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

  ; Test case: Validate ptr
  push    word heapData + SIZEOF_HEAPITEM
  push    2048
  call    resize
call checkHeap

  ; Test case: Delegate alloc
  push    word 0
  push    4096
  call    resize
call checkHeap

  ; Test case: Release ptr
  push    ax
  push    word 0
  call    resize
call checkHeap

  ; Test case: Shrink ptr
  push    4096
  call    alloc
  push    ax
  push    2048
  call    resize
  mov     [ptr1], ax
call checkHeap

  push    word ptr1
  call    free
call checkHeap

  ; Test case: Grow ptr in place
  push    2048
  call    alloc
  mov     [ptr1], ax
  push    4196
  call    alloc
  mov     [ptr2], ax
  push    2048
  call    alloc
  mov     [ptr3], ax
  push    ptr2
  call    free
  push    word [ptr1]
  push    word 3072
  call    resize
call checkHeap

  ; Test case: Delegate grow to alloc
  push    word [ptr1]
  push    word 8192
  call    resize
call checkHeap

  push    ptr1
  call    free
  push    ptr2
  call    free
  push    ptr3
  call    free

call checkLeak

  mov     ax, 0x4C00
  int     0x21

ptr1: dw 0
ptr2: dw 0
ptr3: dw 0
ptr4: dw 0

;
; func resize( &ptr, uint newSize )
;
resize:
  push    bp
  mov     bp, sp
  %define newSize bp + 4
  %define ptr     bp + 6
  %define .result bp - 2
  push    word [ptr] ; result = ptr

  ; Register assignments:
  ; ax = gap         bx = tmpPtr
  ; cx = newSize     si = ptr
  ; dx = totalSize   di = nextPtr

  ; Initialize parameters
  mov     cx, [newSize]
  mov     si, [ptr]
  sub     si, SIZEOF_HEAPITEM

  ; Release pointer if newSize is 0
  test    cx, cx
  jnz     .SkipReleasePtr
  lea     ax, [ptr]
  push    ax
  call    free
  mov     word [.result], 0
  jmp     .Exit
  .SkipReleasePtr:

  ; Allocate pointer if ptr is null/invalid
  cmp     si, heapData
  jb      .DelegateAlloc
  cmp     si, heapDataEnd
  jae     .DelegateAlloc
  nop
  jmp     .SkipDelegateAlloc
 .DelegateAlloc:
  push    cx
  call    alloc
  mov     [.result], ax
  jmp     .Exit
 .SkipDelegateAlloc:

  ; Validate ptr header
  cmp    word [si + HEAPITEM_SIG], SIG_USEDBLOCK
  je     .SkipInvalidPtr
  mov    word [.result], 0
  jmp    .Exit
 .SkipInvalidPtr:

  ; Initialize tmpPtr
  mov     bx, [si + HEAPITEM_SIZE]
  add     bx, si
  add     bx, SIZEOF_HEAPITEM

  ; Initialize nextPtr
  mov     di, cx
  add     di, si
  add     di, SIZEOF_HEAPITEM

  ; Shrink pointer in place
  cmp     cx, [si + HEAPITEM_SIZE]
  jae     .SkipShrinkPtr
  mov     dx, [si + HEAPITEM_SIZE]
  mov     [si + HEAPITEM_SIZE], cx
  jmp     .ProcessGap
 .SkipShrinkPtr:

  ; Grow pointer into next free block, if available
 .GrowPtr:
  cmp     bx, heapData
  jb      .Exit ; .GrowAlloc?
  cmp     bx, heapDataEnd
  ja      .Exit ; .GrowAlloc?
  mov     dx, cx      ; ?
  je      .ProcessGap ; ?

  mov     dx, [si + HEAPITEM_SIZE]
  add     dx, [bx + HEAPITEM_SIZE]

  cmp     cx, dx
  jae     .GrowAlloc
  cmp     word [bx + HEAPITEM_SIG], SIG_FREEBLOCK
  jne     .GrowAlloc

  mov     word [bx + HEAPITEM_SIG], 0
  add     dx, SIZEOF_HEAPITEM

  mov     [si + HEAPITEM_SIZE], cx
  jmp     .ProcessGap

  ; Fallback to alloc/copy/free
 .GrowAlloc:
  push    cx
  call    alloc
  test    ax, ax
  jz      .Exit

  mov     [.result], ax

  mov     di, ax
  mov     si, [ptr]
  mov     cx, [si + HEAPITEM_SIZE]
  rep movsb

  lea     ax, [ptr]
  push    ax
  call    free
  jmp     .Exit

  ; Merge or absorb gap
 .ProcessGap:
  mov     ax, dx
  sub     ax, cx
  cmp     ax, ALLOC_MINSIZE
  jae     .MergeGap

  ; Absorb gap into ptr.size
 .AbsorbGap:
  add     [si + HEAPITEM_SIZE], ax
  jmp     .Exit

  ; Merge gap with next free block
 .MergeGap:
  ; Validate nextPtr
  cmp     di, heapData
  jb      .AbsorbGap ; .Exit?
  cmp     di, heapDataEnd
  jae     .AbsorbGap ; .Exit?

  mov     word [di + HEAPITEM_SIG], SIG_FREEBLOCK
  add     word [di + HEAPITEM_SIZE], ALLOC_MINSIZE - SIZEOF_HEAPITEM

  ; Clean up and exit
 .Exit:
  mov     ax, [.result]

  mov     sp, bp
  pop     bp
  ret 4

;
; func checkHeap( string testCase )
;
checkHeap:
  push    bp
  mov     bp, sp

  mov     sp, bp
  pop     bp
  ret 2

;
; func checkLeak()
;
checkLeak:
  push    bp
  mov     bp, sp

  mov     sp, bp
  pop     bp
  ret

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

maxAvail: dw SIZE_HEAPDATA
memAvail: dw SIZE_HEAPDATA

heapData:
