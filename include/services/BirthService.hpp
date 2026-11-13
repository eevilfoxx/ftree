// BirthService.hpp
#ifndef BIRTH_SERVICE_HPP
#define BIRTH_SERVICE_HPP

#include "managers/PopulationManager.hpp"
#include "managers/FamilyManager.hpp"
#include "entities/repos/IPersonRepository.hpp"
#include "core/EventDispatcher.hpp"
#include "core/RandomEngine.hpp"
#include <iostream>


class BirthService {
public:
    BirthService(std::shared_ptr<PopulationManager> populationManager,
                 std::shared_ptr<FamilyManager> familyManager,
                 std::shared_ptr<IPersonRepository> personRepository,
                 std::shared_ptr<EventDispatcher> eventDispatcher);
    void processBirths();
    
private:
    std::shared_ptr<PopulationManager> populationManager_;
    std::shared_ptr<FamilyManager> familyManager_;
    std::shared_ptr<IPersonRepository> personRepository_;
    std::shared_ptr<EventDispatcher> eventDispatcher_;
};

#endif