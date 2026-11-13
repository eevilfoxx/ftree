// main.cpp
#include <iostream>
#include <memory>
#include "core/Simulation.hpp"
#include "entities/repos/MemoryPersonRepository.hpp"
#include "entities/repos/MemoryFamilyRepository.hpp"
#include "core/EventDispatcher.hpp"
#include "core/Config.hpp"
#include "managers/NameManager.hpp"

int main() {
    Config::getInstance().loadFromFile("../config/simulation.cfg");

    auto personRepo = std::make_shared<MemoryPersonRepository>();
    auto familyRepo = std::make_shared<MemoryFamilyRepository>();
    auto eventDispatcher = std::make_shared<EventDispatcher>();
    
    Simulation simulation(personRepo, familyRepo, eventDispatcher);
    simulation.initialize(10);
    simulation.enableVisualization(true);
    simulation.simulateYears(100);

    return 0;
}