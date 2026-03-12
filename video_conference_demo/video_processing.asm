; video_processing.asm
; SIMD optimized video processing functions (x64 MASM)

.code

; ============================================================================
; Apply Threshold Filter using SSE2
; ============================================================================
; Input:  rcx = data pointer, rdx = size, r8 = threshold
; Output: In-place modification

apply_filter_sse PROC
    push rbx
    push rsi
    push rdi
    
    mov rsi, rcx                ; data pointer
    mov rdi, rcx                ; output (in-place)
    mov rbx, rdx                ; size
    mov eax, r8d                ; threshold
    
    ; Broadcast threshold to all bytes in xmm1
    movd xmm1, eax
    pshuflw xmm1, xmm1, 0
    pshufd xmm1, xmm1, 0
    
    mov rcx, rbx                ; restore size
    
filter_loop:
    cmp rcx, 0
    je filter_done
    
    ; Process 16 bytes at a time
    cmp rcx, 16
    jl filter_remainder
    
    movdqu xmm0, [rsi]          ; Load 16 bytes
    
    ; Compare: xmm0 > xmm1
    pcmpgtb xmm2, xmm0          ; Compare each byte
    
    ; Mask: keep original byte where >= threshold
    pand xmm2, xmm0
    
    movdqu [rdi], xmm2
    
    add rsi, 16
    add rdi, 16
    sub rcx, 16
    jmp filter_loop
    
filter_remainder:
    mov al, byte ptr [rsi]
    cmp al, r8b
    jb zero_byte
    mov byte ptr [rdi], al
    jmp next_byte
zero_byte:
    mov byte ptr [rdi], 0
next_byte:
    inc rsi
    inc rdi
    dec rcx
    jnz filter_remainder
    
filter_done:
    pop rdi
    pop rsi
    pop rbx
    ret
apply_filter_sse ENDP


; ============================================================================
; Audio Mixing using SSE2
; ============================================================================
; Mix two audio streams: dest = (src1 + src2) / 2

mix_audio_sse PROC
    push rbx
    push rsi
    push rdi
    
    mov rsi, rdx                ; src1
    mov rdi, r8                 ; src2
    mov rbx, rcx                ; dest
    mov rcx, r9                 ; count
    
audio_loop:
    cmp rcx, 0
    je audio_done
    
    cmp rcx, 8
    jl audio_remainder
    
    ; Load 8 samples (16-bit each = 128 bits)
    movdqu xmm0, [rsi]
    movdqu xmm1, [rdi]
    
    ; Add and halve (saturating)
    paddsw xmm0, xmm1
    psraw xmm0, 1
    
    movdqu [rbx], xmm0
    
    add rsi, 16
    add rdi, 16
    add rbx, 16
    sub rcx, 8
    jmp audio_loop
    
audio_remainder:
    mov ax, word ptr [rsi]
    add ax, word ptr [rdi]
    sar ax, 1
    mov word ptr [rbx], ax
    
    add rsi, 2
    add rdi, 2
    add rbx, 2
    dec rcx
    jmp audio_remainder
    
audio_done:
    pop rdi
    pop rsi
    pop rbx
    ret
mix_audio_sse ENDP


; ============================================================================
; CRC32 using SSE4.2 Hardware Instruction
; ============================================================================

crc32_sse PROC
    push rsi
    push rcx
    
    xor eax, eax
    mov esi, ecx
    mov ecx, edx
    
    test ecx, ecx
    jz crc_done
    
crc_loop:
    crc32 eax, byte ptr [rsi]
    inc rsi
    dec ecx
    jnz crc_loop
    
crc_done:
    pop rcx
    pop rsi
    ret
crc32_sse ENDP


; ============================================================================
; YUV422 to RGB32 (Simplified) 
; ============================================================================

yuv422_to_rgb32_sse PROC
    push rbx
    push rsi
    push rdi
    
    mov rsi, rcx                ; YUV
    mov rdi, rdx                ; RGB
    mov rbx, r8                 ; width * height
    
yuv_loop:
    cmp rbx, 0
    je yuv_done
    
    ; Load 16 bytes = 8 pixels YUV
    movdqu xmm0, [rsi]
    
    ; Simplified: just copy Y as grayscale
    ; Full conversion needs more complex operations
    punpcklbw xmm0, xmm0        ; Duplicate
    movdqu [rdi], xmm0
    
    add rsi, 16
    add rdi, 8
    sub rbx, 8
    
    jmp yuv_loop
    
yuv_done:
    pop rdi
    pop rsi
    pop rbx
    ret
yuv422_to_rgb32_sse ENDP


; ============================================================================
; Fast Memcpy using SSE2 (Non-temporal store)
; ============================================================================
; Input: rcx = dest, rdx = src, r8 = count
; Output: Data copied to dest

memcpy_sse PROC
    push rdi
    push rsi
    push rbx
    
    mov rdi, rcx                ; dest
    mov rsi, rdx                ; src
    mov rbx, r8                 ; count
    
    ; Process 64 bytes at a time with AVX (if available)
    ; For SSE2, process 16 bytes
    mov rcx, rbx
    shr rcx, 4                  ; count / 16
    
memcpy_loop:
    cmp rcx, 0
    je memcpy_remainder
    
    ; Prefetch ahead (optional)
    ; prefetcht0 [rsi + 64]
    
    ; Load and store 16 bytes
    movdqu xmm0, [rsi]
    movdqu [rdi], xmm0
    
    add rsi, 16
    add rdi, 16
    dec rcx
    jmp memcpy_loop
    
memcpy_remainder:
    mov rcx, rbx
    and rcx, 15                 ; count % 16
    jz memcpy_done
    
    ; Handle remaining bytes
    mov al, byte ptr [rsi]
    mov byte ptr [rdi], al
    inc rdi
    inc rsi
    dec rcx
    jnz memcpy_remainder
    
memcpy_done:
    pop rbx
    pop rsi
    pop rdi
    ret
memcpy_sse ENDP


; ============================================================================
; Fast Memset using SSE2
; ============================================================================
; Input: rcx = ptr, rdx = value (8-bit), r8 = count
; Output: Memory filled with value

memset_sse PROC
    push rdi
    push rbx
    
    mov rdi, rcx                ; ptr
    movzx eax, dl               ; value
    mov rbx, r8                 ; count
    
    ; Broadcast byte to all positions in XMM register
    movd xmm0, eax
    pshuflw xmm0, xmm0, 0
    pshufd xmm0, xmm0, 0
    
    ; Process 16 bytes at a time
    mov rcx, rbx
    shr rcx, 4                  ; count / 16
    
memset_loop:
    cmp rcx, 0
    je memset_remainder
    
    movdqu [rdi], xmm0
    
    add rdi, 16
    dec rcx
    jmp memset_loop
    
memset_remainder:
    mov rcx, rbx
    and rcx, 15                 ; count % 16
    jz memset_done
    
    mov byte ptr [rdi], al
    inc rdi
    dec rcx
    jnz memset_remainder
    
memset_done:
    pop rbx
    pop rdi
    ret
memset_sse ENDP


; ============================================================================
; Apply Gain to Audio using SSE2
; ============================================================================
; Input: rcx = data, rdx = gain (16-bit), r8 = count
; Output: In-place modification

apply_gain_sse PROC
    push rdi
    push rsi
    push rbx
    
    mov rsi, rcx                ; data
    mov rdi, rcx                ; output (in-place)
    movzx eax, dx               ; gain
    mov rbx, r8                 ; count
    
    ; Broadcast gain to all 8 positions in XMM register
    movd xmm1, eax
    pshuflw xmm1, xmm1, 0
    pshufd xmm1, xmm1, 0
    
    ; Process 8 samples at a time (16-bit each)
    mov rcx, rbx
    shr rcx, 3                  ; count / 8
    
gain_loop:
    cmp rcx, 0
    je gain_remainder
    
    ; Load 8 samples
    movdqu xmm0, [rsi]
    
    ; Sign extend to 32-bit and multiply
    movdqa xmm2, xmm0
    punpcklwd xmm2, xmm2
    pmullw xmm2, xmm1
    
    movdqa xmm3, xmm0
    punpckhwd xmm3, xmm3
    pmullw xmm3, xmm1
    
    ; Shift right to divide (fixed-point)
    psrlw xmm2, 8
    psrlw xmm3, 8
    
    ; Pack back to 16-bit
    packuswb xmm2, xmm3
    
    ; Move 8 bytes
    movdqu [rdi], xmm2
    
    add rsi, 16
    add rdi, 16
    dec rcx
    jmp gain_loop
    
gain_remainder:
    mov rcx, rbx
    and rcx, 7                  ; count % 8
    jz gain_done
    
    mov ax, word ptr [rsi]
    imul ax, dx
    sar ax, 8
    mov word ptr [rdi], ax
    
    add rsi, 2
    add rdi, 2
    dec rcx
    jnz gain_remainder
    
gain_done:
    pop rbx
    pop rsi
    pop rdi
    ret
apply_gain_sse ENDP


; ============================================================================
; Video Frame Difference (Motion Detection)
; ============================================================================
; Input: rcx = frame1, rdx = frame2, r8 = output diff, r9 = size
; Output: Absolute difference in output

frame_diff_sse PROC
    push rdi
    push rsi
    push rbx
    
    mov rsi, rcx                ; frame1
    mov rdi, rdx                ; frame2
    mov rbx, r8                 ; output
    mov rcx, r9                 ; size
    
    ; Process 16 bytes at a time
    shr rcx, 4                  ; size / 16
    
diff_loop:
    cmp rcx, 0
    je diff_remainder
    
    ; Load 16 bytes from each frame
    movdqu xmm0, [rsi]
    movdqu xmm1, [rdi]
    
    ; Absolute difference using PABSB (SSSE3) or manual method
    ; Manual: abs(a-b) = max(a,b) - min(a,b)
    movdqa xmm2, xmm0
    pcmpgtb xmm2, xmm1          ; xmm2 = (xmm0 > xmm1)
    movdqa xmm3, xmm2
    pandn xmm2, xmm0            ; xmm2 = xmm0 & ~xmm2 = min(xmm0, xmm1)
    pand xmm3, xmm1             ; xmm3 = xmm1 & xmm2 = max part
    por xmm2, xmm3             ; xmm2 = min(xmm0, xmm1)
    pmaxub xmm0, xmm1          ; xmm0 = max(xmm0, xmm1)
    psubb xmm0, xmm2           ; xmm0 = max - min = abs diff
    
    movdqu [rbx], xmm0
    
    add rsi, 16
    add rdi, 16
    add rbx, 16
    dec rcx
    jmp diff_loop
    
diff_remainder:
    mov rcx, r9
    and rcx, 15                 ; size % 16
    jz diff_done
    
    movzx eax, byte ptr [rsi]  ; Zero-extend to 32-bit
    movzx ebx, byte ptr [rdi]
    sub eax, ebx
    jns diff_positive
    neg eax
diff_positive:
    mov byte ptr [rbx], al
    
    inc rsi
    inc rdi
    inc rbx
    dec rcx
    jnz diff_remainder
    
diff_done:
    pop rbx
    pop rsi
    pop rdi
    ret
frame_diff_sse ENDP

END
