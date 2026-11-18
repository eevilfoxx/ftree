#pragma once

#include "domain/Person.hpp"
#include "domain/World.hpp"
#include "core/Random.hpp"
#include <core/NameDatabase.hpp>

#include <string>

namespace ftree::simulation {

// BirthSystem handles childbirth logic
class BirthSystem {
public:
    // Process births for all pregnant women
    // Returns number of births processed
    static int processBirths(domain::World& world,
                             double probTriple,
                             double probTwin,
                             core::NameDatabase& nameDb,
                             core::Random& random);

    // Give birth to a child for a pregnant person
    // Returns the child's ID, or 0 if no birth
    static domain::PersonId giveBirth(domain::World& world,
                                       domain::PersonId motherId,
                                       const std::string& surname,
                                       core::NameDatabase& nameDb,
                                       core::Random& random);

    // Reset pregnancy state after birth
    static void resetPregnancy(domain::World& world, domain::PersonId motherId);
};

} // namespace ftree::simulation