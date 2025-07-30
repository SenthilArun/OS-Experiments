#define _CRT_SECURE_NO_WARNINGS  // Disable MSVC security warnings
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
 

// Buffer pool implementation
#define BUFFER_POOL_SIZE 1024
#define PACKET_SIZE 1500

typedef struct {
    char data[PACKET_SIZE];
    int length;
    int in_use;
} packet_buffer_t;

static packet_buffer_t buffer_pool[BUFFER_POOL_SIZE];
static int next_buffer = 0;

packet_buffer_t* get_buffer() {
    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        int idx = (next_buffer + i) % BUFFER_POOL_SIZE;
        if (!buffer_pool[idx].in_use) {
            buffer_pool[idx].in_use = 1;
            next_buffer = (idx + 1) % BUFFER_POOL_SIZE;
            return &buffer_pool[idx];
        }
    }
    return NULL; // Pool exhausted
}

void release_buffer(packet_buffer_t* buf) {
    buf->in_use = 0;
}




// Test functions
void test_basic_allocation() {
    printf("Test 1: Basic allocation and release\n");

    packet_buffer_t* buf = get_buffer();
    assert(buf != NULL);
    assert(buf->in_use == 1);

    // Test writing to buffer
    strcpy(buf->data, "Test packet data");
    buf->length = (int)strlen(buf->data);

    printf("  + Buffer allocated successfully\n");
    printf("  + Data written: %s (length: %d)\n", buf->data, buf->length);

    release_buffer(buf);
    assert(buf->in_use == 0);
    printf("  + Buffer released successfully\n\n");
}

void test_multiple_allocations() {
    printf("Test 2: Multiple allocations\n");

    packet_buffer_t* buffers[100];

    // Allocate multiple buffers
    for (int i = 0; i < 100; i++) {
        buffers[i] = get_buffer();
        assert(buffers[i] != NULL);
        sprintf(buffers[i]->data, "Packet %d", i);
        buffers[i]->length = (int)strlen(buffers[i]->data);
    }

    printf("  + Allocated 100 buffers successfully\n");

    // Verify data integrity
    for (int i = 0; i < 100; i++) {
        char expected[32];
        sprintf(expected, "Packet %d", i);
        assert(strcmp(buffers[i]->data, expected) == 0);
    }

    printf("  + Data integrity verified\n");

    // Release all buffers
    for (int i = 0; i < 100; i++) {
        release_buffer(buffers[i]);
    }

    printf("  + All buffers released\n\n");
}

void test_pool_exhaustion() {
    printf("Test 3: Pool exhaustion\n");

    packet_buffer_t* buffers[BUFFER_POOL_SIZE + 1];
    int allocated = 0;

    // Try to allocate all buffers
    for (int i = 0; i < BUFFER_POOL_SIZE + 1; i++) {
        buffers[i] = get_buffer();
        if (buffers[i] != NULL) {
            allocated++;
            sprintf(buffers[i]->data, "Buffer %d", i);
            buffers[i]->length = (int)strlen(buffers[i]->data);
        }
    }

    printf("  + Allocated %d buffers (max: %d)\n", allocated, BUFFER_POOL_SIZE);
    assert(allocated == BUFFER_POOL_SIZE);
    assert(buffers[BUFFER_POOL_SIZE] == NULL);
    printf("  + Pool exhaustion handled correctly\n");

    // Release all allocated buffers
    for (int i = 0; i < allocated; i++) {
        release_buffer(buffers[i]);
    }

    printf("  + All buffers released after exhaustion\n\n");
}

void test_allocation_after_release() {
    printf("Test 4: Allocation after partial release\n");

    packet_buffer_t* buffers[50];

    // Allocate 50 buffers
    for (int i = 0; i < 50; i++) {
        buffers[i] = get_buffer();
        assert(buffers[i] != NULL);
        sprintf(buffers[i]->data, "Initial %d", i);
        buffers[i]->length = (int)strlen(buffers[i]->data);
    }

    // Release every other buffer
    for (int i = 0; i < 50; i += 2) {
        release_buffer(buffers[i]);
        buffers[i] = NULL;
    }

    printf("  + Released 25 buffers\n");

    // Try to allocate 25 new buffers
    int reallocated = 0;
    for (int i = 0; i < 25; i++) {
        packet_buffer_t* new_buf = get_buffer();
        if (new_buf != NULL) {
            sprintf(new_buf->data, "Reallocated %d", i);
            new_buf->length = (int)strlen(new_buf->data);
            reallocated++;
        }
    }

    printf("  + Reallocated %d buffers from freed slots\n", reallocated);
    assert(reallocated == 25);

    // Clean up remaining buffers
    for (int i = 1; i < 50; i += 2) {
        if (buffers[i] != NULL) {
            release_buffer(buffers[i]);
        }
    }

    printf("  + Cleanup completed\n\n");
}

void test_buffer_reuse() {
    printf("Test 5: Buffer reuse pattern\n");

    // Test rapid allocation/release cycles
    for (int cycle = 0; cycle < 10; cycle++) {
        packet_buffer_t* temp_buffers[100];

        // Allocate
        for (int i = 0; i < 100; i++) {
            temp_buffers[i] = get_buffer();
            assert(temp_buffers[i] != NULL);
            sprintf(temp_buffers[i]->data, "Cycle %d Buffer %d", cycle, i);
            temp_buffers[i]->length = (int)strlen(temp_buffers[i]->data);
        }

        // Release immediately
        for (int i = 0; i < 100; i++) {
            release_buffer(temp_buffers[i]);
        }
    }

    printf("  + Completed 10 allocation/release cycles of 100 buffers each\n\n");
}

void test_data_isolation() {
    printf("Test 6: Data isolation between buffers\n");

    packet_buffer_t* buf1 = get_buffer();
    packet_buffer_t* buf2 = get_buffer();

    assert(buf1 != NULL && buf2 != NULL);
    assert(buf1 != buf2); // Different buffer instances

    strcpy(buf1->data, "Buffer 1 data");
    strcpy(buf2->data, "Buffer 2 data");
    buf1->length = (int)strlen(buf1->data);
    buf2->length = (int)strlen(buf2->data);

    // Verify data hasn't been corrupted
    assert(strcmp(buf1->data, "Buffer 1 data") == 0);
    assert(strcmp(buf2->data, "Buffer 2 data") == 0);

    printf("  + Data isolation verified\n");

    release_buffer(buf1);
    release_buffer(buf2);

    printf("  + Buffers released\n\n");
}

int count_free_buffers() {
    int free_count = 0;
    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        if (!buffer_pool[i].in_use) {
            free_count++;
        }
    }
    return free_count;
}

void test_pool_statistics() {
    printf("Test 7: Pool statistics\n");

    int initial_free = count_free_buffers();
    printf("  Initial free buffers: %d\n", initial_free);

    packet_buffer_t* test_buffers[200];

    // Allocate 200 buffers
    for (int i = 0; i < 200; i++) {
        test_buffers[i] = get_buffer();
        assert(test_buffers[i] != NULL);
    }

    int after_alloc = count_free_buffers();
    printf("  Free buffers after allocating 200: %d\n", after_alloc);
    assert(after_alloc == initial_free - 200);

    // Release 100 buffers
    for (int i = 0; i < 100; i++) {
        release_buffer(test_buffers[i]);
    }

    int after_partial_release = count_free_buffers();
    printf("  Free buffers after releasing 100: %d\n", after_partial_release);
    assert(after_partial_release == initial_free - 100);

    // Release remaining buffers
    for (int i = 100; i < 200; i++) {
        release_buffer(test_buffers[i]);
    }

    int final_free = count_free_buffers();
    printf("  Final free buffers: %d\n", final_free);
    assert(final_free == initial_free);

    printf("  + Pool statistics verified\n\n");
}

int main() {
    printf("Buffer Pool Test Suite\n");
    printf("======================\n");
    printf("Pool size: %d buffers\n", BUFFER_POOL_SIZE);
    printf("Packet size: %d bytes\n\n", PACKET_SIZE);

    // Initialize pool (ensure all buffers start as unused)
    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        buffer_pool[i].in_use = 0;
        buffer_pool[i].length = 0;
    }

    test_basic_allocation();
    test_multiple_allocations();
    test_pool_exhaustion();
    test_allocation_after_release();
    test_buffer_reuse();
    test_data_isolation();
    test_pool_statistics();

    printf("All tests passed! [PASS]\n");
    printf("Buffer pool implementation is working correctly.\n");

    return 0;
}