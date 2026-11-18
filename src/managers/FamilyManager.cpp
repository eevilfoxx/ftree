#include "managers/FamilyManager.hpp"

#include <limits>
#include <stdexcept>
#include <utility>

FamilyManager::FamilyManager(std::shared_ptr<IFamilyRepository> familyRepo,
                             std::shared_ptr<IPersonRepository> personRepo,
                             std::shared_ptr<EventDispatcher> eventDispatcher)
    : familyRepository(std::move(familyRepo)),
      personRepository(std::move(personRepo)),
      eventDispatcher(std::move(eventDispatcher)) {}

std::shared_ptr<Family> FamilyManager::createFamily(
    std::shared_ptr<Person> mother,
    std::shared_ptr<Person> father) {
    if (!mother || !father || mother == father
        || mother->getGender() != Gender::Female
        || father->getGender() != Gender::Male
        || !mother->isAlive() || !father->isAlive()
        || mother->isMarried() || father->isMarried()) {
        return nullptr;
    }

    int newFamilyId = 1;
    while (familyRepository->getById(newFamilyId)) {
        if (newFamilyId == std::numeric_limits<int>::max()) {
            throw std::overflow_error("No available family IDs");
        }
        ++newFamilyId;
    }

    auto family = std::make_shared<Family>(newFamilyId, mother, father);

    mother->setFamily(family);
    father->setFamily(family);
    personRepository->update(mother);
    personRepository->update(father);
    familyRepository->add(family);

    Event event(EventType::FamilyCreated);
    event.setData("family_id", std::to_string(newFamilyId));
    event.setData("mother_id", std::to_string(mother->getUuid()));
    event.setData("mother_name", mother->getName());
    event.setData("father_id", std::to_string(father->getUuid()));
    event.setData("father_name", father->getName());
    event.setData("family_surname", family->getSurname());
    eventDispatcher->publish(event);

    return family;
}

void FamilyManager::dissolveFamily(int familyId) {
    auto family = familyRepository->getById(familyId);
    if (!family) {
        return;
    }

    const auto mother = family->getMother();
    const auto father = family->getFather();
    const auto children = family->getChildren();

    if (mother && mother->getFamily() == family) {
        mother->setFamily(nullptr);
        personRepository->update(mother);
    }

    if (father && father->getFamily() == family) {
        father->setFamily(nullptr);
        personRepository->update(father);
    }

    for (const auto& child : children) {
        if (child && child->getFamily() == family) {
            child->setFamily(nullptr);
            personRepository->update(child);
        }
    }

    familyRepository->remove(familyId);

    Event event(EventType::FamilyDissolved);
    event.setData("family_id", std::to_string(familyId));
    if (mother) {
        event.setData("mother_id", std::to_string(mother->getUuid()));
        event.setData("mother_name", mother->getName());
    }
    if (father) {
        event.setData("father_id", std::to_string(father->getUuid()));
        event.setData("father_name", father->getName());
    }
    eventDispatcher->publish(event);
}

void FamilyManager::addChildToFamily(int familyId, std::shared_ptr<Person> child) {
    if (!child) {
        return;
    }

    auto family = familyRepository->getById(familyId);
    if (!family || family->hasChild(child)
        || family->getMother() == child || family->getFather() == child) {
        return;
    }

    if (auto previousFamily = child->getFamily()) {
        if (previousFamily != family) {
            // A person who is a parent in another family cannot simultaneously
            // become a child in this family. Existing child membership can move.
            if (!previousFamily->hasChild(child)) {
                return;
            }

            previousFamily->removeChild(child);
            familyRepository->update(previousFamily);

            Event movedEvent(EventType::PersonMoved);
            movedEvent.setData("family_id", std::to_string(previousFamily->getId()));
            movedEvent.setData("child_id", std::to_string(child->getUuid()));
            eventDispatcher->publish(movedEvent);
        }
    }

    family->addChild(child);
    familyRepository->update(family);
    personRepository->update(child);

    Event event(EventType::PersonBorn);
    event.setData("family_id", std::to_string(familyId));
    event.setData("child_id", std::to_string(child->getUuid()));
    eventDispatcher->publish(event);
}

void FamilyManager::removeChildFromFamily(int familyId, int childId) {
    auto family = familyRepository->getById(familyId);
    auto child = personRepository->getById(childId);
    if (!family || !child || !family->hasChild(child)) {
        return;
    }

    family->removeChild(child);
    familyRepository->update(family);
    personRepository->update(child);

    Event event(EventType::PersonMoved);
    event.setData("family_id", std::to_string(familyId));
    event.setData("child_id", std::to_string(child->getUuid()));
    eventDispatcher->publish(event);
}

std::vector<std::shared_ptr<Family>> FamilyManager::getAllFamilies() {
    return familyRepository->getAll();
}

std::vector<std::shared_ptr<Person>> FamilyManager::getFamilyMembers(int familyId) {
    const auto family = familyRepository->getById(familyId);
    if (!family) {
        return {};
    }

    auto members = family->getChildren();
    if (const auto mother = family->getMother()) {
        members.push_back(mother);
    }
    if (const auto father = family->getFather()) {
        members.push_back(father);
    }
    return members;
}
