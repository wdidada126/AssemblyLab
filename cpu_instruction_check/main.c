#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(_WIN32) || defined(_WIN64)
#include <intrin.h>
#elif defined(__GNUC__)
#include <cpuid.h>
#endif

typedef struct {
    const char* name;
    const char* description;
    const char* ai_relevance;
    bool supported;
} InstructionSet;

bool has_cpuid() {
#if defined(_WIN32) || defined(_WIN64)
    return true;
#elif defined(__GNUC__)
    unsigned int eax, ebx, ecx, edx;
    return __get_cpuid(1, &eax, &ebx, &ecx, &edx);
#endif
}

void get_cpuid(unsigned int leaf, unsigned int subleaf, unsigned int* eax, unsigned int* ebx, unsigned int* ecx, unsigned int* edx) {
#if defined(_WIN32) || defined(_WIN64)
    int info[4];
    __cpuidex(info, leaf, subleaf);
    *eax = info[0];
    *ebx = info[1];
    *ecx = info[2];
    *edx = info[3];
#elif defined(__GNUC__)
    __cpuid_count(leaf, subleaf, *eax, *ebx, *ecx, *edx);
#endif
}

int main() {
    printf("===========================================\n");
    printf("       CPU Instruction Set Checker\n");
    printf("===========================================\n\n");

    if (!has_cpuid()) {
        printf("CPUID not supported!\n");
        return 1;
    }

    unsigned int eax, ebx, ecx, edx;
    get_cpuid(1, 0, &eax, &ebx, &ecx, &edx);

    InstructionSet sets[] = {
        {"SSE",      "Streaming SIMD Extensions",    "Basic vector ops for AI inference", false},
        {"SSE2",     "SSE2 (128-bit)",               "Enhanced precision computing", false},
        {"SSE3",     "SSE3",                         "Horizontal operations", false},
        {"SSSE3",    "Supplemental SSE3",           "Additional SIMD instructions", false},
        {"SSE4.1",   "SSE4.1",                       "Dot product, round, insert", false},
        {"SSE4.2",   "SSE4.2",                       "String/text processing", false},
        {"AVX",      "AVX (256-bit)",               "Deep learning inference acceleration", false},
        {"AVX2",     "AVX2 (256-bit)",              "Integer SIMD, BMI2 instructions", false},
        {"AVX-512",  "AVX-512 (512-bit)",           "High-performance AI training", false},
        {"FMA",      "FMA (Fused Multiply-Add)",    "Neural network computations", false},
        {"BMI1",     "BMI1 (Bit Manipulation)",     "Efficient bit operations", false},
        {"BMI2",     "BMI2",                         "Compressed convolution", false},
        {"TSX",      "TSX (Transactional Memory)", "Speculative execution", false},
        {"AES-NI",   "AES-NI",                       "Hardware encryption", false},
        {"RDSEED",   "RDSEED",                       "Hardware random numbers", false},
    };

    int count = sizeof(sets) / sizeof(sets[0]);

    if (edx & (1 << 25)) sets[0].supported = true;
    if (edx & (1 << 26)) sets[1].supported = true;
    if (ecx & (1 <<  0)) sets[2].supported = true;
    if (ecx & (1 <<  9)) sets[3].supported = true;
    if (ecx & (1 << 19)) sets[4].supported = true;
    if (ecx & (1 << 20)) sets[5].supported = true;
    if (ecx & (1 << 28)) sets[6].supported = true;
    if (ecx & (1 <<  5)) sets[7].supported = true;
    if (ecx & (1 << 30)) sets[12].supported = true;
    if (ecx & (1 <<  1)) sets[13].supported = true;
    if (ecx & (1 << 18)) sets[14].supported = true;

    get_cpuid(7, 0, &eax, &ebx, &ecx, &edx);
    if (ebx & (1 <<  5)) sets[6].supported = true;
    if (ebx & (1 <<  3)) sets[7].supported = true;
    if (ebx & (1 << 16)) sets[8].supported = true;
    if (ebx & (1 << 12)) sets[9].supported = true;
    if (ebx & (1 <<  1)) sets[10].supported = true;
    if (ebx & (1 <<  8)) sets[11].supported = true;
    if (ebx & (1 << 14)) sets[14].supported = true;

    get_cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    if (ecx & (1 << 25)) sets[13].supported = true;

    printf("Instruction Set     | AI Relevance\n");
    printf("---------------------|---------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-20s| [%c] %s\n", 
               sets[i].name, 
               sets[i].supported ? 'X' : ' ', 
               sets[i].supported ? "" : sets[i].ai_relevance);
    }

    printf("\n===========================================\n");
    printf("           AI-Optimized Instruction Sets\n");
    printf("===========================================\n\n");

    int ai_score = 0;
    if (sets[6].supported) { printf("★ AVX/AVX2: Found - Good for AI inference\n"); ai_score++; }
    if (sets[8].supported) { printf("★ AVX-512: Found - Best for AI training (FP32/INT8)\n"); ai_score++; }
    if (sets[9].supported) { printf("★ FMA: Found - Accelerates matrix multiplication\n"); ai_score++; }
    if (sets[10].supported && sets[11].supported) { printf("★ BMI1/BMI2: Found - Compression algorithms\n"); ai_score++; }

    printf("\nAI Acceleration Score: %d/4\n", ai_score);

    if (ai_score >= 3) {
        printf("\n✓ CPU well-suited for AI workloads!\n");
    } else if (ai_score >= 1) {
        printf("\n△ CPU partially supports AI acceleration.\n");
    } else {
        printf("\n✗ Consider upgrading CPU for AI workloads.\n");
    }

    return 0;
}
