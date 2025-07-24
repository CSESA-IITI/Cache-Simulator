# Cache Simulator in C++

## Overview

This project is a C++ implementation of a CPU cache simulator. It supports multiple cache organizations and replacement policies, and provides detailed statistics to evaluate the cache performance.

### Supported Cache Types

- Direct Mapped Cache
- Set Associative Cache
- Fully Associative Cache

### Replacement Policies

- Least Recently Used (LRU)
- Least Frequently Used (LFU)
- First-In First-Out (FIFO)
- Random Replacement

### Metrics Tracked

- Total Accesses
- Cache Hits
- Cache Misses:
  - Compulsory Misses
  - Conflict Misses
  - Capacity Misses
- Hit Rate

---

## Features

- Modular, object-oriented C++ design
- Support for custom cache and block sizes
- Multiple memory access patterns (6 built-in generators)
- Detailed classification of misses
- Clean and extensible codebase

---

## Technologies Used

- C++17
- STL Containers (`vector`, `unordered_map`)
- Bitwise operations for tag/index extraction
- Randomized access patterns

---

## Getting Started

### Prerequisites

You need a C++ compiler that supports C++17 or above.

For example:
- g++ 7 or later
- clang++ 7 or later

### Build Instructions

1. Clone the repository:

   ```bash
   git clone https://github.com/your-username/cache-simulator-cpp.git
   cd cache-simulator-cpp

   
2. Compile the simulator:
   ``bash
   g++ -std=c++17 -O2 -o main main.cpp
   ``

3. Run the simulation:
   ``bash
   ./main
   ``

## 📥 Input Options

When the simulator starts, you will be prompted to configure the cache setup. For example:

--- Cache Simulator Setup ---
Enter cache type (0: Direct Mapped, 1: Set Associative, 2: Fully Associative):
Enter block size (bytes, power of 2, e.g., 32):
Enter cache size (KB, power of 2, e.g., 16):

Depending on the selected cache type, you may be prompted for additional input:

###  Set Associative Cache

Enter number of ways (power of 2, e.g., 4):

### Fully Associative Cache

Enter replacement policy (0: LRU, 1: LFU, 2: FIFO, 3: RANDOM):

## Memory Access Patterns

The simulator includes 6 predefined memory access generators located under the `MemoryGenerator` namespace. These functions simulate different types of memory access behaviors, helping test cache performance under various scenarios.

| Function Name         | Description                                          |
|-----------------------|------------------------------------------------------|
| `memGen1()`           | Sequential access: starts from 0 and increments by 1 |
| `memGen2()`           | Random access within a 128 KB range                  |
| `memGen3()`           | Random access across the full DRAM size              |
| `memGen4()`           | Repeating access pattern within 1 KB                 |
| `memGen5()`           | Repeating access pattern within 64 KB                |
| `memGen6()`           | Strided access: increments by 256 bytes each time    |

### Changing the Memory Generator

To switch the memory generator used in the simulation, modify the line inside the `run_simulation()` function in `main.cpp`:

```cpp
auto mem_gen_func = MemoryGenerator::memGen5; // Default

### Sample Output

After running the simulator with your configuration, the output will display cache statistics like this:
--- Cache Simulation Results ---
Total Accesses: 1000000
Hits: 834215 (83.42%)
Misses: 165785
Compulsory: 1024
Capacity: 50231
Conflict: 114530


You can use this data to analyze how well different cache organizations and replacement strategies perform under specific access patterns.

### Project Structure
cache-simulator-cpp/
├── main.cpp # Main source file containing all cache classes and logic
├── README.md # Project documentation (this file)

## Author

This project was developed by Aayush Yadav as a simulation tool to better understand CPU caching strategies and their trade-offs.

If you'd like to connect or discuss improvements, feel free to reach out!

GitHub: [github.com/drstrox](https://github.com/drstrox)




