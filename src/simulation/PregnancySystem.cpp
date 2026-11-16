#include "simulation/PregnancySystem.hpp"
#include "core/RandomEngine.hpp"

namespace ftree::simulation {

bool PregnancySystem::tryConceive(World& world, PersonId motherId, PersonId fatherId) {
    // Get person data
    auto& mother = world.getPersonRef(motherId);
    auto& father = world.getPersonRef(fatherId);
    
    // Check: mother must be female, alive, not already pregnant
    if (mother.gender != ftree::domain::Gender::Female) return false;
    if (!mother.isAlive()) return false;
    if (mother.isPregnant()) return false;
    
    // Age constraints: mother 18-45, father able to conceive
    if (mother.age < 18 || mother.age > 45) return false;
    
    // Use the old conception formula from the original code
    // P(conceive) = fert * base_conception_prob
    // where fert = (fertility(mother_age) + fertility(father_age)) / 2.0
    // fertility(age) = 0 if age < 18; 1.0 if 18 <= age <= 30; 0.7 if 30 < age < 40; 0.3 if 40 <= age < 45; 0.0 if age >= 45
    
    auto fertility = [](int age) -> double {
        if (age < 18) return 0.0;
        if (age <= 30) return 1.0;
        if (age < 40) return 0.7;
        if (age < 45) return 0.3;
        return 0.0;
    };
    
    double fert = (fertility(mother.age) + fertility(father.age)) / 2.0;
    double baseConceptionProb = 0.3; // From config
    
    double probability = fert * baseConceptionProb;
    
    // Use RandomEngine for deterministic behavior
    // In the annual cycle, this would use the core Random with fixed seed
    // For now, use a simple check with a counter
    static unsigned int conceive_counter = 0;
    conceive_counter++;
    
    // Simplified: if probability > threshold based on counter, conceive
    // The full RNG integration is in the annual cycle
    if (conceive_counter % 100 < static_cast<unsigned int>(probability * 100)) {
        // Conception succeeded - mark mother as pregnant
        mother.setPregnant(true);
        return true;
    }
    
    return false;
}

bool PregnancySystem::canConceive(PersonData& person) {
    return person.isAlive && person.gender == ftree::domain::Gender::Female 
        && !person.isPregnant && person.age >= 18 && person.age <= 45;
}
