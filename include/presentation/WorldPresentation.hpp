
#pragma once

#include "domain/World.hpp"
#include "presentation/FamilyDisplay.hpp"
#include "presentation/PersonDisplay.hpp"

#include <vector>

namespace ftree::presentation {

class WorldPresentation {
public:
    // Returns all people, including deceased people.
    // Results are sorted by person ID.
    static std::vector<PersonDisplay> getAllPeople(
        const domain::World& world
    );

    // Returns all families.
    // Results are sorted by family ID.
    static std::vector<FamilyDisplay> getAllFamilies(
        const domain::World& world
    );

    // Returns IDs of living people.
    static std::vector<domain::PersonId> getLivingPeopleIds(
        const domain::World& world
    );

    // Returns members of a family.
    static std::vector<domain::PersonId> getFamilyMembers(
        const domain::World& world,
        domain::FamilyId familyId
    );

    // Returns display data for one person.
    // Throws std::out_of_range if the person does not exist.
    static PersonDisplay getPersonData(
        const domain::World& world,
        domain::PersonId id
    );
};

} // namespace ftree::presentation
