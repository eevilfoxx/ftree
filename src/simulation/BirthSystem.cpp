#include "simulation/BirthSystem.hpp"

namespace ftree::simulation {

int BirthSystem::processBirths(World& world) {
    int births = 0;
    auto living = world.getLivingPeople();
    
    for (PersonId id : living) {
        auto& person = world.getPersonRef(id);
        if (person.isPregnant) {
            // Give birth - create child and add to family
            PersonId childId = world.createPerson("Newborn", 
                mother.isPregnant ? ftree::domain::Gender::Female : ftree::domain::Gender::Male, 0);
            
            if (mother.familyId > 0) {
                auto& family = world.getFamilyRef(mother.familyId);
                family.addChild(childId);
            }
            
            // Reset pregnancy
            mother.setPregnant(false);
            births++;
        }
    }
    
    return births;
}

PersonId BirthSystem::giveBirth(World& world, PersonId motherId, const std::string& surname) {
    return 0;
}

void BirthSystem::resetPregnancy(World& world, PersonId motherId) {
    auto& mother = world.getPersonRef(motherId);
    mother.setPregnant(false);
}
