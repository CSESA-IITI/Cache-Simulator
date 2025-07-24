#include <bits/stdc++.h>

//----------------------------------------------------------------------------
// Constants and Configuration
//----------------------------------------------------------------------------
constexpr unsigned int DRAM_SIZE = (64 * 1024 * 1024);
constexpr int SIMULATION_ITERATIONS = 1'000'000;

// Enums for better type safety and readability
enum class CacheType { DIRECT_MAPPED, SET_ASSOCIATIVE, FULLY_ASSOCIATIVE };
enum class ReplacementPolicy { LRU, LFU, FIFO, RANDOM };

//----------------------------------------------------------------------------
// Memory Address Generators (Original Logic Preserved)
//----------------------------------------------------------------------------
namespace MemoryGenerator {
    unsigned int m_w = 0xABABAB55;
    unsigned int m_z = 0x05080902;

    unsigned int rand_() {
        m_z = 36969 * (m_z & 65535) + (m_z >> 16);
        m_w = 18000 * (m_w & 65535) + (m_w >> 16);
        return (m_z << 16) + m_w;
    }

    unsigned int memGen1() { static unsigned int addr = 0; return (addr++) % (DRAM_SIZE); }
    unsigned int memGen2() { return rand_() % (128 * 1024); }
    unsigned int memGen3() { return rand_() % (DRAM_SIZE); }
    unsigned int memGen4() { static unsigned int addr = 0; return (addr++) % (1024); }
    unsigned int memGen5() { static unsigned int addr = 0; return (addr++) % (1024 * 64); }
    unsigned int memGen6() { static unsigned int addr = 0; return (addr += 256) % (DRAM_SIZE); }
}

//----------------------------------------------------------------------------
// Cache Simulator Base Class
//----------------------------------------------------------------------------
class CacheSimulator {
protected:
    // Statistics
    long long hits = 0;
    long long compulsory_misses = 0;
    long long capacity_misses = 0;
    long long conflict_misses = 0;

    // Configuration
    int cache_size_kb;
    int block_size;

    // Calculated values
    int offset_bits;
    unsigned int num_total_blocks;

public:
    CacheSimulator(int size_kb, int blk_size)
        : cache_size_kb(size_kb), block_size(blk_size) {
        offset_bits = static_cast<int>(log2(block_size));
        num_total_blocks = (cache_size_kb * 1024) / block_size;
    }

    virtual ~CacheSimulator() = default;

    // Public interface to access the cache
    bool access(unsigned int address) {
        bool hit = perform_access(address);
        if (hit) {
            hits++;
        }
        return hit;
    }
    
    // Pure virtual function to be implemented by derived classes
    virtual bool perform_access(unsigned int address) = 0;

    void print_stats() const {
        long long total_accesses = hits + compulsory_misses + capacity_misses + conflict_misses;
        double hit_rate = (total_accesses > 0) ? (100.0 * hits / total_accesses) : 0.0;
        
        std::cout << "\n--- Cache Simulation Results ---\n";
        std::cout << "Total Accesses: " << total_accesses << "\n";
        std::cout << "Hits:           " << hits << " (" << std::fixed << std::setprecision(2) << hit_rate << "%)\n";
        std::cout << "Misses:         " << (total_accesses - hits) << "\n";
        std::cout << "  - Compulsory: " << compulsory_misses << "\n";
        std::cout << "  - Capacity:   " << capacity_misses << "\n";
        std::cout << "  - Conflict:   " << conflict_misses << "\n";
        std::cout << "--------------------------------\n";
    }
};

//----------------------------------------------------------------------------
// Direct Mapped Cache Implementation
//----------------------------------------------------------------------------
class DirectMappedCache : public CacheSimulator {
private:
    struct CacheLine {
        unsigned int tag = 0;
        bool valid = false;
    };
    std::vector<CacheLine> cache;
    int index_bits;

public:
    DirectMappedCache(int size_kb, int blk_size) : CacheSimulator(size_kb, blk_size) {
        index_bits = static_cast<int>(log2(num_total_blocks));
        cache.resize(num_total_blocks);
    }

    bool perform_access(unsigned int address) override {
        unsigned int index = (address >> offset_bits) & (num_total_blocks - 1);
        unsigned int tag = address >> (offset_bits + index_bits);

        if (cache[index].valid && cache[index].tag == tag) {
            return true; // Hit
        } else {
            // Miss
            if (!cache[index].valid) {
                compulsory_misses++;
            } else {
                conflict_misses++; // A valid block is being replaced
            }
            cache[index].tag = tag;
            cache[index].valid = true;
            return false;
        }
    }
};

//----------------------------------------------------------------------------
// Set Associative Cache Implementation
//----------------------------------------------------------------------------
class SetAssociativeCache : public CacheSimulator {
private:
    struct CacheLine {
        unsigned int tag = 0;
        bool valid = false;
    };
    
    int ways;
    int num_sets;
    int index_bits;
    std::vector<std::vector<CacheLine>> cache;

public:
    SetAssociativeCache(int size_kb, int blk_size, int num_ways)
        : CacheSimulator(size_kb, blk_size), ways(num_ways) {
        num_sets = num_total_blocks / ways;
        index_bits = static_cast<int>(log2(num_sets));
        cache.resize(num_sets, std::vector<CacheLine>(ways));
    }

    bool perform_access(unsigned int address) override {
        unsigned int index = (address >> offset_bits) & (num_sets - 1);
        unsigned int tag = address >> (offset_bits + index_bits);

        // Search for a hit
        for (int i = 0; i < ways; ++i) {
            if (cache[index][i].valid && cache[index][i].tag == tag) {
                return true; // Hit
            }
        }

        // Miss: Find a way to place the new block
        // 1. Look for an invalid (empty) way
        for (int i = 0; i < ways; ++i) {
            if (!cache[index][i].valid) {
                cache[index][i].valid = true;
                cache[index][i].tag = tag;
                compulsory_misses++;
                return false;
            }
        }

        // 2. No invalid ways found, must replace (Random Replacement)
        int victim_way = rand() % ways;
        cache[index][victim_way].tag = tag;
        // The miss is either conflict or capacity. We'll group them here.
        conflict_misses++; 
        return false;
    }
};

//----------------------------------------------------------------------------
// Fully Associative Cache Implementation
//----------------------------------------------------------------------------
class FullyAssociativeCache : public CacheSimulator {
private:
    struct CacheBlock {
        unsigned int tag = 0;
        bool valid = false;
        int lru_counter = 0; // For LRU and FIFO
        int lfu_counter = 0; // For LFU
    };

    std::vector<CacheBlock> cache;
    ReplacementPolicy policy;
    int current_time = 0; // For LRU/FIFO
    unsigned int filled_blocks = 0;

public:
    FullyAssociativeCache(int size_kb, int blk_size, ReplacementPolicy p)
        : CacheSimulator(size_kb, blk_size), policy(p) {
        cache.resize(num_total_blocks);
    }

    bool perform_access(unsigned int address) override {
        current_time++;
        unsigned int tag = address >> offset_bits;

        // Search for hit
        for (unsigned int i = 0; i < filled_blocks; ++i) {
            if (cache[i].tag == tag) { // Hit
                if (policy == ReplacementPolicy::LRU) {
                    cache[i].lru_counter = current_time;
                } else if (policy == ReplacementPolicy::LFU) {
                    cache[i].lfu_counter++;
                }
                return true;
            }
        }

        // Miss
        unsigned int victim_index = 0;
        if (filled_blocks < num_total_blocks) {
            // Still space in cache -> Compulsory Miss
            victim_index = filled_blocks;
            filled_blocks++;
            compulsory_misses++;
        } else {
            // Cache is full -> Capacity Miss, find victim
            capacity_misses++;
            victim_index = find_victim();
        }

        // Place new block
        cache[victim_index].tag = tag;
        cache[victim_index].valid = true;
        cache[victim_index].lru_counter = current_time; // For LRU/FIFO
        cache[victim_index].lfu_counter = 1;         // For LFU
        return false;
    }
    
private:
    unsigned int find_victim() {
        unsigned int victim = 0;
        switch (policy) {
            case ReplacementPolicy::RANDOM:
                victim = rand() % num_total_blocks;
                break;
            case ReplacementPolicy::FIFO: // Falls through to LRU
            case ReplacementPolicy::LRU: {
                int min_val = cache[0].lru_counter;
                for (unsigned int i = 1; i < num_total_blocks; ++i) {
                    if (cache[i].lru_counter < min_val) {
                        min_val = cache[i].lru_counter;
                        victim = i;
                    }
                }
                break;
            }
            case ReplacementPolicy::LFU: {
                int min_freq = cache[0].lfu_counter;
                for (unsigned int i = 1; i < num_total_blocks; ++i) {
                    if (cache[i].lfu_counter < min_freq) {
                        min_freq = cache[i].lfu_counter;
                        victim = i;
                    }
                }
                break;
            }
        }
        return victim;
    }
};

//----------------------------------------------------------------------------
// Main Function
//----------------------------------------------------------------------------
void run_simulation(CacheSimulator& simulator) {
    // Select a memory generator for the simulation
    // Using memGen5 as an example, change as needed.
    auto mem_gen_func = MemoryGenerator::memGen5;
    
    std::cout << "Running simulation with " << SIMULATION_ITERATIONS << " memory accesses...\n";
    for (int i = 0; i < SIMULATION_ITERATIONS; ++i) {
        unsigned int addr = mem_gen_func();
        simulator.access(addr);
    }
    simulator.print_stats();
}

int main() {
    // Seed the random number generator ONCE
    srand(static_cast<unsigned int>(time(nullptr)));

    int cache_type_choice, cache_size, block_size;

    std::cout << "--- Cache Simulator Setup ---\n";
    std::cout << "Enter cache type (0: Direct Mapped, 1: Set Associative, 2: Fully Associative): ";
    std::cin >> cache_type_choice;
    
    std::cout << "Enter block size (bytes, power of 2, e.g., 32): ";
    std::cin >> block_size;

    std::cout << "Enter cache size (KB, power of 2, e.g., 16): ";
    std::cin >> cache_size;

    std::unique_ptr<CacheSimulator> simulator;
    CacheType type = static_cast<CacheType>(cache_type_choice);

    switch (type) {
        case CacheType::DIRECT_MAPPED:
            simulator = std::make_unique<DirectMappedCache>(cache_size, block_size);
            break;
        case CacheType::SET_ASSOCIATIVE: {
            int ways;
            std::cout << "Enter number of ways (power of 2, e.g., 4): ";
            std::cin >> ways;
            simulator = std::make_unique<SetAssociativeCache>(cache_size, block_size, ways);
            break;
        }
        case CacheType::FULLY_ASSOCIATIVE: {
            int policy_choice;
            std::cout << "Enter replacement policy (0: LRU, 1: LFU, 2: FIFO, 3: RANDOM): ";
            std::cin >> policy_choice;
            ReplacementPolicy policy = static_cast<ReplacementPolicy>(policy_choice);
            simulator = std::make_unique<FullyAssociativeCache>(cache_size, block_size, policy);
            break;
        }
        default:
            std::cerr << "Invalid cache type selected. Exiting.\n";
            return 1;
    }

    if (simulator) {
        run_simulation(*simulator);
    }

    return 0;
}