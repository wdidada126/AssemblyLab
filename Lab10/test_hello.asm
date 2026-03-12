; test_hello.asm
; 在螢幕第8行、第3列顯示 "welcome to masm!" (綠色字，黑色底)

.8086
.model small
.stack 100h             ; 堆疊大小 256 bytes 就夠

.data
    msg     db  'welcome to masm!', 0

.code
start:
    mov     ax, @data
    mov     ds, ax

    mov     dh, 8           ; 行號 (從 0 開始)
    mov     dl, 3           ; 列號 (從 0 開始)
    mov     cl, 0Ah         ; 顏色屬性：淺綠色字 + 黑色底 (可改成 07h 白底黑字)

    mov     si, offset msg
    call    show_str

    ; 等待按任意鍵
    mov     ah, 1
    int     21h

    ; 正常結束程式
    mov     ax, 4c00h
    int     21h

;------------------------------------------------
; 顯示字串子程序
; 輸入： dh=行, dl=列, cl=顏色屬性, ds:si → 字串 (0 結尾)
;------------------------------------------------
show_str proc
    push    ax
    push    bx
    push    cx
    push    dx
    push    si
    push    di
    push    bp
    push    es

    mov     ax, 0B800h
    mov     es, ax

    ; 計算顯示位置：行×160 + 列×2
    mov     al, 160         ; 每行 80 字元 × 2 bytes = 160
    mul     dh              ; dh = 行號 → ax = 行偏移
    mov     di, ax

    mov     al, dl
    shl     al, 1           ; 列號 × 2
    add     di, ax          ; di = 最終偏移

    mov     ah, cl          ; 顏色屬性搬到 ah

show_loop:
    mov     al, ds:[si]     ; 取字元到 al
    or      al, al
    jz      show_done       ; 遇到 0 結束

    mov     es:[di], ax     ; 一次寫入 word：低 byte=字元, 高 byte=屬性

    inc     si
    add     di, 2
    jmp     show_loop

show_done:
    pop     es
    pop     bp
    pop     di
    pop     si
    pop     dx
    pop     cx
    pop     bx
    pop     ax
    ret
show_str endp

end start