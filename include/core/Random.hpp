#pragma once

#include <random>
#include <chrono>
#include <stdexcept>

namespace ftree::core {

class Random {
public:
    // Constructor with explicit seed
    explicit Random(unsigned seed);
    
    // Default constructor with time-based seed
    Random();
    
    // Generate random integer in [min, max]
    int randomInt(int min, int max);
    
    // Generate random double in [min, max]
    double randomDouble(double min, double max);
    
    // Generate random bool (50/50)
    bool randomBool();
    
    // Get the underlying engine for advanced use
    std::mt19937& engine() { return engine_; }
    
    // Get current seed
    unsigned seed() const { return engine_.seed(); }
    
private:
    std::mt19937 engine_;
};

} // namespace ftree::core
