#pragma once

#include "domain/Person.hpp"
#include "domain/World.hpp"

namespace ftree::simulation {

// MarriageSystem handles marriage logic
class MarriageSystem {
public:
    // Try to marry two eligible persons
    // Returns true if marriage was created
    static bool tryMarry(World& world, PersonId maleId, PersonId femaleId);

    // Get minimum marriage age
    static int getMin MarriageAge();

    // Get maximum person age
    static int getMaxPersonAge();
};

}