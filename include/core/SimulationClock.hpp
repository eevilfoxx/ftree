#pragma once

namespace ftree::core {

class SimulationClock {
public:
    SimulationClock() : currentYear_(0) {}
    
    // Advance to next year
    void nextYear();
    
    // Get current year
    int currentYear() const { return currentYear_; }
    
    // Reset to year 0
    void reset();
    
private:
    int currentYear_;
};

} // namespace ftree::core
