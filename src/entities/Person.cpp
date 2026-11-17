#include "entities/Person.hpp"
#include "entities/Family.hpp"
#include "core/RandomEngine.hpp"
#include "core/Config.hpp"
#include <cmath>
#include <iostream>

#include "managers/NameManager.hpp"

Person::Person(int uuid, Gender gender, int age)
    : uuid(uuid), gender(gender), age(age), pregnant(false), alive(true),
      name(NameManager::getInstance().getRandomName(gender)) {}

Person::Person(int uuid, Gender gender, std::shared_ptr<Person> mother, 
               std::shared_ptr<Person> father, int age)
    : uuid(uuid), gender(gender), mother(mother), father(father), 
      age(age), pregnant(false), alive(true),
      name(NameManager::getInstance().getRandomName(gender)) {}

bool Person::isMarried() const {
    auto familyPtr = family.lock();
    if (!familyPtr) return false;
    
    auto father = familyPtr->getFather();
    auto mother = familyPtr->getMother();
    
    return (father && father->getUuid() == uuid) ||
           (mother && mother->getUuid() == uuid);
}

void Person::setFamily(std::shared_ptr<Family> family)
{
    this->family = family;
}

void Person::incrementAge() {
    if (alive) {
        age++;
    }
}

void Person::setPregnant(bool isPregnant) {
    if (gender == Gender::Female) {
        pregnant = isPregnant;
    }
}

void Person::setAlive(bool isAlive) {
    alive = isAlive;
    if (!alive) {
        pregnant = false;
    }
}

bool Person::tryConceive(int partnerAge) {
    if (!alive || pregnant || gender != Gender::Female) {
        return false;
    }
    
    if (age < 18 || age > 45) {
        return false;
    }
    
    auto& config = Config::getInstance();
    double baseConceptionProb = config.getDouble("base_conception_prob", 0.3);
    
    auto fertility = [](int age) -> double {
        if (age < 18) return 0.0;
        if (age <= 30) return 1.0;
        if (age < 40) return 0.7;
        if (age < 45) return 0.3;
        return 0.0;
    };
    
    double fert = (fertility(age) + fertility(partnerAge)) / 2.0;
    double probability = fert * baseConceptionProb;
    
    bool conceived = RandomEngine::getInstance().randomDouble(0.0, 1.0) < probability;
    
    if (conceived) {
        this->pregnant = true;
    }
    
    return conceived;
}

bool Person::tryDie() {
    if (!alive) return false;
    
    auto& config = Config::getInstance();
    double mortalityA = config.getDouble("mortality_a", 0.001);
    double mortalityB = config.getDouble("mortality_b", 0.0693);
    
    double deathProb = mortalityA * std::exp(mortalityB * age);
    if (RandomEngine::getInstance().randomDouble(0.0, 1.0) < deathProb) {
        alive = false;
        pregnant = false;
        return true;
    }
    
    return false;
}