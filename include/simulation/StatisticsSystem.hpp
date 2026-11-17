#pragma once

#include "domain/World.hpp"

namespace ftree::simulation {

class StatisticsSystem {
public:
    static void collectStatistics(const domain::World& world, int year, int& population,
                                  int& families, int& children, int& adults, int& elders);
    static void printStatistics(int year, int population, int families,
                                int children, int adults, int elders);
};

}