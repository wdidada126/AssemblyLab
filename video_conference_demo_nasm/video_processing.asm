; video_processing.asm
; SIMD optimized video processing functions (x64 NASM)
; Compatible with Linux/macOS

section .text
global apply_filter_sse
global mix_audio_sse
global crc32_sse
global yuv422_to_rgb32_sse
global memcpy_sse
global memset_sse
global apply_gain_sse
global frame_diff_sse

; ============================================================================
; Apply Threshold Filter using SSE2
; ============================================================================
; Input: rdi = data pointer, rsi = size, rdx = threshold
; Output: In-place modification
; C prototype: void apply_filter_sse(uint8_t *data, size_t size, uint8_t threshold)

apply_filter_sse:
    push rbx
    push r12
    push r13
    push r14
    
    mov r12, rdi                ; data pointer
    mov r13, rsi                ; size
    mov r14, rdx                ; threshold
    
    ; Broadcast threshold to all bytes in xmm1
    movd xmm1, r14d
    pshuflw xmm1, xmm1, 0
    pshufd xmm1, xmm1, 0
    
    mov rcx, r13                ; restore size
    mov rsi, r12                ; src pointer
    mov rdi, r12                ; dest pointer (in-place)

filter_loop:
    cmp rcx, 0
    je filter_done
    
    ; Process 16 bytes at a time
    cmp rcx, 16
    jl filter_remainder
    
    movdqu xmm0, [rsi]          ; Load 16 bytes
    
    ; Broadcast threshold to all bytes in xmm2 for comparison
    movdqa xmm2, xmm1
    
    ; Compare: xmm2 > xmm0 (threshold > each byte)
    pcmpgtb xmm2, xmm0
    
    ; Mask: keep original byte where >= threshold (not less than)
    pandn xmm2, xmm0
    
    movdqu [rdi], xmm2
    
    add rsi, 16
    add rdi, 16
    sub rcx, 16
    jmp filter_loop
    
filter_remainder:
    mov al, [rsi]
    cmp al, r14b
    jb zero_byte
    mov [rdi], al
    jmp next_byte
zero_byte:
    mov byte [rdi], 0
next_byte:
    inc rsi
    inc rdi
    dec rcx
    jnz filter_remainder
    
filter_done:
    pop r14
    pop r13
    pop r12
    pop rbx
    ret


; ============================================================================
; Audio Mixing using SSE2
; ============================================================================
; Mix two audio streams: dest = (src1 + src2) / 2
; Input: rdi = dest, rsi = src1, rdx = src2, rcx = count

mix_audio_sse:
    push rbx
    push r12
    push r13
    push r14
    
    mov r12, rdi                ; dest
    mov r13, rsi                ; src1
    mov r14, rdx                ; src2
    mov rcx, rcx                ; count (original in rcx)

audio_loop:
    cmp rcx, 0
    je audio_done
    
    cmp rcx, 8
    jl audio_remainder
    
    ; Load 8 samples (16-bit each = 128 bits)
    movdqu xmm0, [r13]
    movdqu xmm1, [r14]
    
    ; Add and halve (saturating)
    paddsw xmm0, xmm1
    psraw xmm0, 1
    
    movdqu [r12], xmm0
    
    add r13, 16
    add r14, 16
    add r12, 16
    sub rcx, 8
    jmp audio_loop
    
audio_remainder:
    mov ax, [r13]
    add ax, [r14]
    sar ax, 1
    mov [r12], ax
    
    add r13, 2
    add r14, 2
    add r12, 2
    dec rcx
    jmp audio_remainder
    
audio_done:
    pop r14
    pop r13
    pop r12
    pop rbx
    ret


; ============================================================================
; CRC32 using SSE4.2 Hardware Instruction
; ============================================================================
; Input: rdi = data pointer, rsi = length
; Output: rax = crc32 result

crc32_sse:
    push rcx
    
    xor eax, eax
    mov rcx, rsi
    
    test rcx, rcx
    jz crc_done
    
crc_loop:
    crc32 eax, byte [rdi]
    inc rdi
    dec rcx
    jnz crc_loop
    
crc_done:
    pop rcx
    ret


; ============================================================================
; YUV422 to RGB32 (Simplified) 
; ============================================================================
; Input: rdi = YUV input, rsi = RGB output, rdx = pixel count

yuv422_to_rgb32_sse:
    push rbx
    push r12
    push r13
    
    mov r12, rdi                ; YUV input
    mov r13, rsi                ; RGB output
    mov rbx, rdx                ; pixel count

yuv_loop:
    cmp rbx, 0
    je yuv_done
    
    ; Load 16 bytes = 8 pixels YUV
    movdqu xmm0, [r12]
    
    ; Simplified: just copy Y as grayscale
    punpcklbw xmm0, xmm0        ; Duplicate
    movdqu [r13], xmm0
    
    add r12, 16
    add r13, 8
    sub rbx, 8
    
    jmp yuv_loop
    
yuv_done:
    pop r13
    pop r12
    pop rbx
    ret


; ============================================================================
; Fast Memcpy using SSE2
; ============================================================================
; Input: rdi = dest, rsi = src, rdx = count

memcpy_sse:
    push rdi
    push rsi
    push rbx
    
    mov rdi, rdi                ; dest
    mov rsi, rsi                ; src
    mov rbx, rdx                ; count
    
    ; Process 16 bytes at a time with SSE2
    mov rcx, rbx
    shr rcx, 4                  ; count / 16
    
memcpy_loop:
    cmp rcx, 0
    je memcpy_remainder
    
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
    mov al, [rsi]
    mov [rdi], al
    inc rdi
    inc rsi
    dec rcx
    jnz memcpy_remainder
    
memcpy_done:
    pop rbx
    pop rsi
    pop rdi
    ret


; ============================================================================
; Fast Memset using SSE2
; ============================================================================
; Input: rdi = ptr, rsi = value (8-bit), rdx = count

memset_sse:
    push rdi
    push rbx
    
    mov rdi, rdi                ; ptr
    movzx eax, sil              ; value
    mov rbx, rdx                ; count
    
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
    
    mov [rdi], al
    inc rdi
    dec rcx
    jnz memset_remainder
    
memset_done:
    pop rbx
    pop rdi
    ret


; ============================================================================
; Apply Gain to Audio using SSE2
; ============================================================================
; Input: rdi = data, rsi = gain (16-bit), rdx = count

apply_gain_sse:
    push rdi
    push rsi
    push rbx
    push r12
    push r13
    
    mov r12, rdi                ; data
    movzx eax, si               ; gain
    mov r13, rdx                ; count
    mov rdi, r12                ; output (in-place)
    mov rsi, r12                ; input
    
    ; Broadcast gain to all 8 positions in XMM register
    movd xmm1, eax
    pshuflw xmm1, xmm1, 0
    pshufd xmm1, xmm1, 0
    
    ; Process 8 samples at a time (16-bit each)
    mov rcx, r13
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
    mov rcx, r13
    and rcx, 7                  ; count % 8
    jz gain_done
    
    mov ax, [rsi]
    imul ax, [rsp+0]            ; gain from stack
    sar ax, 8
    mov [rdi], ax
    
    add rsi, 2
    add rdi, 2
    dec rcx
    jnz gain_remainder
    
gain_done:
    pop r13
    pop r12
    pop rbx
    pop rsi
    pop rdi
    ret


; ============================================================================
; Video Frame Difference (Motion Detection)
; ============================================================================
; Input: rdi = frame1, rsi = frame2, rdx = output diff, rcx = size

frame_diff_sse:
    push rdi
    push rsi
    push rbx
    push r12
    push r13
    push r14
    
    mov r12, rdi                ; frame1
    mov r13, rsi                ; frame2
    mov r14, rdx                ; output
    mov rcx, rcx                ; size (original in rcx)
    
    ; Process 16 bytes at a time
    shr rcx, 4                  ; size / 16
    
diff_loop:
    cmp rcx, 0
    je diff_remainder
    
    ; Load 16 bytes from each frame
    movdqu xmm0, [r12]
    movdqu xmm1, [r13]
    
    ; Absolute difference: abs(a-b) = max(a,b) - min(a,b)
    movdqa xmm2, xmm0
    pcmpgtb xmm2, xmm1          ; xmm2 = (xmm0 > xmm1)
    movdqa xmm3, xmm2
    pandn xmm2, xmm0            ; xmm2 = xmm0 & ~xmm2 = min(xmm0, xmm1)
    pand xmm3, xmm1             ; xmm3 = xmm1 & xmm2 = max part
    por xmm2, xmm3              ; xmm2 = min(xmm0, xmm1)
    pmaxub xmm0, xmm1           ; xmm0 = max(xmm0, xmm1)
    psubb xmm0, xmm2            ; xmm0 = max - min = abs diff
    
    movdqu [r14], xmm0
    
    add r12, 16
    add r13, 16
    add r14, 16
    dec rcx
    jmp diff_loop
    
diff_remainder:
    mov rcx, [rsp+40]           ; original size from stack
    and rcx, 15                 ; size % 16
    jz diff_done
    
    movzx eax, byte [r12]
    movzx ebx, byte [r13]
    sub eax, ebx
    jns diff_positive
    neg eax
diff_positive:
    mov [r14], al
    
    inc r12
    inc r13
    inc r14
    dec rcx
    jnz diff_remainder
    
diff_done:
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rsi
    pop rdi
    ret
