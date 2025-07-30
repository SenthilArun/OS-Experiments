## Performance Characteristics

- **Memory Allocators**: O(1) allocation with configurable alignment, efficient pool-based management
- **Ring Buffer**: O(1) enqueue/dequeue operations with lock-free design
- **Event System**: O(n) event dispatch with priority ordering, O(1) registration/removal
- **Producer-Consumer**: Bounded waiting times with proper backpressure handling
- **Thread Synchronization**: Minimal lock contention with condition variable optimization
- **Timer System**: O(log n) event insertion/removal using balanced data structures
- **Tree Operations**: O(h) traversal and search where h is tree height

### Event-Driven Architecture
- **DynamicCallbacks.c**: Production-grade callback management system
  - Thread-safe callback registration and execution with comprehensive mutex handling
  - Priority-based callback ordering for real-time system requirements
  - Comprehensive error handling, diagnostics, and performance monitoring
  - Statistics tracking with execution time analysis and error counting
  - Cross-platform compatibility (Windows/Linux) with proper resource cleanup

- **DynamicEventHandler.c**: Lightweight event system for embedded applications
  - Dynamic listener registration/removal with ID-based management
  - Multiple handlers per event type with efficient linked list storage
  - Memory-efficient implementation suitable for resource-constrained environments
  - Event triggering with data payload support for flexible communication# OS-Experiments

A comprehensive collection of operating systems concepts and implementations, demonstrating fundamental OS mechanisms, data structures, and system programming techniques.

## Overview

This repository showcases practical implementations of core operating system concepts including memory management, synchronization primitives, inter-process communication, and system-level programming. Each module is designed to demonstrate real-world OS principles with production-quality code.

## Project Structure

```
OS-Experiments/
├── Src/
│   ├── Align-Padding.c          # Memory alignment and padding demonstration
│   ├── CircularBuffer.cpp       # Ring buffer implementation for IPC
│   ├── DynamicCallbacks.c       # Dynamic callback registration system
│   ├── DynamicEventHandler.c    # Event-driven programming framework
│   ├── List.c                   # Linked list operations and memory management
│   ├── ProducerConsumer.cpp     # Producer-Consumer synchronization
│   ├── Sem-ProducerConsumer.cpp # Semaphore-based synchronization
│   ├── StateMachine.cpp         # Finite state machine implementation
│   ├── imagebin.cpp            # Binary data handling and signal processing
│   ├── bufferthread.cpp        # Windows multi-threading with bouncing characters
│   ├── LeetCodeMemPool.cpp     # Custom memory pool allocator implementations
│   ├── Synchronization.cpp     # Template-based thread-safe queue system
│   ├── thread.cpp              # Windows threading fundamentals
│   ├── ThreadPriorities.cpp    # Priority-based task scheduling system
│   ├── TimerDynamicArray.cpp   # Hardware timer event management system
│   └── treechallenge.cpp       # Advanced tree data structures and algorithms
└── README.md
```

## Core Implementations

### Memory Management & Allocation
- **Align-Padding.c**: Custom aligned memory allocator with SIMD optimization support
  - Implements `malloc_aligned()` and `free_aligned()` functions
  - Supports 16-byte and 32-byte alignment for vectorized operations
  - Comprehensive memory layout visualization and debugging tools
  - Platform-specific optimizations for Windows and Unix systems

- **LeetCodeMemPool.cpp**: Advanced memory pool allocator implementations
  - ID-based memory allocation with sequential buffer management
  - Fixed-size block allocation with reuse optimization
  - Memory fragmentation prevention and efficient deallocation
  - Pool-based memory management for embedded and real-time systems

### Threading & Synchronization
- **ProducerConsumer.cpp**: Thread-safe bounded queue using condition variables
  - Blocking operations with proper synchronization
  - Template-based implementation for type safety
  - Demonstrates mutex and condition variable usage

- **Synchronization.cpp**: Professional-grade template-based synchronization system
  - Generic thread-safe queue implementation with comprehensive documentation
  - Condition variable coordination for multiple producers/consumers
  - Exception safety and proper resource management
  - MIT-licensed production-ready code

- **Sem-ProducerConsumer.cpp**: POSIX semaphore-based producer-consumer
  - Binary and counting semaphore implementations
  - Buffer overflow/underflow protection
  - Classic OS synchronization problem solution

- **thread.cpp**: Windows threading fundamentals using Win32 API
  - CreateThread() and thread management demonstrations
  - Thread synchronization with WaitForSingleObject()
  - Proper thread handle cleanup and resource management

- **ThreadPriorities.cpp**: Priority-based task scheduling system
  - Multi-producer, multi-consumer with priority queues
  - Priority-ordered task insertion and execution
  - Demonstrates advanced thread coordination patterns

- **bufferthread.cpp**: Interactive multi-threading demonstration
  - Windows console-based bouncing character animation
  - Real-time thread creation and termination
  - Mutex-based screen buffer synchronization
  - Interactive keyboard-driven thread management

### Hardware Abstraction & Timing
- **TimerDynamicArray.cpp**: Hardware timer event management system
  - Callback-based timer event registration and execution
  - Hardware register simulation for embedded systems
  - Dynamic timer addition/removal with handle-based management
  - Interrupt-driven programming patterns for real-time systems

### Inter-Process Communication
- **CircularBuffer.cpp**: Lock-free ring buffer for high-performance IPC
  - Thread-safe read/write operations with overflow/underflow detection
  - Efficient memory usage with wrap-around logic
  - Suitable for UART, network buffers, and data streaming

### Data Structures & Algorithms
- **List.c**: Fundamental linked list operations and memory management
  - Dynamic node creation, insertion, and deletion
  - Memory leak prevention with proper cleanup functions
  - Comprehensive edge case handling (empty lists, single nodes)
  - Foundation for more complex data structure implementations

- **treechallenge.cpp**: Advanced tree data structures and algorithms
  - Multi-way tree implementation with configurable child nodes
  - Template-based generic tree traversal algorithms
  - Tree path tracking and node relationship analysis
  - Complex tree construction from specification arrays

### System Programming
- **StateMachine.cpp**: Finite state machine for embedded systems
  - State transition management with event-driven architecture
  - LED control simulation (embedded systems pattern)
  - Clean separation of state logic and action handlers

- **imagebin.cpp**: Signal handling and binary data processing
  - POSIX signal handling (simulated ISR) with proper cleanup
  - Binary file I/O operations for graphics/model data
  - Interrupt-driven programming concepts
  - Vertex and index buffer management for 3D applications

## Technical Features

### Memory Management
- Custom memory allocators with alignment guarantees
- Memory layout visualization and debugging
- SIMD-optimized data structures
- Memory leak prevention and cleanup

### Concurrency Control
- Multiple synchronization primitives (mutexes, semaphores, condition variables)
- Deadlock-free designs
- Thread-safe data structures
- Performance-optimized critical sections

### System Integration
- Cross-platform compatibility (Windows/Linux)
- POSIX compliance where applicable
- Error handling and recovery mechanisms
- Comprehensive logging and diagnostics

## Key OS Concepts Demonstrated

| Concept | Implementation | File |
|---------|---------------|------|
| **Memory Alignment** | Custom aligned malloc/free with SIMD support | `Align-Padding.c` |
| **Memory Pool Management** | ID-based allocation with fragmentation prevention | `LeetCodeMemPool.cpp` |
| **Producer-Consumer** | Condition variables & semaphores | `ProducerConsumer.cpp`, `Sem-ProducerConsumer.cpp` |
| **Thread Synchronization** | Template-based thread-safe containers | `Synchronization.cpp` |
| **Multi-Threading** | Windows API threading with resource management | `thread.cpp`, `bufferthread.cpp` |
| **Priority Scheduling** | Task prioritization with multi-producer/consumer | `ThreadPriorities.cpp` |
| **Hardware Timers** | Event-driven timer management with interrupts | `TimerDynamicArray.cpp` |
| **Ring Buffers** | Lock-free circular queue for IPC | `CircularBuffer.cpp` |
| **Event Systems** | Dynamic callback registration with priorities | `DynamicCallbacks.c`, `DynamicEventHandler.c` |
| **State Machines** | FSM with event handling for embedded systems | `StateMachine.cpp` |
| **Signal Handling** | Interrupt simulation with proper cleanup | `imagebin.cpp` |
| **Data Structures** | Linked lists and advanced tree algorithms | `List.c`, `treechallenge.cpp` |

## Testing & Validation

Each implementation includes comprehensive test cases covering:
- Normal operation scenarios
- Edge cases and error conditions
- Memory leak detection
- Performance benchmarking
- Thread safety validation

## Real-World Applications

These implementations are suitable for:

### Embedded & Real-Time Systems
- **Memory Pool Allocators**: Fixed-allocation patterns for deterministic timing
- **State Machines**: Device control logic and protocol handling
- **Ring Buffers**: UART communication and sensor data buffering
- **Timer Systems**: Interrupt-driven event scheduling and periodic tasks

### High-Performance Computing
- **SIMD-Aligned Memory**: Vectorized operations and parallel processing
- **Lock-Free Data Structures**: Low-latency inter-thread communication
- **Priority-Based Scheduling**: Real-time task management and deadline handling

### System Software & Infrastructure
- **Callback Systems**: Plugin architectures and event-driven frameworks
- **Thread Synchronization**: Database connection pools and request handling
- **Signal Handling**: System service management and graceful shutdowns

### Network & Communication Systems
- **Producer-Consumer Patterns**: Packet processing pipelines and message queues
- **Circular Buffers**: Network interface buffering and stream processing
- **Event-Driven Architecture**: Asynchronous I/O and reactive systems

### Game Development & Graphics
- **Memory Management**: Object pools for game entities and rendering resources
- **State Machines**: AI behavior trees and game state management
- **Binary Data Processing**: Asset loading and mesh optimization

## Learning Outcomes

This project demonstrates advanced proficiency in:

### Core Systems Programming
- **Low-level memory management**: Custom allocators, alignment, and pool-based allocation
- **Concurrent programming**: Multi-threading, synchronization primitives, and lock-free algorithms
- **Hardware abstraction**: Timer management, interrupt handling, and signal processing
- **Platform-specific development**: Windows API, POSIX compliance, and cross-platform design

### Advanced OS Concepts
- **Inter-process communication**: Shared memory, message passing, and synchronization
- **Real-time systems**: Deterministic timing, priority scheduling, and deadline management
- **Event-driven architecture**: Callback systems, observer patterns, and reactive programming
- **Resource management**: Memory pools, handle-based systems, and automatic cleanup

### Software Engineering Excellence
- **Performance optimization**: Cache-friendly algorithms, lock-free programming, and SIMD utilization
- **Scalability design**: Template-based generics, configurable parameters, and modular architecture
- **Error handling**: Comprehensive validation, graceful degradation, and diagnostic capabilities
- **Code quality**: Extensive documentation, test coverage, and production-ready patterns

## Code Quality Features

- **Memory Safety**: Proper allocation/deallocation patterns
- **Thread Safety**: Careful synchronization design
- **Error Handling**: Comprehensive error checking and recovery
- **Documentation**: Detailed inline comments and algorithmic explanations
- **Portability**: Cross-platform compatibility
- **Performance**: Optimized algorithms and data structures

## Future Enhancements

- [ ] **Lock-Free Data Structures**: Atomic operations-based queues and stacks for ultra-low latency
- [ ] **NUMA-Aware Memory Management**: Topology-conscious allocation for multi-socket systems  
- [ ] **Advanced Scheduling Algorithms**: CFS, EDF, and real-time scheduling simulations
- [ ] **Virtual Memory Management**: Page tables, TLB simulation, and memory mapping demonstrations
- [ ] **File System Implementation**: Inode structures, block allocation, and journaling concepts
- [ ] **Network Stack Components**: Protocol implementation and packet processing pipelines
- [ ] **Distributed Systems Patterns**: Consensus algorithms, leader election, and distributed locks
- [ ] **Performance Profiling Tools**: Built-in benchmarking and performance analysis capabilities

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Please feel free to submit pull requests for:
- Additional OS concept implementations
- Performance optimizations
- Cross-platform compatibility improvements
- Documentation enhancements

---

**Author**: SenthilArun  
**Focus**: Operating Systems, System Programming, Embedded Software  
**Expertise**: Memory Management, Concurrency, Real-Time Systems
