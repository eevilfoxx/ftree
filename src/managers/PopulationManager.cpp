#include "managers/PopulationManager.hpp"
#include <algorithm>
#include <iostream>

PopulationManager::PopulationManager(std::shared_ptr<IPersonRepository> personRepo, 
                                     std::shared_ptr<EventDispatcher> eventDispatcher)
    : personRepository(personRepo), eventDispatcher(eventDispatcher) {}

void PopulationManager::addPerson(std::shared_ptr<Person> person) {
    if (person) {
        personRepository->add(person);
        
        Event event(EventType::PersonBorn);
        event.setData("person_id", std::to_string(person->getUuid()));

        eventDispatcher->publish(event);
    }
}

void PopulationManager::removePerson(int uuid) {
    if (auto person = personRepository->getById(uuid)) {
        Event event(EventType::PersonDied);
        event.setData("person_id", std::to_string(uuid));

        eventDispatcher->publish(event);
    }
    
    personRepository->remove(uuid);
}

void PopulationManager::agePerson(int uuid) {
    if (auto person = personRepository->getById(uuid)) {
        person->incrementAge();
        personRepository->update(person);
    }
}

std::vector<std::shared_ptr<Person>> PopulationManager::find(const std::function<bool(const Person&)>& predicate) {
    return personRepository->find(predicate);
}

std::vector<std::shared_ptr<Person>> PopulationManager::findEligiblePartners(std::shared_ptr<Person> person) {
    if (!person || !person->isAlive()) {
        return {};
    }
    
    auto& config = Config::getInstance();
    int minMarriageAge = config.getInt("min_marriage_age", 18);
    int maxPersonAge = config.getInt("max_person_age", 100);

    if (person->getAge() < minMarriageAge || person->isMarried()) {
        return {};
    }
    
    Gender targetGender = (person->getGender() == Gender::Male) ? 
                         Gender::Female : Gender::Male;
    
    auto potentialPartners = personRepository->getByGender(targetGender);
    
    std::vector<std::shared_ptr<Person>> eligiblePartners;
    for (auto& partner : potentialPartners) {
        if (!partner || !partner->isAlive()) {
            continue;
        }

        if (partner->getAge() < minMarriageAge || 
            partner->getAge() > maxPersonAge) {
                continue;
            }

        if (partner->isMarried()) {
                continue;
            }

        if (partner->getUuid() == person->getUuid()) {
            continue;
        }

        if (!areRelatives(person, partner)) {
                eligiblePartners.push_back(partner);
            }
    }
    
    return eligiblePartners;
}

bool PopulationManager::areRelatives(std::shared_ptr<Person> p1, std::shared_ptr<Person> p2) {
    if (!p1 || !p2) return false;
    
    if (p1->getUuid() == p2->getUuid()) return true;
    
    auto p1Mother = p1->getMother();
    auto p1Father = p1->getFather();
    auto p2Mother = p2->getMother();
    auto p2Father = p2->getFather();
    
    if (p1Mother && p2Mother && p1Mother->getUuid() == p2Mother->getUuid()) return true;
    if (p1Father && p2Father && p1Father->getUuid() == p2Father->getUuid()) return true;
    
    return false;
}

std::vector<std::shared_ptr<Person>> PopulationManager::getLivingPopulation() {
    return personRepository->getLiving();
}

std::vector<std::shared_ptr<Person>> PopulationManager::getByAgeRange(int minAge, int maxAge) {
    auto allLiving = personRepository->getLiving();
    std::vector<std::shared_ptr<Person>> result;
    
    for (auto& person : allLiving) {
        if (person->getAge() >= minAge && person->getAge() <= maxAge) {
            result.push_back(person);
        }
    }
    
    return result;
}