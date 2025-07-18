#include <iostream>
#include <vector>

class CircularBuffer {
public:
    CircularBuffer(size_t size);

    bool write(uint8_t data); // Write a byte to the buffer
    bool read(uint8_t& data); // Read a byte from the buffer
    bool isEmpty() const; // Check if the buffer is empty
    bool isFull() const;  // Check if the buffer is full

private:
    std::vector<uint8_t> buffer_; // Storage for the buffer
    int head_;                 // Index for the next write operation (starts at -1)
    size_t tail_;                 // Index for the next read operation (starts at 0)
    bool is_full_;                // Indicates whether the buffer is full
    int cnt;
    int size;
};

CircularBuffer::CircularBuffer(size_t size) : buffer_(size), size(size),head_(0), tail_(0), cnt(0),is_full_(false) {}

bool CircularBuffer::write(uint8_t data) {
    
    buffer_[head_] = data;
    head_ = (head_ + 1) % buffer_.size(); // Increment head and wrap around if needed
         
        if (cnt < size)
        {
           
            cnt++;
        }
        else
        {
            tail_ = (tail_ + 1) % buffer_.size();
      
         
        }

 
    return true;
}

bool CircularBuffer::read(uint8_t& data) {
    if (isEmpty()) {
        return false;
    }

    data = buffer_[tail_];
    tail_ = (tail_ + 1) % buffer_.size(); // Increment tail and wrap around if needed

    cnt--;
   /*s_full_ = false;*/

    return true;
}

bool CircularBuffer::isEmpty() const {
    return(cnt == 0 );
     
}

bool CircularBuffer::isFull() const {
    return is_full_;
}
#if 0
int main() {
    CircularBuffer uartBuffer(5); // Create a circular buffer with size 128

    // Example: Simulate writing to the buffer
    for (int i = 0; i < 10; i++) {
        if (!uartBuffer.write(static_cast<uint8_t>(i))) {
            std::cout << "Buffer overflow at value: " << i << std::endl;
        }
    }

    // Example: Simulate reading from the buffer
    uint8_t data;
    while (uartBuffer.read(data)) {
        std::cout << "Read value: " << static_cast<int>(data) << std::endl;
    }

    if (uartBuffer.isEmpty()) {
        std::cout << "Buffer underflow" << std::endl;
    }

    return 0;
}
#endif