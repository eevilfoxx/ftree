// PregnancyService.cpp
#include <services/PregnancyService.hpp>
#include <entities/Person.hpp>

PregnancyService::PregnancyService(std::shared_ptr<FamilyManager> familyManager,
                                   std::shared_ptr<IPersonRepository> personRepository,
                                   std::shared_ptr<EventDispatcher> eventDispatcher)
    : familyManager_(familyManager), personRepository_(personRepository), eventDispatcher_(eventDispatcher) {}

void PregnancyService::processPregnancies() {
    auto families = familyManager_->getAllFamilies();
    
    for (auto& family : families) {
        auto motherId = family->getMother()->getUuid();
        auto fatherId = family->getFather()->getUuid();
        
        auto mother = personRepository_->getById(motherId);
        auto father = personRepository_->getById(fatherId);
        
        if (!mother || !father || !mother->isAlive() || !father->isAlive()) {
            continue;
        }
        
        if (mother->isPregnant()) {
            continue;
        }
        
        if (mother->tryConceive(father->getAge())) {
            personRepository_->update(mother);

            Event pregnancyEvent(EventType::Pregnancy);
            pregnancyEvent.setData("mother_id", std::to_string(mother->getUuid()));
            pregnancyEvent.setData("mother_name", mother->getName());
            pregnancyEvent.setData("mother_age", std::to_string(mother->getAge()));
            pregnancyEvent.setData("father_id", std::to_string(father->getUuid()));
            pregnancyEvent.setData("father_name", father->getName());
            pregnancyEvent.setData("father_age", std::to_string(father->getAge()));

            eventDispatcher_->publish(pregnancyEvent);
        }
    }
}