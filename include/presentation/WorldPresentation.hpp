#pragma once

#include "presentation/PersonDisplay.hpp"
#include "presentation/FamilyDisplay.hpp"
#include "domain/World.hpp"
#include <vector>

namespace ftree::presentation {

class WorldPresentation {
public:
    // Get all people as display format
    static std::vector<PersonDisplay> getAllPeople(const ftree::domain::World& world);

    // Get all families as display format
    static std::vector<FamilyDisplay> getAllFamilies(const ftree::domain::World& world);

    // Get people living
    static std::vector<PersonId> getLivingPeopleIds(const ftree::domain::World& world);

    // Get family members
    static std::vector<PersonId> getFamilyMembers(const ftree::domain::World& world, FamilyId familyId);

    // Get person data for display
    static PersonDisplay getPersonData(domain::PersonId id);

    // Get person data for display
    static PersonDisplay getPersonData(domain::PersonId id);
};

}