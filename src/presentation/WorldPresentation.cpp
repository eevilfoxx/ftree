#include "presentation/WorldPresentation.hpp"
#include "domain/World.hpp"
#include <stdexcept>
#include <string>

namespace ftree::presentation {

std::vector<PersonDisplay> WorldPresentation::getAllPeople(const ftree::domain::World& world) {
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

std::vector<FamilyDisplay> WorldPresentation::getAllFamilies(const ftree::domain::World& world) {
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

std::vector<PersonId> WorldPresentation::getLivingPeopleIds(const ftree::domain::World& world) {
    return world.getLivingPeople();
}

std::vector<PersonId> WorldPresentation::getFamilyMembers(const ftree::domain::World& world, FamilyId familyId) {
    return world.getFamilyMembers(familyId);
}

PersonDisplay WorldPresentation::getPersonData(
    const ftree::domain::World& world,
    domain::PersonId id
) {
    const auto person = world.getPerson(id);

    if (!person.has_value()) {
        throw std::out_of_range(
            "Person with ID " + std::to_string(id) + " does not exist"
        );
    }

    const auto& data = person.value();

    PersonDisplay result;
    result.id = data.id;
    result.name = data.name;
    result.displayName = data.name + " " + std::to_string(data.age) + " yrs";
    result.isMale = data.gender == ftree::domain::Gender::Male;
    result.isAlive = data.alive;
    result.isPregnant = data.pregnant;
    result.isDeceased = !data.alive;
    result.motherId = data.motherId;
    result.fatherId = data.fatherId;
    result.familyId = data.familyId;
    result.generationLevel = 0;
    result.depth = 0;

    return result;
}

}