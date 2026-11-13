// BirthService.cpp
#include <services/BirthService.hpp>
#include <entities/Person.hpp>

BirthService::BirthService(std::shared_ptr<PopulationManager> populationManager,
                           std::shared_ptr<FamilyManager> familyManager,
                           std::shared_ptr<IPersonRepository> personRepository,
                           std::shared_ptr<EventDispatcher> eventDispatcher)
    : populationManager_(populationManager), familyManager_(familyManager),
      personRepository_(personRepository), eventDispatcher_(eventDispatcher) {}

void BirthService::processBirths() {
    auto pregnantWomen = populationManager_->find([](const Person& person) {
        return person.isAlive() && person.isPregnant() && person.getGender() == Gender::Female;
    });
    
    for (auto& mother : pregnantWomen) {
        auto family = mother->getFamily();
        if (!family) {
            std::cerr << "Warning: Pregnant woman " << mother->getUuid() 
                      << " has no family\n";
            continue;
        }
        
        auto father = family->getFather();
        if (!father) {
            std::cerr << "Warning: Pregnant woman " << mother->getUuid() 
                      << " has no father in family\n";
            continue;
        }
        
        auto& config = Config::getInstance();
        double probTriple = config.getDouble("prob_triple", 0.01);
        double probTwin = config.getDouble("prob_twin", 0.09);
        
        double r = RandomEngine::getInstance().randomDouble(0, 1);
        int numChildren = 1;
        
        if (r < probTriple) {
            numChildren = 3;
        } else if (r < probTriple + probTwin) {
            numChildren = 2;
        }
        
        for (int i = 0; i < numChildren; i++) {
            Gender childGender = RandomEngine::getInstance().randomBool() ? 
                                Gender::Male : Gender::Female;
            
            int childId = personRepository_->getAll().size() + 1;
            auto child = std::make_shared<Person>(childId, childGender, mother, father);
            
            populationManager_->addPerson(child);
            familyManager_->addChildToFamily(family->getId(), child);
                   
            Event bornEvent(EventType::PersonBorn);
            bornEvent.setData("person_id", std::to_string(child->getUuid()));
            bornEvent.setData("person_name", child->getName());
            eventDispatcher_->publish(bornEvent);

            Event birthEvent(EventType::Childbirth);
            birthEvent.setData("child_id", std::to_string(child->getUuid()));
            birthEvent.setData("mother_id", std::to_string(mother->getUuid()));
            birthEvent.setData("father_id", std::to_string(father->getUuid()));
            birthEvent.setData("child_name", child->getName());
            birthEvent.setData("family_name", family->getSurname());
            eventDispatcher_->publish(birthEvent);
        }
        
        mother->setPregnant(false);
    }
}