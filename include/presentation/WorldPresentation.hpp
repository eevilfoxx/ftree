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
    static std::vector<domain::PersonId> getLivingPeopleIds(const ftree::domain::World& world);

    // Get family members
    static std::vector<domain::PersonId> getFamilyMembers(const ftree::domain::World& world, domain::FamilyId familyId);

    // Get person data for display
    static PersonDisplay getPersonData(const ftree::domain::World& world,domain::PersonId id);
};

}