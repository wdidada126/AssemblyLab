#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void mem_init_decreasing(uint8_t* addr, size_t size);

int main() {
    const size_t SIZE = 256;
    uint8_t* buffer = (uint8_t*)malloc(SIZE);

    if (buffer == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    printf("Initializing memory block with decreasing sequence...\n");
    printf("Address: %p\n", (void*)buffer);
    printf("Size: %zu bytes\n\n", SIZE);

    mem_init_decreasing(buffer, SIZE);

    printf("Memory content (first 20 bytes):\n");
    for (int i = 0; i < 20; i++) {
        printf("%02X ", buffer[i]);
        if ((i + 1) % 10 == 0) printf("\n");
    }
    printf("\n\nLast 10 bytes:\n");
    for (int i = (int)SIZE - 10; i < (int)SIZE; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");

    free(buffer);
    return 0;
}

void mem_init_decreasing(uint8_t* addr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        addr[i] = (uint8_t)(size - 1 - i);
    }
}
