#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#define _CRT_SECURE_NO_WARNINGS

typedef struct freelist {
    struct freelist* next;  // Fix: Added 'struct' keyword
} freelist;

typedef struct {
    void* pool;
    size_t numblocks;
    size_t blocksize;
    freelist* list;
} MemoryPool;

MemoryPool* create_pool(size_t blocksize, size_t numblocks) {  // Fix: Swapped parameter order to match usage
    MemoryPool* poolptr = (MemoryPool*)malloc(sizeof(MemoryPool));
    if (!poolptr) return NULL;

    poolptr->pool = malloc(numblocks * blocksize);
    if (!poolptr->pool) {  // Fix: Added null check for pool allocation
        free(poolptr);
        return NULL;
    }

    poolptr->numblocks = numblocks;
    poolptr->blocksize = blocksize;
    poolptr->list = NULL;  // Fix: Initialize list to NULL

    uint8_t* baseptr = (uint8_t*)poolptr->pool;
    for (size_t i = 0; i < numblocks; i++) {  // Fix: Changed int to size_t
        freelist* node = (freelist*)(baseptr + i * blocksize);
        node->next = poolptr->list;
        poolptr->list = node;
    }
    return poolptr;
}

void* pool_alloc(MemoryPool* pool) {
    if (!pool || !pool->list)
        return NULL;
    freelist* block = pool->list;
    pool->list = pool->list->next;
    return (void*)block;
}

void pool_free(MemoryPool* pool, void* ptr) {
    if (!pool || !ptr) return;
    freelist* node = (freelist*)ptr;
    node->next = pool->list;
    pool->list = node;
}

// Fix: Added missing pool_free_count function
size_t pool_free_count(MemoryPool* pool) {
    if (!pool) return 0;
    size_t count = 0;
    freelist* current = pool->list;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

// Fix: Added missing destroy_pool function
void destroy_pool(MemoryPool* pool) {
    if (!pool) return;
    free(pool->pool);
    free(pool);
}

// Test program
int main() {
    printf("Simple Memory Pool Test\n");
    printf("=======================\n\n");

    // Create pool: 64-byte blocks, 8 blocks total
    MemoryPool* pool = create_pool(64, 8);
    if (!pool) {
        printf("Failed to create pool\n");
        return 1;
    }
    printf("Created pool: %zu blocks of %zu bytes each\n",
        pool->numblocks, pool->blocksize);
    printf("Free blocks: %zu\n\n", pool_free_count(pool));

    // Allocate some blocks
    void* blocks[5];
    printf("Allocating 5 blocks:\n");
    for (int i = 0; i < 5; i++) {
        blocks[i] = pool_alloc(pool);
        if (blocks[i]) {
            printf("  Block %d: %p\n", i, blocks[i]);
            // Write some test data
           // sprintf((char*)blocks[i], "Data in block %d", i);
            snprintf((char*)blocks[i], pool->blocksize, "Data in block %d", i);

        }
        else {
            printf("  Failed to allocate block %d\n", i);
        }
    }
    printf("Free blocks after allocation: %zu\n\n", pool_free_count(pool));

    // Verify data integrity
    printf("Reading back data:\n");
    for (int i = 0; i < 5; i++) {
        if (blocks[i]) {
            printf("  Block %d: '%s'\n", i, (char*)blocks[i]);
        }
    }
    printf("\n");

    // Free some blocks
    printf("Freeing blocks 1 and 3:\n");
    pool_free(pool, blocks[1]);
    pool_free(pool, blocks[3]);
    printf("Free blocks after freeing: %zu\n\n", pool_free_count(pool));

    // Allocate again (should reuse freed blocks)
    printf("Allocating 2 new blocks:\n");
    void* new1 = pool_alloc(pool);
    void* new2 = pool_alloc(pool);
    printf("  New block 1: %p\n", new1);
    printf("  New block 2: %p\n", new2);
    printf("Free blocks: %zu\n\n", pool_free_count(pool));

    // Test exhaustion
    printf("Allocating remaining blocks:\n");
    int allocated = 0;
    void* ptr;
    while ((ptr = pool_alloc(pool)) != NULL) {
        printf("  Allocated: %p\n", ptr);
        allocated++;
    }
    printf("Total additional blocks allocated: %d\n", allocated);
    printf("Free blocks (should be 0): %zu\n\n", pool_free_count(pool));

    // Try to allocate when pool is exhausted
    printf("Trying to allocate when pool is full:\n");
    ptr = pool_alloc(pool);
    if (ptr) {
        printf("  Unexpected allocation: %p\n", ptr);
    }
    else {
        printf("  Allocation correctly failed (pool exhausted)\n");
    }

    destroy_pool(pool);
    printf("\nPool destroyed successfully\n");
    return 0;
}