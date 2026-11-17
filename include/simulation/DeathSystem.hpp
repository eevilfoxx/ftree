#pragma once

#include "domain/Person.hpp"
#include "domain/World.hpp"

namespace ftree::simulation {

// DeathSystem processes deaths using the formula:
// P = mortality_a * exp(mortality_b * age)
class DeathSystem {
public:
    // Process deaths for all living people in the world
    // Returns number of deaths processed
    static int processDeaths(World& world, double mortality_a, double mortality_b);

    // Kill a specific person (marks dead, preserves genealogy)
    static void killPerson(World& world, PersonId id);
};

}