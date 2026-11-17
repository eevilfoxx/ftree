#include "core/Random.hpp"
#include <random>
#include <chrono>

namespace ftree::core {

Random::Random(unsigned seed) : engine_(seed) {}

Random::Random() {
    auto now = std::chrono::high_resolution_clock::now();
    auto ns = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count());
    engine_(static_cast<unsigned int>(ns));
}

int Random::randomInt(int min, int max) {
    if (min > max) throw std::invalid_argument("randomInt: min > max");
    std::uniform_int_distribution<int> dist(min, max);
    return dist(engine_);
}

double Random::randomDouble(double min, double max) {
    if (min > max) throw std::invalid_argument("randomDouble: min > max");
    std::uniform_real_distribution<double> dist(min, max);
    return dist(engine_);
}

bool Random::randomBool() {
    return randomInt(0, 1) == 1;
}

} // namespace ftree::core
