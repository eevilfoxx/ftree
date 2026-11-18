#include "core/Random.hpp"
#include <random>
#include <stdexcept>
#include <sstream>

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

std::string Random::serializeState() const {
    std::ostringstream output;
    output << engine_;
    if (!output) throw std::runtime_error("Failed to serialize random engine state");
    return output.str();
}

void Random::restoreState(const std::string& state) {
    std::istringstream input(state);
    std::mt19937 restored;
    input >> restored;
    if (!input) throw std::invalid_argument("Invalid serialized random engine state");
    input >> std::ws;
    if (!input.eof()) throw std::invalid_argument("Trailing data in random engine state");
    engine_ = restored;
}

bool Random::randomBool() {
    return randomInt(0, 1) == 1;
}

} // namespace ftree::core
