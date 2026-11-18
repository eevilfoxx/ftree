#include "simulation/DeathSystem.hpp"



#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace ftree::simulation {

int DeathSystem::processDeaths(domain::World& world,
                               double mortality_a,
                               double mortality_b,
                               core::Random& random) {
    if (!std::isfinite(mortality_a) || !std::isfinite(mortality_b)
        || mortality_a < 0.0 || mortality_b < 0.0) {
        throw std::invalid_argument("Mortality parameters must be finite and non-negative");
    }

    int deaths = 0;

    for (domain::PersonId id : world.getLivingPeople()) {
        const auto& person = world.getPersonRef(id);
        const double rawProbability =
            mortality_a * std::exp(mortality_b * static_cast<double>(person.age));
        const double deathProbability =
            std::clamp(rawProbability, 0.0, 1.0);

        if (random.randomDouble(0.0, 1.0)
            < deathProbability) {
            killPerson(world, id);
            ++deaths;
        }
    }

    return deaths;
}

void DeathSystem::killPerson(domain::World& world, domain::PersonId id) {
    if (world.personExists(id)) {
        world.killPerson(id);
    }
}

} // namespace ftree::simulation
