#include "core/RandomEngine.hpp"

#include <chrono>

RandomEngine& RandomEngine::getInstance() {
    static RandomEngine instance;
    return instance;
}

RandomEngine::RandomEngine() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    engine.seed(seed);
}

bool RandomEngine::randomBool() {
    std::uniform_int_distribution<int> distribution(0, 1);
    return distribution(engine) == 1;
}

int RandomEngine::randomInt(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(engine);
}

double RandomEngine::randomDouble(double min, double max) {
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(engine);
}