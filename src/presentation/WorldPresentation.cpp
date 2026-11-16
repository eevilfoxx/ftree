#include "presentation/WorldPresentation.hpp"
#include "core/World.hpp"
#include <algorithm>

namespace ftree::presentation {

std::vector<PersonDisplay> WorldPresentation::getAllPeople(const World& world) {
    std::vector<PersonDisplay> result;
    auto people = world.getAllPeopleIncludingDead();
    
    int genLevel = 0;
    for (const auto& [id, data] : people) {
        PersonDisplay pd;
        pd.id = data.id;
        pd.name = data.name;
        pd.displayName = data.name + " " + std::to_string(data.age) + " yrs";
        pd.isMale = data.gender == ftree::domain::Gender::Male;
        pd.isAlive = data.alive;
        pd.isPregnant = data.pregnant;
        pd.isDeceased = !data.alive;
        pd.motherId = data.motherId;
        pd.fatherId = data.fatherId;
        pd.familyId = data.familyId;
        pd.generationLevel = genLevel;
        pd.depth = 0;  // will be set by layout
        result.push_back(pd);
    }
    
    return result;
}

std::vector<FamilyDisplay> WorldPresentation::getAllFamilies(const World& world) {
    std::vector<FamilyDisplay> result;
    auto families = world.getAllFamilies();
    
    for (const auto& [id, data] : families) {
        FamilyDisplay fd;
        fd.id = data.id;
        fd.motherId = data.motherId;
        fd.fatherId = data.fatherId;
        fd.childrenIds = data.childrenIds;
        fd.surname = data.surname;
        fd.generationLevel = 0;
        fd.memberCount = 0;  // will be calculated
        
        // Count members
        if (fd.motherId > 0) fd.memberCount++;
        if (fd.fatherId > 0) fd.memberCount++;
        fd.memberCount += static_cast<int>(fd.childrenIds.size());
        
        result.push_back(fd);
    }
    
    return result;
}

std::vector<PersonId> WorldPresentation::getLivingPeopleIds(const World& world) {
    return world.getLivingPeople();
}

std::vector<PersonId> WorldPresentation::getFamilyMembers(const World& world, FamilyId familyId) {
    return world.getFamilyMembers(familyId);
}
