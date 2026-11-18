#include "simulation/AgingSystem.hpp"

namespace ftree::simulation {

void AgingSystem::ageAllLiving(domain::World& world) {
    for (domain::PersonId id : world.getLivingPeople()) {
        agePerson(world, id);
    }
}

void AgingSystem::agePerson(domain::World& world, domain::PersonId id) {
    if (!world.personExists(id)) {
        return;
    }

    auto& person = world.getPersonRef(id);
    if (person.alive) {
        ++person.age;
    }
}

} // namespace ftree::simulation
