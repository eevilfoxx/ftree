#pragma once

#include <random>
#include <string>

namespace ftree::core {

class Random {
public:
    explicit Random(unsigned seed);
    Random();

    int randomInt(int min, int max);
    double randomDouble(double min, double max);
    bool randomBool();

    std::mt19937& engine() noexcept { return engine_; }
    unsigned seed() const noexcept { return seed_; }
    std::string serializeState() const;
    void restoreState(const std::string& state);

private:
    std::mt19937 engine_;
    unsigned seed_;
};

} // namespace ftree::core
