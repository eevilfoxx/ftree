// MarriageService.cpp
#include <algorithm>
#include "services/MarriageService.hpp"

MarriageService::MarriageService(std::shared_ptr<FamilyManager> familyManager,
                                 std::shared_ptr<PopulationManager> populationManager,
                                 std::shared_ptr<IPersonRepository> personRepository,
                                 std::shared_ptr<EventDispatcher> eventDispatcher)
    : familyManager_(familyManager), populationManager_(populationManager),
      personRepository_(personRepository), eventDispatcher_(eventDispatcher) {}

void MarriageService::processMarriages() {
    auto& config = Config::getInstance();
    double ageDiffFactor = config.getDouble("age_diff_factor", 0.1);
    int minMarriageAge = config.getInt("min_marriage_age", 18);
    int maxPersonAge = config.getInt("max_person_age", 100);

    auto allPersons = populationManager_->getByAgeRange(minMarriageAge, maxPersonAge);
    std::vector<std::shared_ptr<Person>> singlePersons;
    
    for (auto& person : allPersons) {
        if (!person->isMarried()) {
            singlePersons.push_back(person);
        }
    }
    
    if (singlePersons.empty()) return;

    if (singlePersons.size() > 1) {
        try {
            auto rng = RandomEngine::getInstance().getEngine();
            std::shuffle(singlePersons.begin(), singlePersons.end(), rng);
        } catch (const std::exception& e) {
            std::cerr << "Shuffle error: " << e.what() << std::endl;
        }
    }

    std::set<int> marriedPersons;

    for (auto& person : singlePersons) {
        if (marriedPersons.find(person->getUuid()) != marriedPersons.end()) {
            continue;
        }

        auto partners = populationManager_->findEligiblePartners(person);

        for (auto& partner : partners) {
            if (marriedPersons.find(partner->getUuid()) != marriedPersons.end()) {
                continue;
            }

            int ageDiff = std::abs(person->getAge() - partner->getAge());
            double probability = std::exp(-ageDiffFactor * ageDiff);

            if (RandomEngine::getInstance().randomDouble(0, 1) < probability) {
                std::shared_ptr<Person> female, male;

                if (person->getGender() == Gender::Female) {
                    female = person;
                    male = partner;
                } else {
                    male = person;
                    female = partner;
                }

                try {
                    auto newFamily = familyManager_->createFamily(female, male);
                    marriedPersons.insert(person->getUuid());
                    marriedPersons.insert(partner->getUuid());

                    Event marriageEvent(EventType::Marriage);
                    marriageEvent.setData("male_id", std::to_string(male->getUuid()));
                    marriageEvent.setData("female_id", std::to_string(female->getUuid()));
                    marriageEvent.setData("male_name", male->getName());
                    marriageEvent.setData("female_name", female->getName());
                    eventDispatcher_->publish(marriageEvent);

                    Event familyEvent(EventType::FamilyCreated);
                    familyEvent.setData("family_id", std::to_string(newFamily->getId()));
                    familyEvent.setData("family_surname", newFamily->getSurname());
                    eventDispatcher_->publish(familyEvent);
                    
                    break;
                } catch (const std::exception& e) {
                    std::cerr << "ERROR: Failed to create family: " << e.what() << std::endl;
                }
            }
        }
    }
}