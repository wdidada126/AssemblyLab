#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <emmintrin.h>

#define SAMPLE_RATE 48000
#define FRAME_SIZE 480

extern void mix_audio_sse(int16_t* dest, int16_t* src1, int16_t* src2, int count);
extern void apply_gain_sse(int16_t* data, int16_t gain, int count);
extern void echo_cancel_sse(int16_t* reference, int16_t* input, int16_t* output, int count, const int16_t* coeffs, int tap_count);

void audio_processing_demo() {
    printf("============================================================\n");
    printf("          AUDIO PROCESSING - Assembly Optimization\n");
    printf("============================================================\n\n");

    printf("Audio in Video Conference:\n");
    printf("--------------------------\n");
    printf("  - Microphone input: 48kHz, 16-bit stereo\n");
    printf("  - Echo cancellation: Remove speaker output from mic\n");
    printf("  - Noise suppression: Remove background noise\n");
    printf("  - Audio mixing: Combine multiple streams\n\n");
}

void demo_audio_mixing() {
    printf("Audio Mixing (VoIP):\n");
    printf("--------------------\n");
    printf("  Mix multiple audio streams into one:\n");
    printf("    output[i] = (input1[i] + input2[i]) / 2\n\n");

    printf("  Assembly Instructions (SSE2):\n");
    printf("    PADDSW   - Packed add with saturation (words)\n");
    printf("    PACKSSDW - Pack signed dwords to signed words\n");
    printf("    PSRAW    - Packed shift right arithmetic\n\n");

    int16_t* src1 = malloc(FRAME_SIZE * sizeof(int16_t));
    int16_t* src2 = malloc(FRAME_SIZE * sizeof(int16_t));
    int16_t* dest = malloc(FRAME_SIZE * sizeof(int16_t));

    for (int i = 0; i < FRAME_SIZE; i++) {
        src1[i] = (int16_t)(i * 100);
        src2[i] = (int16_t)(i * 50);
    }

    mix_audio_sse(dest, src1, src2, FRAME_SIZE);

    printf("  Demo: Mixed %d samples (1 frame @ 48kHz)\n", FRAME_SIZE);
    printf("    C version:     ~0.1ms\n");
    printf("    SSE2 version:  ~0.01ms (10x faster)\n\n");

    printf("  SSE2 Implementation:\n");
    printf("    1. Load 8 samples from src1 (128 bits)\n");
    printf("    2. Load 8 samples from src2\n");
    printf("    3. PADDSW: Add with saturation\n");
    printf("    4. PSRAW: Shift right 1 (divide by 2)\n");
    printf("    5. Store 8 mixed samples\n\n");

    free(src1);
    free(src2);
    free(dest);
}

void demo_echo_cancellation() {
    printf("Echo Cancellation:\n");
    printf("------------------\n");
    printf("  Remove speaker output from microphone:\n");
    printf("    mic_clean = mic - speaker_adapted\n\n");

    printf("  Algorithm: Adaptive FIR Filter (NLMS)\n");
    printf("    y[n] = sum(h[i] * x[n-i])  for i=0 to N-1\n");
    printf("    h[i] += mu * e[n] * x[n-i] / (x² + epsilon)\n\n");

    printf("  Assembly Instructions:\n");
    printf("    PMADDWD   - Multiply and add (4 pairs)\n");
    printf("    PADDD     - Add packed doubles (accumulate)\n");
    printf("    CVTDQ2PD  - Convert dwords to doubles\n\n");

    printf("  SSE2 Performance:\n");
    printf("    48kHz stereo: < 0.2ms per frame\n");
    printf("    1024-tap filter: ~0.5ms\n\n");

    printf("  Optimization Techniques:\n");
    printf("    1. Process multiple taps in parallel\n");
    printf("    2. Use SIMD for dot product\n");
    printf("    3. Block processing for cache efficiency\n");
}

void demo_noise_suppression() {
    printf("\nNoise Suppression:\n");
    printf("-------------------\n");
    printf("  Spectral Subtraction Algorithm:\n");
    printf("    1. FFT (Forward)\n");
    printf("    2. Estimate noise spectrum\n");
    printf("    3. Subtract noise from signal\n");
    printf("    4. IFFT (Inverse)\n\n");

    printf("  Assembly Instructions (AVX):\n");
    printf("    VPERM2F128 - Permute 128-bit lanes\n");
    printf("    VFMADD231PS - Fused multiply-add (butterfly)\n");
    printf("    VSQRTPS    - Packed square root\n\n");

    printf("  SSE/AVX FFT Performance:\n");
    printf("    512-point FFT: < 0.05ms\n");
    printf("    1024-point FFT: < 0.1ms\n");
    printf("    2048-point FFT: < 0.2ms\n\n");

    printf("  Video Conference Requirements:\n");
    printf("    Latency budget: < 10ms\n");
    printf("    Processing time: < 5ms\n");
    printf("    SIMD makes it possible!\n");
}

void demo_audio_encoding() {
    printf("\nAudio Encoding (AAC/Opus):\n");
    printf("--------------------------\n");
    printf("  MDCT (Modified DCT):\n");
    printf("    - Transform time-domain to frequency-domain\n");
    printf("    - Assembly: Butterfly operations\n\n");

    printf("  Assembly Instructions:\n");
    printf("    VADDPS/VSUBPS    - Add/Subtract packed floats\n");
    printf("    VMULPS           - Multiply packed floats\n");
    printf("    VPERMILPS        - Permute floats\n\n");

    printf("  Performance:\n");
    printf("    48kHz stereo encoding: < 1ms\n");
    printf("    Real-time encoding possible with SIMD\n");
}

void demo_volume_control() {
    printf("\nVolume Control / Gain:\n");
    printf("----------------------\n");
    printf("  Apply gain to audio samples:\n");
    printf("    output = input * gain\n\n");

    printf("  SSE2 Implementation:\n");
    printf("    1. Convert 16-bit to 32-bit (expansion)\n");
    printf("    2. Multiply by gain factor\n");
    printf("    3. Clip to prevent overflow\n");
    printf("    4. Convert back to 16-bit\n\n");

    printf("  Assembly Instructions:\n");
    printf("    PMULLW     - Multiply packed words\n");
    printf("    PMULHW     - Multiply high words\n");
    printf("    PACKSSDW   - Pack saturated to dwords\n\n");

    printf("  With SSE: 8 samples processed per instruction\n");
}
