#pragma once

#include "core/Random.hpp"
#include "core/SimulationConfig.hpp"
#include "core/NameDatabase.hpp"
#include "domain/World.hpp"

namespace ftree::simulation {

class MarriageSystem {
public:
    // Match eligible single adults and create families.
    // Returns the number of marriages created during this step.
    static int processMarriages(domain::World& world,
                                const core::SimulationConfig& config,
                                core::Random& random,
                                const core::NameDatabase& names);

    // Attempt one male/female match using the configured age-difference probability.
    static bool tryMarry(domain::World& world,
                         domain::PersonId maleId,
                         domain::PersonId femaleId,
                         const core::SimulationConfig& config,
                         core::Random& random,
                         const std::string& surname);
};

} // namespace ftree::simulation
