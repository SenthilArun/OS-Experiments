#include<stdio.h>
#include<iostream>
#include<stdint.h>
#include<vector>
#include <assert.h>
using namespace std;

//Id based allocations for required size in sequence of buffer
class Allocator { 
public:
    vector<int> bufpool;
    size_t capacity;
    Allocator(int n) {
        bufpool.resize(n); 
        capacity = n;
    }   

    int allocate(size_t size, size_t id) {

 
        //assert(size >= this->capacity);
       
        for (int start = 0;start <= capacity - size;start++) {
            
                bool is_slotavailable = true;
                for (size_t  i = start;i < start + size;i++) {
                    if (bufpool[i] != 0) {
                        is_slotavailable = false;
                        break;
                    }
                }
                if (is_slotavailable) {
                    for (size_t  i = start;i < start + size;i++) {
                          bufpool[i] = id;
                    }
                    return start;
                }
        }
        return -1;
    }
    int freeMemory(int mID) {
        int freememory = 0;
        for (size_t i = 0; i < capacity; i++) {
            if (bufpool[i] == mID) {
                bufpool[i] = 0;
                freememory++;
            }
        }
        return freememory;
    }

    void printBuffer() {
        cout << "[";
        for (size_t i = 0; i < capacity; i++) {
            cout << bufpool[i];
            if (i < capacity - 1) cout << ",";
        }
        cout << "]" << endl;
    }
};

// Example usage and test cases
 
#include <cassert>

int main() {
    // Test Case 1: Basic functionality
    Allocator allocator(10);

    // Allocate 1 unit with mID 1
    int result1 = allocator.allocate(1, 1);
    std::cout << "Allocate(1, 1): " << result1 << std::endl;  // Expected: 0
    allocator.printBuffer();  // [1,0,0,0,0,0,0,0,0,0]
    assert(result1 == 0);

    // Allocate 1 unit with mID 2
    int result2 = allocator.allocate(1, 2);
    std::cout << "Allocate(1, 2): " << result2 << std::endl;  // Expected: 1
    allocator.printBuffer();  // [1,2,0,0,0,0,0,0,0,0]
    assert(result2 == 1);

    // Allocate 1 unit with mID 1 (same mID, different block)
    int result3 = allocator.allocate(1, 1);
    std::cout << "Allocate(1, 1): " << result3 << std::endl;  // Expected: 2
    allocator.printBuffer();  // [1,2,1,0,0,0,0,0,0,0]
    assert(result3 == 2);

    // Free memory with mID 2
    int freed1 = allocator.freeMemory(2);
    std::cout << "Free(2): " << freed1 << std::endl;  // Expected: 1
    allocator.printBuffer();  // [1,0,1,0,0,0,0,0,0,0]
    assert(freed1 == 1);

    // Allocate 3 units with mID 3
    int result4 = allocator.allocate(3, 3);
    std::cout << "Allocate(3, 3): " << result4 << std::endl;  // CORRECTED: Expected 3, not 1!
    allocator.printBuffer();  // [1,0,1,3,3,3,0,0,0,0]
    assert(result4 == 3);  // FIXED: Should be 3, not 1

    // Free memory with mID 1 (should free 2 units)
    int freed2 = allocator.freeMemory(1);
    std::cout << "Free(1): " << freed2 << std::endl;  // Expected: 2
    allocator.printBuffer();  // [0,0,0,3,3,3,0,0,0,0]
    assert(freed2 == 2);

    // Try to allocate 10 units (should fail - not enough contiguous space)
    int result5 = allocator.allocate(10, 4);
    std::cout << "Allocate(10, 4): " << result5 << std::endl;  // Expected: -1
    assert(result5 == -1);

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
#if  COND//similar approach with confitional flag
#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<assert.h>

//memory pool wioth buffer
#define MEMBLOCK 1500
#define MEM_POOL 1024

typedef struct {
    uint8_t Data[MEMBLOCK];
    _Bool isused;
    uint16_t length;
}Pool;
Pool MemBufferPool[MEM_POOL] = { 0 };
static uint16_t Memoffset = 0;

Pool* AllocFromPool() {
    for (int i = 0;i < MEM_POOL;i++) {
        uint16_t index = ((Memoffset + i) % MEM_POOL);
        if (!MemBufferPool[index].isused) {
            MemBufferPool[index].isused = 1;
            Memoffset = ((index + 1) % MEM_POOL);
            return (&MemBufferPool[index]);
        }

    }
    return NULL;
}

void freepool(Pool* buf) {
    buf->isused = 0;
}

int main()
{

    printf("Test 1: Basic allocation and release\n");

    Pool* buf = AllocFromPool();
    assert(buf != NULL);
    assert(buf->isused == 1);

    // Test writing to buffer
    strcpy(buf->Data, "Test packet data");
    buf->length = (int)strlen(buf->Data);

    printf("  + Buffer allocated successfully\n");
    printf("  + Data written: %s (length: %d)\n", buf->Data, buf->length);

    freepool(buf);
    assert(buf->isused == 0);
    printf("  + Buffer released successfully\n\n");
}

#endif