#include "core/SimulationClock.hpp"

namespace ftree::core {

void SimulationClock::nextYear() {
    currentYear_++;
}

int SimulationClock::currentYear() const {
    return currentYear_;
}

void SimulationClock::reset() {
    currentYear_ = 0;
}

} // namespace ftree::core
