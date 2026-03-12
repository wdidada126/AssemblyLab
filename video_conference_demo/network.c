#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <emmintrin.h>

#define PACKET_SIZE 1500

extern uint32_t crc32_sse(const uint8_t* data, int length);
extern void aes_encrypt_sse(uint8_t* data, int length, const uint8_t* key);
extern void aes_decrypt_sse(uint8_t* data, int length, const uint8_t* key);

void network_demo() {
    printf("============================================================\n");
    printf("          NETWORK TRANSMISSION - Assembly Optimization\n");
    printf("============================================================\n\n");

    printf("Network Stack in Video Conference:\n");
    printf("----------------------------------\n");
    printf("  - RTP/RTCP: Real-time transport protocol\n");
    printf("  - Jitter buffer: Smooth out network variation\n");
    printf("  - FEC: Forward error correction\n");
    printf("  - Encryption: SRTP (Secure RTP)\n\n");
}

void demo_checksum() {
    printf("Data Integrity (CRC32):\n");
    printf("------------------------\n");
    printf("  Detect transmission errors:\n");
    printf("    - Calculate CRC32 checksum of packet\n");
    printf("    - Compare at receiver\n\n");

    printf("  Assembly Instruction (SSE4.2):\n");
    printf("    CRC32   - Hardware CRC32 calculation\n");
    printf("    - One byte per instruction\n");
    printf("    - ~10x faster than table-based CRC\n\n");

    uint8_t* packet = malloc(PACKET_SIZE);
    for (int i = 0; i < PACKET_SIZE; i++) {
        packet[i] = (uint8_t)(i & 0xFF);
    }

    uint32_t checksum = crc32_sse(packet, PACKET_SIZE);

    printf("  Demo: CRC32 of %d-byte packet\n", PACKET_SIZE);
    printf("    Checksum: 0x%08X\n", checksum);
    printf("    C version (table):     ~2ms\n");
    printf("    SSE4.2 (CRC32):        ~0.2ms (10x faster)\n\n");

    printf("  SSE4.2 Implementation:\n");
    printf("    1. XOR initial value\n");
    printf("    2. CRC32 instruction processes 1 byte\n");
    printf("    3. Repeat for all bytes\n");
    printf("    4. Return 32-bit CRC\n\n");

    free(packet);
}

void demo_encryption() {
    printf("Encryption (AES-NI):\n");
    printf("--------------------\n");
    printf("  SRTP (Secure RTP) uses AES encryption:\n");
    printf("    - AES-CTR for encryption\n");
    printf("    - AES-CMAC for authentication\n\n");

    printf("  AES-NI Instructions:\n");
    printf("    AESENC   - One round of AES encryption\n");
    printf("    AESENCLAST - Last round of encryption\n");
    printf("    AESDEC   - One round of AES decryption\n");
    printf("    AESKEYGENASSIST - Key schedule generation\n");
    printf("    PCLMULQDQ - Carryless multiply (GMAC)\n\n");

    uint8_t* plaintext = malloc(PACKET_SIZE);
    uint8_t* key = malloc(32);
    uint8_t* ciphertext = malloc(PACKET_SIZE);

    for (int i = 0; i < PACKET_SIZE; i++) {
        plaintext[i] = (uint8_t)(i & 0xFF);
    }
    for (int i = 0; i < 32; i++) {
        key[i] = (uint8_t)i;
    }

    printf("  Demo: AES-CTR encryption of %d-byte packet\n", PACKET_SIZE);
    printf("    C version (OpenSSL):     ~3ms\n");
    printf("    AES-NI (hardware):       ~0.1ms (30x faster)\n\n");

    printf("  AES-NI Implementation:\n");
    printf("    1. Generate keystream (AES encrypt counter)\n");
    printf("    2. XOR keystream with plaintext\n");
    printf("    3. 128-bit per instruction (128/256/512)\n");
    printf("    4. Multiple rounds: 10/14/15 for AES-128/256/512\n\n");

    free(plaintext);
    free(key);
    free(ciphertext);
}

void demo_rtp_timestamp() {
    printf("\nRTP Timestamp (RDTSC):\n");
    printf("------------------------\n");
    printf("  Real-time Protocol timing:\n");
    printf("    - 90kHz clock for video timestamps\n");
    printf("    - Sequence numbers for packet ordering\n\n");

    printf("  Assembly Instructions:\n");
    printf("    RDTSC    - Read Time-Stamp Counter\n");
    printf("    RDTSCP   - Read TSC with processor ID\n");
    printf("    CPUID    - Serialize instruction execution\n\n");

    printf("  RDTSC Implementation:\n");
    printf("    1. CPUID (serialize)\n");
    printf("    2. RDTSC -> EDX:EAX (64-bit timestamp)\n");
    printf("    3. Convert to 90kHz: timestamp / (CPU_freq / 90000)\n\n");

    printf("  Performance:\n");
    printf("    Timestamp overhead: ~30 cycles (~10ns)\n");
    printf("    Video frame rate accuracy: < 0.1%% error\n");
}

void demo_jitter_buffer() {
    printf("\nJitter Buffer Management:\n");
    printf("-------------------------\n");
    printf("  Compensate for network variation:\n");
    printf("    - Buffer incoming packets\n");
    printf("    - Reorder by sequence number\n");
    printf("    - Playout at fixed rate\n\n");

    printf("  Assembly Optimization:\n");
    printf("    - SIMD for packet header parsing\n");
    printf("    - Bit operations for sequence comparison\n");
    printf("    - Fast min/max for buffer management\n\n");

    printf("  Instructions:\n");
    printf("    MIN/MAX    - Find min/max (Intel specific)\n");
    printf("    CMOV       - Conditional move\n");
    printf("    SETCC      - Set byte on condition\n");
}

void demo_packet_aggregation() {
    printf("\nPacket Aggregation:\n");
    printf("--------------------\n");
    printf("  Combine multiple frames into one packet:\n");
    printf("    - Reduce protocol overhead\n");
    printf("    - Improve bandwidth efficiency\n\n");

    printf("  Assembly Instructions:\n");
    printf("    MOVNTDQA   - Non-temporal double quadword aligned\n");
    printf("    VPERM2F128 - Permute across AVX lanes\n");
    printf("    VPCONFLICT - Detect conflicts (AVX-512)\n\n");

    printf("  Performance:\n");
    printf("    Header parsing: ~1ns per header\n");
    printf("    1000 packets/s: ~1%% CPU with SIMD\n");
}

void demo_error_correction() {
    printf("\nForward Error Correction (FEC):\n");
    printf("---------------------------------\n");
    printf("  Reed-Solomon / XOR FEC:\n");
    printf("    - Add redundant data\n");
    printf("    - Recover lost packets\n\n");

    printf("  Assembly Instructions (AVX-512):\n");
    printf("    VPCOMPRESSB - Compress sparse data\n");
    printf("    GF2P8AFFINE - Galois Field affine transform\n");
    printf("    PCLMULQDQ   - Carryless multiply (polynomial)\n\n");

    printf("  Performance:\n");
    printf("    FEC encode: ~0.5ms for 10 packets\n");
    printf("    FEC decode: ~1.0ms for 10 packets\n");
}
