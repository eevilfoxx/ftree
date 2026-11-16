#include "simulation/AgingSystem.hpp"
#include "core/Random.hpp"

namespace ftree::simulation {

void AgingSystem::ageAllLiving(World& world) {
    auto living = world.getLivingPeople();
    for (PersonId id : living) {
        auto& person = world.getPersonRef(id);
        person.incrementAge();
    }
}

void AgingSystem::agePerson(World& world, PersonId id) {
    if (world.personExists(id)) {
        auto& person = world.getPersonRef(id);
        person.incrementAge();  // Already checks if alive
    }
}
