#include "core/SimulationClock.hpp"
#include <stdexcept>

namespace ftree::core {

void SimulationClock::nextYear() {
    ++currentYear_;
}

void SimulationClock::reset() {
    currentYear_ = 0;
}

void SimulationClock::restoreYear(int year) {
    if (year < 0) throw std::invalid_argument("Simulation year cannot be negative");
    currentYear_ = year;
}

} // namespace ftree::core
