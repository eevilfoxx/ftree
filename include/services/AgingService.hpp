// AgingService.hpp
#ifndef AGING_SERVICE_HPP
#define AGING_SERVICE_HPP

#include "managers/PopulationManager.hpp"
#include "entities/repos/IPersonRepository.hpp"

class AgingService {
public:
    AgingService(std::shared_ptr<PopulationManager> populationManager,
                 std::shared_ptr<IPersonRepository> personRepository);
    void processAging();
    
private:
    std::shared_ptr<PopulationManager> populationManager_;
    std::shared_ptr<IPersonRepository> personRepository_;
};

#endif