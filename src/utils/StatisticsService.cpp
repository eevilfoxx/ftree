// StatisticsService.cpp
#include "utils/StatisticsService.hpp"

StatisticsService::StatisticsService(std::shared_ptr<PopulationManager> populationManager,
                                     std::shared_ptr<FamilyManager> familyManager)
    : populationManager_(populationManager), familyManager_(familyManager) {}

void StatisticsService::printStatistics(int year) const {
    auto living = populationManager_->getLivingPopulation();
    auto families = familyManager_->getAllFamilies();
    
    std::cout << "Year " << year << ":\n";
    std::cout << "  Population: " << living.size() << "\n";
    std::cout << "  Families: " << families.size() << "\n";
    
    int children = 0, adults = 0, elders = 0;
    for (auto& person : living) {
        if (person->getAge() < 18) children++;
        else if (person->getAge() < 60) adults++;
        else elders++;
    }
    
    std::cout << "  Children: " << children << ", Adults: " << adults << ", Elders: " << elders << "\n";
}