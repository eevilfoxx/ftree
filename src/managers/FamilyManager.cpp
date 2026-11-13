#include "managers/FamilyManager.hpp"
#include <algorithm>

FamilyManager::FamilyManager(std::shared_ptr<IFamilyRepository> familyRepo,
                             std::shared_ptr<IPersonRepository> personRepo,
                             std::shared_ptr<EventDispatcher> eventDispatcher)
    : familyRepository(familyRepo), personRepository(personRepo), 
      eventDispatcher(eventDispatcher) {}

std::shared_ptr<Family> FamilyManager::createFamily(std::shared_ptr<Person> mother, 
                                                   std::shared_ptr<Person> father) {
    if (!mother || !father || 
        mother->getGender() != Gender::Female || 
        father->getGender() != Gender::Male ||
        !mother->isAlive() || !father->isAlive()) {
        return nullptr;
    }
    
    // Создаем новую семью
    int newFamilyId = familyRepository->getAll().size() + 1;
    auto family = std::make_shared<Family>(newFamilyId, mother, father);
    
    // Устанавливаем семью для родителей
    mother->setFamily(family);
    father->setFamily(family);
    
    // Обновляем информацию о родителях в репозитории
    personRepository->update(mother);
    personRepository->update(father);
    
    // Сохраняем семью в репозитории
    familyRepository->add(family);
    
    // Публикуем событие
    Event event(EventType::FamilyCreated);
    event.setData("family_id", std::to_string(newFamilyId));
    event.setData("mother_id", std::to_string(mother->getUuid()));
    event.setData("mother_name", mother->getName());
    event.setData("father_id", std::to_string(father->getUuid()));
    event.setData("father_name", father->getName());

    eventDispatcher->publish(event);
    
    return family;
}

void FamilyManager::dissolveFamily(int familyId) {
    if (auto family = familyRepository->getById(familyId)) {
        // Освобождаем связи родителей с семьей
        if (auto mother = family->getMother()) {
            mother->setFamily(nullptr);
            personRepository->update(mother);
        }
        
        if (auto father = family->getFather()) {
            father->setFamily(nullptr);
            personRepository->update(father);
        }
        
        // Удаляем семью из репозитория
        familyRepository->remove(familyId);
        
        // Публикуем событие
        Event event(EventType::FamilyDissolved);
        event.setData("family_id", std::to_string(familyId));
        event.setData("mother_id", std::to_string(family->getMother()->getUuid()));
        event.setData("mother_name", family->getMother()->getName());
        event.setData("father_id", std::to_string(family->getFather()->getUuid()));
        event.setData("father_name", family->getFather()->getName());

        eventDispatcher->publish(event);
    }
}

void FamilyManager::addChildToFamily(int familyId, std::shared_ptr<Person> child) {
    if (auto family = familyRepository->getById(familyId)) {
        family->addChild(child);
        familyRepository->update(family);
        
        // Публикуем событие
        Event event(EventType::PersonBorn);
        event.setData("family_id", std::to_string(familyId));
        event.setData("child_id", std::to_string(child->getUuid()));
       
        eventDispatcher->publish(event);
    }
}

void FamilyManager::removeChildFromFamily(int familyId, int childId) {
    if (auto family = familyRepository->getById(familyId)) {
        if (auto child = personRepository->getById(childId)) {
            family->removeChild(child);
            familyRepository->update(family);

            Event event(EventType::PersonMoved);
            event.setData("family_id", std::to_string(familyId));
            event.setData("child_id", std::to_string(child->getUuid()));
        
            eventDispatcher->publish(event);
        }
    }
}

std::vector<std::shared_ptr<Family>> FamilyManager::getAllFamilies() {
    return familyRepository->getAll();
}

std::vector<std::shared_ptr<Person>> FamilyManager::getFamilyMembers(int familyId) {
    if (auto family = familyRepository->getById(familyId)) {
        auto members = family->getChildren();
        
        if (auto mother = family->getMother()) {
            members.push_back(mother);
        }
        
        if (auto father = family->getFather()) {
            members.push_back(father);
        }
        
        return members;
    }
    
    return {};
}