#pragma once

#include "domain/Person.hpp"
#include "domain/World.hpp"
#include "core/Random.hpp"

namespace ftree::simulation {

// PregnancySystem handles conception and pregnancy logic
class PregnancySystem {
public:
    // Try to conceive a child for the given family
    // Returns true if conception succeeded
    static bool tryConceive(domain::World& world,
                            domain::PersonId motherId,
                            domain::PersonId fatherId,
                            double baseConceptionProbability,
                            core::Random& random);

    // Check if a person can attempt conception
    static bool canConceive(const domain::PersonData& person);
};

}