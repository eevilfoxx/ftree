#ifndef RANDOM_ENGINE_HPP
#define RANDOM_ENGINE_HPP

#include <random>

class RandomEngine {
public:
    RandomEngine(const RandomEngine&) = delete;
    RandomEngine& operator=(const RandomEngine&) = delete;
    
    static RandomEngine& getInstance();

    bool randomBool();
    
    int randomInt(int min, int max);
    
    double randomDouble(double min, double max);
    
    std::mt19937& getEngine() { return engine; }
    
private:
    RandomEngine();
    
    std::random_device randomDevice;
    std::mt19937 engine;
};

#endif