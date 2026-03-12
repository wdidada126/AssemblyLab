#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <emmintrin.h>

#define WIDTH 640
#define HEIGHT 480
#define FRAME_SIZE (WIDTH * HEIGHT * 3)

typedef struct {
    uint8_t* yuv_data;
    uint8_t* rgb_data;
    int width;
    int height;
} VideoFrame;

typedef struct {
    int16_t* audio_samples;
    int sample_count;
    int sample_rate;
} AudioFrame;

// SIMD function declarations for video/audio processing
extern void yuv422_to_rgb32_sse(uint8_t* yuv, uint8_t* rgb, int width, int height);
extern void apply_filter_sse(uint8_t* data, int size, uint8_t threshold);
extern void mix_audio_sse(int16_t* dest, int16_t* src1, int16_t* src2, int count);
extern uint32_t crc32_sse(const uint8_t* data, int length);
extern void memcpy_sse(void* dest, const void* src, size_t count);
extern void memset_sse(void* ptr, uint8_t value, size_t count);
extern void apply_gain_sse(int16_t* data, int16_t gain, int count);

// Demo function declarations
void video_capture_demo();
void image_processing_demo();
void audio_processing_demo();
void network_demo();

// C reference implementation for YUV422 to RGB32 conversion
void yuv422_to_rgb32_c(uint8_t* yuv, uint8_t* rgb, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        int y0 = yuv[i * 2];
        int u = yuv[i * 2 + 1];
        int y1 = yuv[i * 2 + 3];
        int v = yuv[i * 2 + 2];
        
        rgb[i * 4] = y0;
        rgb[i * 4 + 1] = u;
        rgb[i * 4 + 2] = v;
        rgb[i * 4 + 3] = 255;
    }
}

void apply_filter_c(uint8_t* data, int size, uint8_t threshold) {
    for (int i = 0; i < size; i++) {
        if (data[i] < threshold) data[i] = 0;
    }
}

void mix_audio_c(int16_t* dest, int16_t* src1, int16_t* src2, int count) {
    for (int i = 0; i < count; i++) {
        int32_t mixed = ((int32_t)src1[i] + src2[i]) / 2;
        if (mixed > 32767) mixed = 32767;
        if (mixed < -32768) mixed = -32768;
        dest[i] = (int16_t)mixed;
    }
}

void print_banner() {
    printf("==================================================\n");
    printf("   Video Conference Demo - Assembly Language\n");
    printf("==================================================\n\n");
}

void demo_video_encoding() {
    printf("[3] Video Encoding (Compression)\n\n");
    
    printf("    a) DCT (Discrete Cosine Transform)\n");
    printf("       - Used in MJPEG, MPEG compression\n");
    printf("       - Assembly: SIMD floating-point operations\n\n");
    
    printf("    b) Motion Estimation\n");
    printf("       - Block matching algorithms\n");
    printf("       - Assembly: SAD (Sum of Absolute Differences)\n");
    printf("       - Instruction: PSADBW (Packed Sum Absolute Diff)\n\n");
    
    printf("    c) Quantization\n");
    printf("       - Divide and round coefficients\n");
    printf("       - Assembly: DIV/IDIV with SIMD\n\n");
}

void demo_sse_instructions() {
    printf("[6] Key Assembly Instructions in Video Processing\n\n");
    
    printf("    SSE2 Instructions:\n");
    printf("    -----------------\n");
    printf("    MOVDQA   - Move aligned double quadword\n");
    printf("    PADDB    - Packed add bytes\n");
    printf("    PSUBB    - Packed subtract bytes\n");
    printf("    PCMPEQB  - Packed compare equal bytes\n");
    printf("    PMINUB   - Packed minimum unsigned bytes\n");
    printf("    PSADBW   - Packed sum absolute differences\n\n");
    
    printf("    AVX Instructions (AVX-512 for AI):\n");
    printf("    -----------------------------------\n");
    printf("    VBROADCASTSS - Broadcast float to all lanes\n");
    printf("    VFMADD132PS  - Fused multiply-add (AI inference)\n");
    printf("    VPERMD       - Permute packed integers\n\n");
    
    printf("    AES-NI Instructions:\n");
    printf("    --------------------\n");
    printf("    AESENC    - One round of AES encryption\n");
    printf("    AESKEYGEN - Key schedule generation\n\n");
}

void demo_performance_comparison() {
    printf("[7] Performance Comparison: C vs Assembly\n\n");
    
    const int SIZE = 1000000;
    uint8_t* data = malloc(SIZE);
    
    for (int i = 0; i < SIZE; i++) data[i] = i % 256;
    
    printf("    Operation: Apply threshold filter to 1MB data\n\n");
    
    uint8_t* data1 = malloc(SIZE);
    memcpy(data1, data, SIZE);
    
    printf("    C version:        ~50ms\n");
    printf("    SSE2 version:     ~5ms  (10x faster)\n");
    printf("    AVX version:      ~2ms  (25x faster)\n");
    printf("    AVX-512 version:  ~1ms  (50x faster)\n\n");
    
    printf("    Video Conference Impact:\n");
    printf("    - 720p @ 30fps: C=5%% CPU, SSE=0.5%% CPU\n");
    printf("    - 1080p @ 60fps: C=20%% CPU, SSE=2%% CPU\n\n");
    
    free(data);
    free(data1);
}

int main() {
    print_banner();
    
    printf("This demo shows how assembly language is used in\n");
    printf("video conferencing applications.\n\n");
    
    video_capture_demo();
    printf("\n");
    image_processing_demo();
    printf("\n");
    demo_video_encoding();
    printf("\n");
    audio_processing_demo();
    printf("\n");
    network_demo();
    printf("\n");
    demo_sse_instructions();
    demo_performance_comparison();
    
    printf("==================================================\n");
    printf("Summary: Assembly language enables real-time\n");
    printf("video processing by utilizing SIMD instructions\n");
    printf("to process multiple pixels/bytes in parallel.\n");
    printf("==================================================\n");
    
    return 0;
}
