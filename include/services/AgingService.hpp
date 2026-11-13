// AgingService.hpp
#ifndef AGING_SERVICE_HPP
#define AGING_SERVICE_HPP

#include "managers/PopulationManager.hpp"

class AgingService {
public:
    explicit AgingService(std::shared_ptr<PopulationManager> populationManager);
    void processAging();
    
private:
    std::shared_ptr<PopulationManager> populationManager_;
};

#endif