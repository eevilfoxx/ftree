#include "simulation/MarriageSystem.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

namespace ftree::simulation {
namespace {

bool areCloseRelatives(const domain::PersonData& first,
                       const domain::PersonData& second) {
    if (first.id == second.id) {
        return true;
    }

    const bool firstIsParent =
        first.id == second.motherId || first.id == second.fatherId;
    const bool secondIsParent =
        second.id == first.motherId || second.id == first.fatherId;
    if (firstIsParent || secondIsParent) {
        return true;
    }

    const bool shareMother =
        first.motherId != 0 && first.motherId == second.motherId;
    const bool shareFather =
        first.fatherId != 0 && first.fatherId == second.fatherId;
    return shareMother || shareFather;
}

bool isEligible(const domain::PersonData& person,
                domain::Gender expectedGender,
                const core::SimulationConfig& config) {
    return person.alive
        && person.gender == expectedGender
        && person.familyId == 0
        && person.age >= config.min_marriage_age
        && person.age <= config.max_person_age;
}

} // namespace

bool MarriageSystem::tryMarry(domain::World& world,
                              domain::PersonId maleId,
                              domain::PersonId femaleId,
                              const core::SimulationConfig& config,
                              core::Random& random,
                              const std::string& surname) {
    if (!world.personExists(maleId) || !world.personExists(femaleId)) {
        return false;
    }

    const auto& male = world.getPersonRef(maleId);
    const auto& female = world.getPersonRef(femaleId);

    if (!isEligible(male, domain::Gender::Male, config)
        || !isEligible(female, domain::Gender::Female, config)
        || areCloseRelatives(male, female)) {
        return false;
    }

    const int ageDifference = std::abs(male.age - female.age);
    const double probability =
        std::exp(-config.age_diff_factor * static_cast<double>(ageDifference));

    if (random.randomDouble(0.0, 1.0) >= probability) {
        return false;
    }

    world.createFamily(femaleId, maleId, surname);
    return true;
}

int MarriageSystem::processMarriages(domain::World& world,
                                     const core::SimulationConfig& config,
                                     core::Random& random,
                                     const core::NameDatabase& names) {
    std::vector<domain::PersonId> males;
    std::vector<domain::PersonId> females;

    for (domain::PersonId id : world.getLivingPeople()) {
        const auto& person = world.getPersonRef(id);
        if (isEligible(person, domain::Gender::Male, config)) {
            males.push_back(id);
        } else if (isEligible(person, domain::Gender::Female, config)) {
            females.push_back(id);
        }
    }

    std::shuffle(males.begin(), males.end(), random.engine());
    std::shuffle(females.begin(), females.end(), random.engine());

    int marriages = 0;
    for (domain::PersonId maleId : males) {
        for (domain::PersonId femaleId : females) {
            if (tryMarry(world, maleId, femaleId, config, random,
                         names.getRandomSurname())) {
                ++marriages;
                break;
            }
        }
    }

    return marriages;
}

} // namespace ftree::simulation
