#include "simulation/BirthSystem.hpp"

#include <stdexcept>
#include <cmath>


namespace ftree::simulation {

int BirthSystem::processBirths(domain::World& world,
                               double probTriple,
                               double probTwin,
                               core::NameDatabase& nameDb,
                               core::Random& random) {
    if (!std::isfinite(probTriple) || !std::isfinite(probTwin)
        || probTriple < 0.0 || probTwin < 0.0
        || probTriple > 1.0 || probTwin > 1.0
        || probTriple + probTwin > 1.0) {
        throw std::invalid_argument("Birth probabilities must be in [0,1] and sum to at most 1");
    }
    int births = 0;
    const auto livingPeople = world.getLivingPeople();

    for (domain::PersonId motherId : livingPeople) {
        if (!world.personExists(motherId)) {
            continue;
        }

        const auto& mother = world.getPersonRef(motherId);
        if (!mother.alive || !mother.pregnant
            || mother.gender != domain::Gender::Female) {
            continue;
        }

        const double roll = random.randomDouble(0.0, 1.0);
        const int childCount = roll < probTriple
            ? 3
            : (roll < probTriple + probTwin ? 2 : 1);

        for (int i = 0; i < childCount; ++i) {
            if (giveBirth(world, motherId, nameDb.getRandomSurname(random), nameDb, random) != 0) {
                ++births;
            }
        }

        resetPregnancy(world, motherId);
    }

    return births;
}

domain::PersonId BirthSystem::giveBirth(
                        domain::World& world,
                        domain::PersonId motherId,
                        const std::string& surname,
                        core::NameDatabase& nameDb,
                        core::Random& random) {
    if (!world.personExists(motherId)) {
        return 0;
    }

    const auto& mother = world.getPersonRef(motherId);
    if (!mother.alive || !mother.pregnant
        || mother.gender != domain::Gender::Female) {
        return 0;
    }

    domain::PersonId fatherId = 0;
    domain::FamilyId familyId = mother.familyId;
    if (familyId != 0 && world.familyExists(familyId)) {
        fatherId = world.getFamilyRef(familyId).fatherId;
    } else {
        familyId = 0;
    }

    const auto childGender = random.randomBool()
        ? domain::Gender::Male
        : domain::Gender::Female;

    bool isMale = (childGender == domain::Gender::Male) ? true : false;

    const domain::PersonId childId =
        world.createPerson(nameDb.getRandomName(isMale, random), childGender, 0);

    auto& child = world.getPersonRef(childId);
    child.motherId = motherId;
    child.fatherId = fatherId;
    // familyId represents the person's current partner family, not their
    // family of origin. Parent-child membership lives in FamilyData::childrenIds.
    child.familyId = 0;

    if (familyId != 0) {
        auto& family = world.getFamilyRef(familyId);
        family.childrenIds.push_back(childId);
    }

    return childId;
}

void BirthSystem::resetPregnancy(domain::World& world,
                                 domain::PersonId motherId) {
    if (world.personExists(motherId)) {
        world.getPersonRef(motherId).pregnant = false;
    }
}

} // namespace ftree::simulation
