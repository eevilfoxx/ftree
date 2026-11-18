#include "core/Random.hpp"
#include <random>
#include <stdexcept>

namespace ftree::core {

Random::Random(unsigned seed) : engine_(seed), seed_(seed) {}

Random::Random()
    : Random(std::random_device{}()) {}

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
