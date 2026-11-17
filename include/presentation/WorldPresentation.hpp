#pragma once

#include "presentation/PersonDisplay.hpp"
#include "presentation/FamilyDisplay.hpp"
#include "domain/World.hpp"
#include <vector>

namespace ftree::presentation {

class WorldPresentation {
public:
    // Get all people as display format
    static std::vector<PersonDisplay> getAllPeople(const World& world);

    // Get all families as display format
    static std::vector<FamilyDisplay> getAllFamilies(const World& world);

    // Get people living
    static std::vector<PersonId> getLivingPeopleIds(const World& world);

    // Get family members
    static std::vector<PersonId> getFamilyMembers(const World& world, FamilyId familyId);
};

} // namespace ftree::presentation
