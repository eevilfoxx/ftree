#include "simulation/StatisticsSystem.hpp"
#include "domain/World.hpp"
#include <iostream>
#include <algorithm>

namespace ftree::simulation {

void StatisticsSystem::collectStatistics(const domain::World& world, int /*year*/, int& population,
                                         int& families, int& children, int& adults, int& elders) {
    auto living = world.getLivingPeople();
    auto families_ = world.getAllFamilies();

    population = living.size();
    families = families_.size();

    children = 0;
    adults = 0;
    elders = 0;
    for (const auto& personId : living) {
        auto personOpt = world.getPerson(personId);
        if (personOpt) {
            const auto& person = personOpt.value();
            if (person.age < 18) children++;
            else if (person.age < 60) adults++;
            else elders++;
        }
    }
}

void StatisticsSystem::printStatistics(int year, int population, int families,
                                       int children, int adults, int elders) {
    std::cout << "Year " << year << ":\n";
    std::cout << "  Population: " << population << "\n";
    std::cout << "  Families: " << families << "\n";
    std::cout << "  Children: " << children << ", Adults: " << adults << ", Elders: " << elders << "\n";
}

} // namespace ftree::simulation
