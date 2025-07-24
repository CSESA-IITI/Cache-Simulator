# 🔄 Cache Simulator in C++

## 📌 Overview

This project simulates different types of CPU caches using C++:
- **Direct Mapped Cache**
- **Set Associative Cache**
- **Fully Associative Cache**

It supports **multiple replacement policies** and memory access patterns to evaluate cache performance metrics like:
- Hits
- Misses (Compulsory, Conflict, Capacity)
- Hit Rate

---

## 🧠 Features

✅ Object-Oriented Design  
✅ Support for multiple cache organizations  
✅ Replacement policies: `LRU`, `LFU`, `FIFO`, `RANDOM`  
✅ 6 Built-in memory access generators  
✅ Detailed simulation statistics  
✅ Custom cache and block size inputs  

---

## 🧰 Technologies Used

- **C++17**  
- **STL containers**: `vector`, `unordered_map`, etc.  
- **Bitwise operations** for address decoding  
- **OOP principles** for extensibility

---

## 🚀 Getting Started

### 🔧 Prerequisites

Ensure you have a C++ compiler installed:
- g++ 7 or later recommended

### 📦 Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/cache-simulator-cpp.git
   cd cache-simulator-cpp
   ``
   
2. Compile the simulator:
   ``bash
   g++ -std=c++17 -O2 -o main main.cpp
   ``

3. Run the simulation:
   ``bash
   ./main
   ``

## 📥 Input Options

During runtime, the simulator prompts the user for setup. Here’s a breakdown of the interactive inputs:

--- Cache Simulator Setup ---
Enter cache type (0: Direct Mapped, 1: Set Associative, 2: Fully Associative):
Enter block size (bytes, power of 2, e.g., 32):
Enter cache size (KB, power of 2, e.g., 16):

Depending on your cache type selection:

### ➕ Set Associative Cache
You’ll be additionally prompted:

Enter number of ways (power of 2, e.g., 4):

### 🧠 Fully Associative Cache
You’ll be additionally prompted:

Enter replacement policy (0: LRU, 1: LFU, 2: FIFO, 3: RANDOM):


You can edit the memory generator being used for simulation in the code directly by modifying this line in `run_simulation()`:

```cpp
auto mem_gen_func = MemoryGenerator::memGen5; // Change to memGen1 to memGen6

### Example

auto mem_gen_func = MemoryGenerator::memGen3;

