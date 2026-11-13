// MarriageService.hpp
#ifndef MARRIAGE_SERVICE_HPP
#define MARRIAGE_SERVICE_HPP

#include "managers/FamilyManager.hpp"
#include "managers/PopulationManager.hpp"
#include "core/EventDispatcher.hpp"
#include "entities/repos/IPersonRepository.hpp"
#include "core/Config.hpp"
#include "core/RandomEngine.hpp"
#include <set>
#include <iostream>

class MarriageService {
public:
    MarriageService(std::shared_ptr<FamilyManager> familyManager,
                    std::shared_ptr<PopulationManager> populationManager,
                    std::shared_ptr<IPersonRepository> personRepository,
                    std::shared_ptr<EventDispatcher> eventDispatcher);
    void processMarriages();
    
private:
    std::shared_ptr<FamilyManager> familyManager_;
    std::shared_ptr<PopulationManager> populationManager_;
    std::shared_ptr<IPersonRepository> personRepository_;
    std::shared_ptr<EventDispatcher> eventDispatcher_;
};

#endif