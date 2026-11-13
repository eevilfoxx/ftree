// StatisticsService.hpp
#ifndef STATISTICS_SERVICE_HPP
#define STATISTICS_SERVICE_HPP

#include "managers/PopulationManager.hpp"
#include "managers/FamilyManager.hpp"
#include <iostream>

class StatisticsService {
public:
    StatisticsService(std::shared_ptr<PopulationManager> populationManager,
                      std::shared_ptr<FamilyManager> familyManager);
    void printStatistics(int year) const;
    
private:
    std::shared_ptr<PopulationManager> populationManager_;
    std::shared_ptr<FamilyManager> familyManager_;
};

#endif