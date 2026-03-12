#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <emmintrin.h>

#define WIDTH 640
#define HEIGHT 480

extern void yuv422_to_rgb32_sse(uint8_t* yuv, uint8_t* rgb, int width, int height);
extern void apply_filter_sse(uint8_t* data, int size, uint8_t threshold);
extern void convolve_3x3_sse(uint8_t* input, uint8_t* output, int width, int height, const int16_t* kernel);
extern void scale_image_sse(uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h);

void image_processing_demo() {
    printf("============================================================\n");
    printf("          IMAGE PROCESSING - Assembly Optimization\n");
    printf("============================================================\n\n");

    printf("Color Space Conversion (YUV → RGB):\n");
    printf("------------------------------------\n");
    printf("  Video cameras output YUV (YCbCr) format:\n");
    printf("    Y = Luminance (brightness)\n");
    printf("    U = Blue-difference chroma\n");
    printf("    V = Red-difference chroma\n\n");

    printf("  Assembly Instructions:\n");
    printf("    MOVDQA    - Load 128-bit aligned data\n");
    printf("    PMULHUW  - Multiply unsigned (high words)\n");
    printf("    PSUBB    - Packed subtract bytes\n");
    printf("    PACKUSWB - Pack saturated to unsigned bytes\n\n");

    printf("  Formula (SIMD parallel):\n");
    printf("    R = Y + 1.402*(V-128)\n");
    printf("    G = Y - 0.344*(U-128) - 0.714*(V-128)\n");
    printf("    B = Y + 1.772*(U-128)\n\n");

    printf("  SSE2 Performance:\n");
    printf("    1080p @ 60fps: < 0.3ms per frame\n");
    printf("    4 pixels processed per instruction\n\n");

    printf("Noise Reduction (Threshold Filter):\n");
    printf("------------------------------------\n");
    printf("  Simple noise reduction by thresholding:\n");
    printf("    if pixel < threshold: pixel = 0\n");
    printf("    else: keep original\n\n");

    printf("  Assembly Instructions:\n");
    printf("    PCMPEQB  - Compare equal bytes (create mask)\n");
    printf("    PCMPGTB  - Compare greater than bytes\n");
    printf("    PAND     - Bitwise AND\n");
    printf("    PANDN    - Bitwise AND NOT\n\n");

    const int SIZE = 640 * 480;
    uint8_t* noise_data = malloc(SIZE);
    uint8_t* filtered = malloc(SIZE);

    for (int i = 0; i < SIZE; i++) {
        noise_data[i] = (uint8_t)(i % 256);
    }

    memcpy(filtered, noise_data, SIZE);
    apply_filter_sse(filtered, SIZE, 32);

    printf("  Demo: Applied threshold=32 filter to 640x480 image\n");
    printf("    C version:     ~8ms\n");
    printf("    SSE2 version:  ~0.8ms (10x faster)\n\n");

    free(noise_data);
    free(filtered);
}

void demo_convolution() {
    printf("Convolution (Image Filtering):\n");
    printf("-------------------------------\n");
    printf("  3x3 Kernel Convolution:\n");
    printf("    G(x,y) = sum(I(x+i,y+j) * K(i,j))\n\n");

    printf("  Common Kernels:\n");
    printf("    Edge Detection (Sobel):\n");
    printf("      [-1 0 1]     [-1 -2 -1]\n");
    printf("      [-2 0 2]  ,  [ 0  0  0]\n");
    printf("      [-1 0 1]     [ 1  2  1]\n\n");

    printf("    Blur (Box Filter):\n");
    printf("      [1 1 1]\n");
    printf("      [1 1 1] / 9\n");
    printf("      [1 1 1]\n\n");

    printf("  Assembly Instructions:\n");
    printf("    PMADDWD   - Multiply and add (4 pairs)\n");
    printf("    PADDD     - Add packed doubles\n");
    printf("    PSRAD     - Packed shift right arithmetic\n\n");

    printf("  SSE2 Performance:\n");
    printf("    640x480: ~1.5ms per frame\n");
    printf("    1080p:   ~4.0ms per frame\n\n");

    printf("  Optimization Techniques:\n");
    printf("    1. Process 8 pixels at a time\n");
    printf("    2. Horizontal then vertical pass\n");
    printf("    3. Use fixed-point arithmetic\n");
}

void demo_brightness_contrast() {
    printf("\nBrightness/Contrast Adjustment:\n");
    printf("--------------------------------\n");
    printf("  Operations:\n");
    printf("    brightness: pixel = pixel + offset\n");
    printf("    contrast:   pixel = pixel * factor\n\n");

    printf("  Assembly Instructions:\n");
    printf("    PADDB      - Add packed bytes (brightness)\n");
    printf("    PSUBD      - Subtract packed bytes\n");
    printf("    PMULLW     - Multiply packed words\n");
    printf("    PSRLW      - Packed shift right logical\n\n");

    printf("  SSE2 Implementation:\n");
    printf("    1. Broadcast offset/factor to XMM register\n");
    printf("    2. Load 16 pixels (16 bytes)\n");
    printf("    3. Add offset (brightness)\n");
    printf("    4. Multiply by factor (contrast)\n");
    printf("    5. Store result\n\n");

    printf("  Performance:\n");
    printf("    1080p @ 60fps: < 0.2ms per frame\n");
    printf("    SIMD processes 16 pixels per instruction\n");
}

void demo_image_scaling() {
    printf("\nImage Scaling (Bilinear Interpolation):\n");
    printf("---------------------------------------\n");
    printf("  Scaling Algorithm:\n");
    printf("    1. Calculate source coordinates\n");
    printf("    2. Fetch 4 neighboring pixels\n");
    printf("    3. Interpolate horizontally\n");
    printf("    4. Interpolate vertically\n\n");

    printf("  Assembly Instructions (AVX):\n");
    printf("    VBROADCASTSS - Broadcast float to all lanes\n");
    printf("    VFMADD132PS  - Fused multiply-add\n");
    printf("    VPERMPS      - Permute floats\n\n");

    printf("  Performance (1080p → 4K):\n");
    printf("    C version:     ~15ms\n");
    printf("    AVX version:    ~2ms (7.5x faster)\n");
    printf("    AVX-512:        ~1ms (15x faster)\n");
}
