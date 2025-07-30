/*Memory Layout Block Diagram
PHYSICAL MEMORY LAYOUT - Test Case 1
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Raw Memory Block (123 bytes total)                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  0x0000023CC557A3E0 ┌─────────────────────────────────────────────────────┐ │
│  Raw Start          │            8 bytes padding                        │ │
│                     │         (unused space)                            │ │
│                     └─────────────────────────────────────────────────────┘ │
│                                                                             │
│  0x0000023CC557A3E8 ┌─────────────────────────────────────────────────────┐ │
│  Pointer Storage    │     Original Pointer Storage (8 bytes)             │ │
│                     │     Contains: 0x0000023CC557A3E0                   │ │
│                     └─────────────────────────────────────────────────────┘ │
│                                                                             │
│  0x0000023CC557A3F0 ┌─────────────────────────────────────────────────────┐ │
│  Aligned Start      │                                                     │ │
│  (USER DATA)        │           User Data Area (100 bytes)               │ │
│                     │         "Hello, aligned memory!"                   │ │
│                     │                                                     │ │
│                     │                    ...                             │ │
│                     └─────────────────────────────────────────────────────┘ │
│                                                                             │
│  0x0000023CC557A454 │              Remaining allocated space              │ │
│  End of allocation  │                    (15 bytes)                       │ │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

ALIGNMENT VERIFICATION:
0x0000023CC557A3F0 = ...11110000 (binary)
Last 4 bits = 0000 → Perfectly aligned to 16-byte boundary ✓

Algorithm Flow Diagram
MALLOC_ALIGNED ALGORITHM FLOW
┌─────────────────────────────────────────────────────────────────────────────┐
│                              START                                         │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│ INPUT VALIDATION                                                            │
│ • size = 100 bytes ✓                                                       │
│ • alignment = 16 bytes ✓                                                   │
│ • Check: 16 & (16-1) = 16 & 15 = 0 ✓ (power of 2)                        │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│ CALCULATE TOTAL SIZE                                                        │
│ total_size = size + alignment - 1 + sizeof(void*)                          │
│ total_size = 100 + 16 - 1 + 8 = 123 bytes                                 │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│ RAW MEMORY ALLOCATION                                                       │
│ raw_ptr = malloc(123) → 0x0000023CC557A3E0                                 │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│ CALCULATE ALIGNMENT BASE                                                    │
│ raw_addr = raw_ptr + sizeof(void*)                                          │
│ raw_addr = 0x0000023CC557A3E0 + 8 = 0x0000023CC557A3E8                    │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│ ALIGNMENT CALCULATION                                                       │
│ temp = raw_addr + alignment - 1                                            │
│ temp = 0x0000023CC557A3E8 + 15 = 0x0000023CC557A3F7                       │
│                                                                             │
│ mask = ~(alignment - 1) = ~15 = 0xFFFFFFFFFFFFFFF0                         │
│                                                                             │
│ aligned_addr = temp & mask                                                  │
│ aligned_addr = 0x0000023CC557A3F7 & 0xFFFFFFFFFFFFFFF0                     │
│ aligned_addr = 0x0000023CC557A3F0                                          │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│ STORE ORIGINAL POINTER                                                      │
│ ptr_slot = aligned_addr - sizeof(void*)                                    │
│ ptr_slot = 0x0000023CC557A3F0 - 8 = 0x0000023CC557A3E8                    │
│ *ptr_slot = raw_ptr (store 0x0000023CC557A3E0)                             │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│ RETURN ALIGNED POINTER                                                      │
│ return 0x0000023CC557A3F0                                                  │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                              END                                           │
└─────────────────────────────────────────────────────────────────────────────┘

Free Operation Flow Diagram
FREE_ALIGNED ALGORITHM FLOW
┌─────────────────────────────────────────────────────────────────────────────┐
│                              START                                         │
│ Input: aligned_ptr = 0x0000023CC557A3F0                                    │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│ CALCULATE POINTER STORAGE LOCATION                                          │
│ ptr_slot = aligned_ptr - sizeof(void*)                                     │
│ ptr_slot = 0x0000023CC557A3F0 - 8 = 0x0000023CC557A3E8                    │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│ RETRIEVE ORIGINAL POINTER                                                   │
│ original_ptr = *ptr_slot                                                    │
│ original_ptr = *(0x0000023CC557A3E8) = 0x0000023CC557A3E0                  │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│ FREE ORIGINAL MEMORY                                                        │
│ free(0x0000023CC557A3E0)                                                   │
└─────────────────────────┬───────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                              END                                           │
└─────────────────────────────────────────────────────────────────────────────┘

Address Space Analysis
Memory Regions and Their Purposes
Address RangeSizePurposeContent0x0000023CC557A3E0 - 0x0000023CC557A3E88 bytesPaddingUnused space0x0000023CC557A3E8 - 0x0000023CC557A3F08 bytesPointer StorageOriginal malloc address0x0000023CC557A3F0 - 0x0000023CC557A454100 bytesUser DataApplication data0x0000023CC557A454 - 0x0000023CC557A45B15 bytesUnusedRemaining allocated space
Key Observations*/

#define _CRT_SECURE_NO_WARNINGS  // Disable MSVC security warnings
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// SIMD alignment requirements
#define SIMD_ALIGN_16  16   //  
#define SIMD_ALIGN_32  32   //  

// Platform-specific format specifiers for uintptr_t
#ifdef _WIN64
#define UINTPTR_FMT "0x%016llx"
#define UINTPTR_DEC_FMT "%lld"
#define PTR_FMT "0x%016llx"
#else
#define UINTPTR_FMT "0x%08lx"
#define UINTPTR_DEC_FMT "%ld"
#define PTR_FMT "0x%08lx"
#endif

// ===== ALIGNED MALLOC IMPLEMENTATION =====
void* malloc_aligned(int size, int alignment) {
    printf("\n=== MALLOC_ALIGNED TRACE ===\n");
    printf("Requested size: %d bytes, alignment: %d bytes\n", size, alignment);

    // Input validation
    if (size <= 0 || alignment <= 0) {
        return NULL;
    }

    // Check if alignment is a power of 2
    if ((alignment & (alignment - 1)) != 0) {
        return NULL;  // alignment must be power of 2
    }

    // Calculate total memory needed
    size_t total_size = size + alignment - 1 + sizeof(void*);
    printf("Total memory needed: %zu bytes\n", total_size);
    printf("  - Requested size: %d\n", size);
    printf("  - Alignment padding (max): %d\n", alignment - 1);
    printf("  - Pointer storage: %zu\n", sizeof(void*));

    // Allocate raw memory
    void* raw_ptr = malloc(total_size);
    if (raw_ptr == NULL) {
        return NULL;
    }

    printf("Raw malloc returned: %p\n", raw_ptr);

    // Calculate aligned address
    uintptr_t raw_addr = (uintptr_t)raw_ptr + sizeof(void*);
    printf("Raw addr + pointer size: " UINTPTR_FMT "\n", (unsigned long long)raw_addr);

    uintptr_t aligned_addr = (raw_addr + alignment - 1) & ~(alignment - 1);
    printf("Aligned address calculation:\n");
    printf("  - (raw_addr + alignment - 1): " UINTPTR_FMT "\n", (unsigned long long)(raw_addr + alignment - 1));
    printf("  - Alignment mask (~(alignment-1)): " UINTPTR_FMT "\n", (unsigned long long)(~(alignment - 1)));
    printf("  - Final aligned address: " UINTPTR_FMT "\n", (unsigned long long)aligned_addr);

    // Store the original pointer just before the aligned memory block
    void** ptr_slot = (void**)(aligned_addr - sizeof(void*));
    printf("Pointer storage location: %p\n", ptr_slot);
    *ptr_slot = raw_ptr;
    printf("Stored original pointer: %p\n", *ptr_slot);

    printf("Returning aligned pointer: %p\n", (void*)aligned_addr);
    return (void*)aligned_addr;
}

void free_aligned(void* aligned_ptr) {
    printf("\n=== FREE_ALIGNED TRACE ===\n");
    printf("Free called with aligned pointer: %p\n", aligned_ptr);

    if (aligned_ptr == NULL) {
        return;
    }

    // Retrieve the original pointer stored just before the aligned block
    void** ptr_slot = (void**)((uintptr_t)aligned_ptr - sizeof(void*));
    printf("Reading original pointer from: %p\n", ptr_slot);

    void* original_ptr = *ptr_slot;
    printf("Retrieved original pointer: %p\n", original_ptr);

    // Free the original memory block
    free(original_ptr);
    printf("Freed original memory block\n");
}

// ===== MEMORY LAYOUT VISUALIZATION =====
void print_memory_layout(void* raw_ptr, void* aligned_ptr, int alignment) {
    printf("\n=== MEMORY LAYOUT VISUALIZATION ===\n");

    uintptr_t raw_addr = (uintptr_t)raw_ptr;
    uintptr_t aligned_addr = (uintptr_t)aligned_ptr;
    uintptr_t ptr_slot_addr = aligned_addr - sizeof(void*);

    printf("Memory Layout:\n");
    printf("+-------------------------------------------------------------+\n");
    printf("| Raw Memory Block (from malloc)                             |\n");
    printf("+-------------------------------------------------------------+\n");
    printf("| Raw Start: %p                                   |\n", raw_ptr);
    printf("+-------------------------------------------------------------+\n");
    printf("| [Potential Padding]                                        |\n");
    printf("+-------------------------------------------------------------+\n");
    printf("| Original Ptr Storage: %p                        |\n", (void*)ptr_slot_addr);
    printf("| Contains: %p                                    |\n", raw_ptr);
    printf("+-------------------------------------------------------------+\n");
    printf("| Aligned Memory Start: %p (USER DATA)            |\n", aligned_ptr);
    printf("| Alignment: %d bytes                                         |\n", alignment);
    printf("+-------------------------------------------------------------+\n");

    printf("\nAddress Analysis:\n");
    printf("- Padding bytes: " UINTPTR_DEC_FMT "\n", (unsigned long long)(ptr_slot_addr - raw_addr));
    printf("- Pointer storage size: %zu bytes\n", sizeof(void*));
    printf("- Alignment check: %p %% %d = " UINTPTR_DEC_FMT " (should be 0)\n",
        aligned_ptr, alignment, (unsigned long long)(aligned_addr % alignment));
}

// ===== TEST FUNCTION =====
int main() {
    printf("=== ALIGNED MEMORY ALLOCATION TEST ===\n");
    printf("sizeof(void*) = %zu bytes\n", sizeof(void*));

    // Test Case 1: 16-byte alignment
    printf("\n");
    for (int i = 0; i < 60; i++) printf("=");
    printf("\n");
    printf("TEST CASE 1: 100 bytes with 16-byte alignment\n");
    for (int i = 0; i < 60; i++) printf("=");
    printf("\n");

    void* ptr1 = malloc_aligned(100, SIMD_ALIGN_16);
    if (ptr1) {
        print_memory_layout(*(void**)((uintptr_t)ptr1 - sizeof(void*)), ptr1, SIMD_ALIGN_16);

        // Test writing to the memory
        printf("\nTesting memory write/read:\n");
        strcpy((char*)ptr1, "Hello, aligned memory!");
        printf("Written and read back: '%s'\n", (char*)ptr1);

        free_aligned(ptr1);
    }

    // Test Case 2: 32-byte alignment
    printf("\n");
    for (int i = 0; i < 60; i++) printf("=");
    printf("\n");
    printf("TEST CASE 2: 256 bytes with 32-byte alignment\n");
    for (int i = 0; i < 60; i++) printf("=");
    printf("\n");

    void* ptr2 = malloc_aligned(256, SIMD_ALIGN_32);
    if (ptr2) {
        print_memory_layout(*(void**)((uintptr_t)ptr2 - sizeof(void*)), ptr2, SIMD_ALIGN_32);

        // Fill with pattern and verify
        printf("\nTesting pattern fill:\n");
        int* int_ptr = (int*)ptr2;
        for (int i = 0; i < 64; i++) {  // 256 bytes / 4 bytes per int = 64 ints
            int_ptr[i] = i * 2;
        }

        printf("Pattern verification (first 10 values): ");
        for (int i = 0; i < 10; i++) {
            printf("%d ", int_ptr[i]);
        }
        printf("\n");

        free_aligned(ptr2);
    }

    // Test Case 3: Edge case - small alignment
    printf("\n");
    for (int i = 0; i < 60; i++) printf("=");
    printf("\n");
    printf("TEST CASE 3: 50 bytes with 8-byte alignment\n");
    for (int i = 0; i < 60; i++) printf("=");
    printf("\n");

    void* ptr3 = malloc_aligned(50, 8);
    if (ptr3) {
        print_memory_layout(*(void**)((uintptr_t)ptr3 - sizeof(void*)), ptr3, 8);
        free_aligned(ptr3);
    }

    // Test Case 4: Error cases
    printf("\n");
    for (int i = 0; i < 60; i++) printf("=");
    printf("\n");
    printf("TEST CASE 4: Error handling\n");
    for (int i = 0; i < 60; i++) printf("=");
    printf("\n");

    printf("Testing invalid alignment (non-power-of-2):\n");
    void* ptr4 = malloc_aligned(100, 15);  // 15 is not power of 2
    printf("Result: %p (should be NULL)\n", ptr4);

    printf("\nTesting negative size:\n");
    void* ptr5 = malloc_aligned(-10, 16);
    printf("Result: %p (should be NULL)\n", ptr5);

    printf("\nTesting free with NULL:\n");
    free_aligned(NULL);  // Should handle gracefully

    printf("\n=== ALL TESTS COMPLETED ===\n");
    return 0;
}