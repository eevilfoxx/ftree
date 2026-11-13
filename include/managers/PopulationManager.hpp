// PopulationManager.hpp
#ifndef POPULATION_MANAGER_HPP
#define POPULATION_MANAGER_HPP

#include <memory>
#include <vector>
#include <entities/repos/IPersonRepository.hpp>
#include <core/EventDispatcher.hpp>
#include <core/Config.hpp>

class PopulationManager {
public:
    PopulationManager(std::shared_ptr<IPersonRepository> personRepo, 
                     std::shared_ptr<EventDispatcher> eventDispatcher);
    
    void addPerson(std::shared_ptr<Person> person);
    void removePerson(int uuid);
    void agePerson(int uuid);
    
    std::vector<std::shared_ptr<Person>> find(const std::function<bool(const Person&)>& predicate);

    bool areRelatives(std::shared_ptr<Person> p1, std::shared_ptr<Person> p2);

    std::vector<std::shared_ptr<Person>> findEligiblePartners(std::shared_ptr<Person> person);
    std::vector<std::shared_ptr<Person>> getLivingPopulation();
    std::vector<std::shared_ptr<Person>> getByAgeRange(int minAge, int maxAge);
    
private:
    std::shared_ptr<IPersonRepository> personRepository;
    std::shared_ptr<EventDispatcher> eventDispatcher;
};

#endif