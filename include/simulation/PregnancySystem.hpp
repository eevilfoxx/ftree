#pragma once

#include "domain/Person.hpp"
#include "domain/World.hpp"

namespace ftree::simulation {

// PregnancySystem handles conception and pregnancy logic
class PregnancySystem {
public:
    // Try to conceive a child for the given family
    // Returns true if conception succeeded
    static bool tryConceive(domain::World& world, domain::PersonId motherId, domain::PersonId fatherId);

    // Check if a person can attempt conception
    static bool canConceive(domain::PersonData& person);
};

}