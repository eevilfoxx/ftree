// DeathService.hpp
#ifndef DEATH_SERVICE_HPP
#define DEATH_SERVICE_HPP

#include "managers/PopulationManager.hpp"
#include "core/EventDispatcher.hpp"

class DeathService {
public:
    DeathService(std::shared_ptr<PopulationManager> populationManager, 
                 std::shared_ptr<EventDispatcher> eventDispatcher);
    void processDeaths();
    
private:
    std::shared_ptr<PopulationManager> populationManager_;
    std::shared_ptr<EventDispatcher> eventDispatcher_;
};

#endif