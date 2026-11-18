#include "simulation/PregnancySystem.hpp"



#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace ftree::simulation {

bool PregnancySystem::tryConceive(domain::World& world,
                                  domain::PersonId motherId,
                                  domain::PersonId fatherId,
                                  double baseConceptionProbability,
                                  core::Random& random) {
    if (!std::isfinite(baseConceptionProbability)
        || baseConceptionProbability < 0.0
        || baseConceptionProbability > 1.0) {
        throw std::invalid_argument("Base conception probability must be in [0,1]");
    }

    if (!world.personExists(motherId) || !world.personExists(fatherId)) {
        return false;
    }

    auto& mother = world.getPersonRef(motherId);
    const auto& father = world.getPersonRef(fatherId);

    if (!canConceive(mother)
        || !father.alive
        || father.gender != domain::Gender::Male) {
        return false;
    }

    const auto fertility = [](int age) -> double {
        if (age < 18) return 0.0;
        if (age <= 30) return 1.0;
        if (age < 40) return 0.7;
        if (age < 45) return 0.3;
        return 0.0;
    };

    const double fertilityFactor =
        (fertility(mother.age) + fertility(father.age)) / 2.0;
    const double probability = std::clamp(
        fertilityFactor * baseConceptionProbability, 0.0, 1.0);

    if (random.randomDouble(0.0, 1.0) < probability) {
        mother.pregnant = true;
        return true;
    }

    return false;
}

bool PregnancySystem::canConceive(const domain::PersonData& person) {
    return person.alive
        && person.gender == domain::Gender::Female
        && !person.pregnant
        && person.age >= 18
        && person.age <= 45;
}

} // namespace ftree::simulation
