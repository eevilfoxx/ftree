#pragma once

#include "domain/Person.hpp"
#include "domain/World.hpp"

namespace ftree::simulation {

// BirthSystem handles childbirth logic
class BirthSystem {
public:
    // Process births for all pregnant women
    // Returns number of births processed
    static int processBirths(World& world);

    // Give birth to a child for a pregnant person
    // Returns the child's ID, or 0 if no birth
    static PersonId giveBirth(World& world, PersonId motherId, const std::string& surname);

    // Reset pregnancy state after birth
    static void resetPregnancy(World& world, PersonId motherId);
};

}
} // namespace ftree::simulation