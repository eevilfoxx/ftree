#pragma once

#include "domain/Person.hpp"
#include "domain/World.hpp"

namespace ftree::simulation {

// AgingSystem increases age of all living persons by 1 year
class AgingSystem {
public:
    // Apply aging to all living people in the world
    static void ageAllLiving(World& world);

    // Age a specific person (no-op if dead)
    static void agePerson(World& world, PersonId id);
};

} // namespace ftree::simulation
